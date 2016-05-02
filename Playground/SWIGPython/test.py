#!/usr/bin/python
import example

print example.fact(4)
print example.accumulate


v = example.IntVector()
v.push_back(1)
v.push_back(2)
v.push_back(3)

print example.accumulate(v)

