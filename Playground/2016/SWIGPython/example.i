/* File: example.i */
%module example

%include <std_vector.i>

%{
#include "example.h"
%}

%include "example.h"

%include "std_vector.i"

%template(IntVector)    std::vector<int>;
