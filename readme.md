## Python Virtual Machine
A lightweight Python 3.10 virtual machine implementation written in C++.

### Requirements
- Ubuntu 22.04
- CMake 4.2.0
- GCC 11.4.0

### Build
```bash
$ cd build
$ cmake ../
$ make all
```

### Usage
```bash
# run the python virtual machine
$ scripts/pyvm.sh <py_file>
# parse a pyc file
$ python3 scripts/parser_pyc.py build/<pyc_file>
```