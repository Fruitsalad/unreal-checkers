#pragma once


#include "typedefs.hpp"

#if FRUIT_DEBUG
#   include <csignal>
#   include <iostream>
#endif
#if FRUIT_USE_BOOST
#   include <boost/stacktrace.hpp>
#endif



/** FRUIT_FUNC prints the current function. */
#if __GNUC__
#   define FRUIT_FUNC __PRETTY_FUNCTION__
#else
#   define FRUIT_FUNC __func__
#endif


/** Running FRUIT_BREAKPOINT should do the same thing as a normal breakpoint.
 * This only works in debug builds and does nothing in a release build. */
#if FRUIT_DEBUG
# ifdef __linux__
    // Linux
#   define FRUIT_BREAKPOINT raise(SIGTRAP)
# elif _WIN32
    // Windows
#   define FRUIT_BREAKPOINT DebugBreak()
# else
#   warning \
This operating system is not supported by breakpoint() yet. \
Modify util/debug.hpp to get rid of this warning.
#   define FRUIT_BREAKPOINT {}
# endif
#else
# define FRUIT_BREAKPOINT {}
#endif

/** The same as FRUIT_BREAKPOINT, this triggers a breakpoint in debug mode. */
#define breakpoint FRUIT_BREAKPOINT;


/** WHERE_AM_I prints the position in the file,
 * e.g. "debug.hpp:30, in breakpoint" */
#define FRUIT_WHERE_AM_I (String(__FILE__)+":"+$(__LINE__)+",\n"\
"in the function “"+String(FRUIT_FUNC))+"”"


/** STACKTRACE is meant to print a stacktrace, but it doesn't work unless you
 * link Boost and set FRUIT_USE_BOOST. I don't even do that myself. */
#if FRUIT_USE_BOOST
#   define FRUIT_STACKTRACE\
        (boost::stacktrace::stacktrace())
#else
#   define FRUIT_STACKTRACE ("")
#endif


/** FRUIT_ASSERT does a "gentle" assertion that doesn't crash the program.
 * FRUIT_ASSERT_MSG lets you add a message. */
#if FRUIT_DEBUG
#   define FRUIT_ASSERT(expression...) do {             \
        if (!(expression)) {                            \
            std::cerr << "SANITY TEST FAILED at "       \
                      << FRUIT_WHERE_AM_I << std::endl  \
                      << "The following condition was not met: "#expression \
                      << std::endl \
                      << FRUIT_STACKTRACE << std::endl; \
            FRUIT_BREAKPOINT;                           \
        }                                               \
    } while (false)

#   define FRUIT_ASSERT_MSG(expression, message...) do {\
        if (!(expression)) {                            \
            std::cerr << "SANITY TEST FAILED: "         \
                      << (message) << std::endl         \
                      << "At " << FRUIT_WHERE_AM_I << std::endl \
                      << "The following condition was not met: "#expression \
                      << std::endl \
                      << FRUIT_STACKTRACE << std::endl; \
            FRUIT_BREAKPOINT;                           \
        }                                               \
    } while (false)
#else
// Stubs for release mode.
#   define FRUIT_ASSERT(expression...) do {} while (false)
#   define FRUIT_ASSERT_MSG(expression, message...) do {} while (false)
#endif



/** Check whether an expression is true, for the purpose of sanity testing. */
#define assert_(expression...) FRUIT_ASSERT(expression)


/** It's `assert_` but with a custom message.
 * Note: Be careful to not put any commas in the expression. The preprocessor
 * isn't very smart. */
#define assert_msg(expr, msg...) FRUIT_ASSERT_MSG(expr, msg)


/** Trigger a breakpoint and crash the program, even in release mode. */
#define crash() do {\
    FRUIT_BREAKPOINT; \
    abort(); \
} while(false)
/* crash() is a macro instead of a normal function because putting `abort()`
 * into a function with a macro makes clang-tidy stop emitting the warning
 * "control reaches end of non-void function", and calling a function that
 * aborts doesn't. */