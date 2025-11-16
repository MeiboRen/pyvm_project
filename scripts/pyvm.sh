#!/bin/sh
main_file="$1"
shift
python3 -m compileall "$main_file" "$@"
mv "__pycache__/${main_file%.*}.cpython-310.pyc" "build/${main_file%.*}.pyc"
for dep in "$@"; do
    mv "__pycache__/${dep%.*}.cpython-310.pyc" "build/lib/${dep%.*}.pyc"
done
rm -rf __pycache__
cd build
./pyvm "${main_file%.*}.pyc"
