#include "Errors.h"


GeneralError::GeneralError(const std::string &description) :
        m_description(description)
{ }

const char *GeneralError::what() const throw()
{
    return ("general error: " + m_description).c_str();
}




//TIoError::TIoError( const std::string &file_name ) :
//  m_file_name( file_name )
//{ ; }
//
//const char *TIoError::what() {
//  return ""
//}
