#!/bin/bash
sum() {
  [ "$#" == "0" ] && exit 1
  [ "$#" == "1" ] && echo $1 && exit
  head="$1"
  shift
  echo $(($head + $(sum $@)))
}

sum $@
