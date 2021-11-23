#pragma once


#include <exception>
#include "afterwards.hpp"

// This class runs a given lambda function when it's being destroyed because the
// stack is being unrolled due to an exception.
template<class Function>
class OnException {
public:
  OnException(Function function)
    : function(function) {}
  
  ~OnException() {
    if (std::current_exception() != nullptr)
      function();
  }
  
  Function function;
};



// This turns into lines like "destructor afterwards_23 = [&]". We append the
// name with __LINE__ to prevent multiple destructors from having the same name.
#define in_case_of_exception OnException CONCATENATE(onExcept__, __LINE__) = [&]

