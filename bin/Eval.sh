#!/bin/bash
while read line ; do eval "${@}${line}" ; done
