#!/bin/bash
otool -L /Applications/Mesmerize.app/Contents/MacOS/QtMesmerize  | tail -n +2 | perl -pe 's,\s*(\S+).*,\1,g' | grep -v '^@'
