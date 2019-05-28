
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5e75898ebe77454793a4419ac53ff82f)](https://app.codacy.com/app/KristonCosta/c-monkey?utm_source=github.com&utm_medium=referral&utm_content=KristonCosta/c-monkey&utm_campaign=Badge_Grade_Dashboard)
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
./bin/CMonkeyTests
```
