#!/bin/sh
find . \( -name "*.cpp" -or -name "*.hpp" \) -exec astyle --style=kr -t {} \;
find . -name "*.orig" -exec rm {} \;
