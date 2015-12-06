/*
 * ReturnStatusCode.h
 *
 *  Created on: Nov 10, 2015
 *      Author: yash
 */

#ifndef RETURNSTATUSCODE_H_
#define RETURNSTATUSCODE_H_
#include <string>
#include <exception>
namespace daf
{
  //A status code so functions can send back feedback to the callee for tons of info
  class ReturnStatusCode;
  //Exception class wrapper around ReturnStatusCode
  class Exception;

  class ReturnStatusCode
  {
  protected:
    const char* _message;
    const int _code;
    const char* _source_file;
    const unsigned long long _line_number;
  public:
    inline ReturnStatusCode() :
      _message(""),
      _code(0),
      _source_file(__FILE__),
      _line_number(__LINE__)
    {
    }

    inline ReturnStatusCode(
      const char* message,
      const int code,
      const char* source,
      const unsigned long long line_num
    ) :
      _message(message),
      _code(code),
      _source_file(source),
      _line_number(line_num)
    {
    }

    inline virtual ~ReturnStatusCode()
    {
    }

    inline const char* message()
    {
      return _message;
    }

    inline int code()
    {
      return _code;
    }

    inline const char* source()
    {
      return _source_file;
    }

    inline unsigned long long line_num()
    {
      return _line_number;
    }

    std::string toString()
    {
      return "";
    }
  };

  class Exception : public std::exception
  {
  private:
    const char* _message;
    const ReturnStatusCode mscde;
  public:
    inline Exception() :
      _message("A daf::Exception was thrown"),
      mscde(
        "A daf::Exception was thrown",
        0,
        __FILE__,
        __LINE__
      )
    {
    }

    inline Exception(ReturnStatusCode status) :
      _message(status.message()),
      mscde(status)
    {
    }

    inline const ReturnStatusCode getStatusCode()
    {
      return mscde;
    }

    inline const char* getMessage()
    {
      return _message;
    }

    inline virtual const char* what() const throw()
    {
      return _message;
    }

    inline virtual ~Exception()
    {
    }
  };
}

//Few definitions to make coding easier.
//Definitions so that they can be undef'd. The end user may not want these cluttering the global namespace.
#define StatusCode(msg, code) (ReturnStatusCode(msg, code, __FILE__, __LINE__))
#define DafException(msg, code) ((Exception(ReturnStatusCode(msg, code, __FILE__, __LINE__))))

#endif /* RETURNSTATUSCODE_H_ */
