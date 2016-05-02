/* File: example.i */
%module example

%include <std_vector.i>

%{
#include "example.h"
%}

%include "example.h"

