
[![Build Status](https://travis-ci.org/KristonCosta/c-monkey.svg?branch=master)](https://travis-ci.org/KristonCosta/c-monkey)
[![codecov](https://codecov.io/gh/KristonCosta/c-monkey/branch/master/graph/badge.svg)](https://codecov.io/gh/KristonCosta/c-monkey)

Based on [Writing an Interpreter in Go](https://interpreterbook.com)

### Install Conan
pip install conan

### Build instructions 

```
mkdir build 
cd build 
conan install ..
conan build ..
./bin/main
```

### Test instructions 
Run either command in build directory:
```
ctest 
```
or 
```
./tests/bin/tests
```
