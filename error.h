#ifndef __VOID_ERROR_HANDLING__
#define __VOID_ERROR_HANDLING__

#include <stdbool.h>

// define an error type for otherwise void functions
// the error type is just a bool; return true if there's
// an error.  This gives the go style idiom of
// err = sideEffectingFunction()
// if (err) { //handle error }

// this is mostly useful for the setters for the CoAP
// messages in the event a message of the wrong size is written, etc.

typedef bool voidError;

#endif