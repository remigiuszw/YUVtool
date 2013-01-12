#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <string>

class TGeneralError : public std::exception {
public:
  TGeneralError( const std::string &file_name );
  const char* what() const throw();
private:
  std::string m_description;
};

//class TIoError : public std::exception {
//public:
//  TIoError( const std::string &file_name );
//  const char* what() const;
//private:
//  std::string m_file_name;
//};

#endif // ERRORS_H
