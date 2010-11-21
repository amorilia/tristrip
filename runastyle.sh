#!/bin/sh
find . \( -name "*.cpp" -or -name "*.hpp" \) -exec astyle --style=linux -t {} \;
#find . -name "*.orig" -exec rm {} \;
