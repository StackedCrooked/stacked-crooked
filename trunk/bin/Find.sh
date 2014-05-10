#!/bin/bash
find . "$@" | grep -v '^\.$' | sed -e 's,^\./,,'
