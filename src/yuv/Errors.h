#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <stdexcept>
#include <string>

class GeneralError : public std::exception
{
public:
    GeneralError( const std::string &file_name );
    const char* what() const throw();
private:
    std::string m_description;
};

inline void check_range( int begin, int value, int end )
{
    if( value<begin || value>=end )
        throw std::out_of_range( "YUVtool variable" );
}

#endif // ERRORS_H
