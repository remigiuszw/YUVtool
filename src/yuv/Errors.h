#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <stdexcept>
#include <string>

class GeneralError : public std::exception
{
public:
    GeneralError(const std::string &file_name);
    const char* what() const throw();
private:
    std::string m_description;
};

template<typename Exception = std::runtime_error, typename Argument>
void my_assert(const bool test, const Argument &argument)
{
    if(!test)
        throw Exception(argument);
}

inline void check_range(int begin, int value, int end)
{
    my_assert<std::out_of_range>(
            begin <= value && value < end,
            "YUVtool variable out of range");
}

#define MY_ASSERT(test) \
    my_assert( \
        test, \
        "Assertion failed: (" # test \
        "), but the programmer was to lazy to explain what it means");

#endif // ERRORS_H
