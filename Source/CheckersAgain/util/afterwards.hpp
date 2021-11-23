#pragma once


// This class runs a given lambda function when it's being destroyed.
template<class Function>
class OnDestruction {
public:
  OnDestruction(Function function) : function(function) {}
  ~OnDestruction() { function(); }
  Function function;
};

// The C preprocessor is mystical and mysterious so just believe me when I say
// this stuff is necessary.
#define CONCATENATE_IMPL(x, y) x ## y
#define CONCATENATE(x, y) CONCATENATE_IMPL(x, y)

// This turns into code like "OnDestruction afterwards_23 = [&]". We append the
// name with __LINE__ to prevent multiple destructors from having the same name.
#define afterwards    OnDestruction CONCATENATE(afterwards_, __LINE__) = [&]
