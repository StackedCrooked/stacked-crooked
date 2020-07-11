#!/bin/bash
set -x
set -e
docker build -t clean/stretch:latest -f Dockerfile .
