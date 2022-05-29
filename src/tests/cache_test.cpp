/* 
 * Copyright 2016 Dominik Wójt
 * 
 * This file is part of YUVtool.
 * 
 * YUVtool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * YUVtool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with YUVtool.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <memory>
#include <yuv/Cache.h>

#include <gtest/gtest.h>

#include <iostream>
#include <stdexcept>
#include <random>
#include <utility>

using namespace YUV_tool;

template<typename Iterator>
void randomize(
        Iterator begin,
        Iterator end,
        const int seed,
        const int range = 1000000)
{
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0, range - 1);
    for(auto i = begin; i != end; ++i)
    {
        *i = dis(gen);
    }
}

template<typename TData>
class Primitive_cache
{
public:
    using Data = TData;
    using Key = int;
    using Time = int;

private:
    struct Slot
    {
        Key m_key = -1;
        Time m_age = -1;
        std::optional<Data> m_data;
    };

private:
    std::vector<Slot> m_slots;

public:
    Primitive_cache(const Index cache_size) :
        m_slots(cache_size)
    { }

    Index get_cache_size() const
    {
        return m_slots.size();
    }

    std::optional<Data> get(const Key key)
    {
        return get_or_update(key, false);
    }

    void update_key(const Key key)
    {
        Data *data = get_or_update(key, true);
        my_assert(data, "trying to update absent key");
    }

    /* more efficient if done at once */
    std::optional<Data> get_and_update(const Key key)
    {
        return get_or_update(key, true);
    }

    Data pop()
    {
        auto iter =
                std::max_element(
                    m_slots.begin(),
                    m_slots.end(),
                    [](const Slot lhs, const Slot rhs)
                    {
                        return lhs.m_age < rhs.m_age;
                    });
        Slot &slot = *iter;

        my_assert(slot.m_data.has_value(), "trying to pop empty cache");

        Data result = slot.m_data.value();
        slot = Slot();

        return std::move(result);
    }

    void push(const Key key, Data &&data)
    {
        auto iter =
                std::find_if(
                    m_slots.begin(),
                    m_slots.end(),
                    [key](const Slot slot)
                    {
                        return slot.m_key == key;
                    });
        if(iter != m_slots.end())
        {
            my_assert(false, "trying to push data already present in cache");
        }
        else
        {
            auto empty_iter =
                    std::find_if(
                        m_slots.begin(),
                        m_slots.end(),
                        [](const Slot &slot)
                        {
                            return !slot.m_data.has_value();
                        });
            my_assert(
                        empty_iter != m_slots.end(),
                        "trying to push more data than cache can hold");

            Slot &slot = *empty_iter;
            slot.m_data = std::move(data);
            slot.m_key = key;
            update(slot);
        }
    }

    bool full() const
    {
        auto empty_iter =
                std::find_if(
                    m_slots.begin(),
                    m_slots.end(),
                    [](const Slot slot)
                    {
                        return !slot.m_data.has_value();
                    });
        return empty_iter == m_slots.end();
    }

private:
    std::optional<Data> get_or_update(const Key key, bool do_update)
    {
        auto iter = std::find_if(
            m_slots.begin(),
            m_slots.end(),
            [key](const Slot slot) {
                return slot.m_key == key && slot.m_data.has_value();
            });
        if(iter != m_slots.end())
        {
            Slot &slot = *iter;
            if(do_update)
                update(slot);
            return slot.m_data;
        }
        else
        {
            return std::nullopt;
        }
    }

    void update(Slot &slot)
    {
        increment_time();
        slot.m_age = 0;
    }

    void increment_time()
    {
        for(Slot &slot : m_slots)
            if(slot.m_data)
                slot.m_age++;
    }
};

TEST(cache_test, sort)
{
    const Index n = 100000;
    for (int seed = 15; seed < 20; seed++)
    {
        std::vector<int> test_std(n);
        randomize(test_std.begin(), test_std.end(), seed);
        std::vector<int> test_custom = test_std;

        {
            Timer std_timer("custom");
            my_make_heap(
                        test_custom.begin(),
                        test_custom.end(),
                        std::less<int>());
            my_sort_heap(
                        test_custom.begin(),
                        test_custom.end(),
                        std::less<int>());
        }

        {
            Timer std_timer("std");
            std::make_heap(
                        test_std.begin(),
                        test_std.end(),
                        std::less<int>());
            std::sort_heap(
                        test_std.begin(),
                        test_std.end(),
                        std::less<int>());
        }

        {
            Timer std_timer("custom");
            my_make_heap(
                        test_custom.begin(),
                        test_custom.end(),
                        std::less<int>());
            my_sort_heap(
                        test_custom.begin(),
                        test_custom.end(),
                        std::less<int>());
        }

        {
            Timer std_timer("std");
            std::make_heap(
                        test_std.begin(),
                        test_std.end(),
                        std::less<int>());
            std::sort_heap(
                        test_std.begin(),
                        test_std.end(),
                        std::less<int>());
        }

        EXPECT_EQ(test_std, test_custom);
    }
}

TEST(cache_test, push_get)
{
    const int cache_size = 3000;
    const int key_range = 20000;

    int resources[cache_size] = {1000, 1001, 1002};

    std::vector<int> tests(2 * cache_size);
    randomize(tests.begin(), tests.end(), 15, key_range);

    std::vector<std::optional<int *>> advanced_results;
    std::vector<std::optional<int *>> primitive_results;
    advanced_results.reserve(5 * cache_size);
    primitive_results.reserve(5 * cache_size);

    {
        Timer timer("advanced");
        Cache<int, int *> advanced_cache(cache_size);
        for(int test : tests)
        {
            std::optional<int*> sought_resource =
                advanced_cache.get_and_update(test);
            advanced_results.push_back(sought_resource);

            if(sought_resource.has_value())
            {
            }
            else
            {
                if(advanced_cache.is_full())
                    sought_resource = advanced_cache.pop();
                else
                    sought_resource = &resources[test % cache_size];

                advanced_cache.push(
                    test, static_cast<int*>(sought_resource.value()));
            }
        }
    }

    {
        Timer timer("primitive");
        Primitive_cache<int *> primitive_cache(cache_size);
        for(int test : tests)
        {
            std::optional<int *> sought_resource =
                    primitive_cache.get_and_update(test);
            primitive_results.push_back(sought_resource);

            if(sought_resource)
            {
            }
            else
            {
                if(primitive_cache.full())
                    sought_resource = primitive_cache.pop();
                else
                    sought_resource = &resources[test % cache_size];

                primitive_cache.push(
                    test, static_cast<int*>(sought_resource.value()));
            }
        }
    }

    EXPECT_EQ(advanced_results, primitive_results);
}

TEST(cache_test, push_over_size)
{
    const int cache_size = 3;
    Cache<int, int> cache_test(cache_size);

    int resources[cache_size] = {1000, 1001, 1002};
    cache_test.push(3, int{resources[0]});
    cache_test.push(7, int{resources[1]});
    cache_test.push(5, int{resources[2]});

    std::pair<int, std::optional<int>> tests[] = {
        {3, resources[0]},
        {4, std::nullopt},
        {7, std::nullopt},
        {4, resources[1]}
    };

    for(auto test : tests)
    {
        std::optional<int> sought_resource =
            cache_test.get_and_update(test.first);
        EXPECT_EQ(sought_resource, test.second);
        if(sought_resource.has_value())
        {
            /* resource found, just use it */
        }
        else
        {
            /* resource not found in the cache */
            /* need to produce the resource, fortunately we can reuse memory
             * allocated for the resource which is now removed from the
             * cache */
            int recycled_resource = cache_test.pop();
            cache_test.push(test.first, std::move(recycled_resource));
        }
        /* use the resource */
    }

    bool test_passed = false;
    try
    {
        cache_test.push(2, int{resources[0]});
    }
    catch(std::runtime_error &)
    {
        test_passed = true;
    }
    EXPECT_TRUE(test_passed);
}

TEST(cache_test, unique_pointers)
{
    Cache<int, std::unique_ptr<double>> cache(10);

    cache.push(3, std::make_unique<double>(2));
    cache.push(9, std::make_unique<double>(99));

    std::unique_ptr<double> &x = cache.get(3).value();

    EXPECT_EQ(*x, 2);
}