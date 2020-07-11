#!/bin/bash
set -x
set -e
docker run --rm -v `pwd`/shared_folder:/mnt -it --name clean_stretch clean/stretch /bin/bash
