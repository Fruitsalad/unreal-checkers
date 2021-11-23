#pragma once

#include <exception>
#include "typedefs.hpp"

#ifdef __GNUC__
#define THROW(msg...) throw Exception(__FILE__, __LINE__, __PRETTY_FUNCTION__, msg)
#define THROW2(type, args...) throw type(__FILE__, __LINE__, __PRETTY_FUNCTION__, args)
#else
#define THROW(msg...) throw Exception(__FILE__, __LINE__, __func__, msg)
#define THROW2(type, args...) throw type(__FILE__, __LINE__, __func__, args)
#endif

class Exception : public std::exception
{
public:
  Exception() : message("Something went wrong but I don't know what.") {}
  
  explicit Exception(String message)
      : message(std::move(message)) {}
  
  Exception(String file, int line, String function, String message)
      : message(std::move(message)),
        function(std::move(function)),
        file(std::move(file)),
        line(line) {}
  
  [[nodiscard]] const char* what() const noexcept override {
    return message.c_str();
  }
  
  // Addenda are useful for adding notes like "~ While processing vertex.glsl"
  void add_note(String$& note) {
    message += "\n" + note;
  }
  
  String to_string() const {
    return "### Exception thrown in function \""+function+"\" "
           "(line "+std::to_string(line)+" of "+file+") ###\n"
      + message + "\n";
  }
  
  friend String to_string(const Exception& e) {
    return e.to_string();
  }
  
  String message;
  String function;
  String file;
  int line = 0;
};
