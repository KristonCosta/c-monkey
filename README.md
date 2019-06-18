[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5e75898ebe77454793a4419ac53ff82f)](https://app.codacy.com/app/KristonCosta/c-monkey?utm_source=github.com&utm_medium=referral&utm_content=KristonCosta/c-monkey&utm_campaign=Badge_Grade_Dashboard)
[![Build Status](https://travis-ci.org/KristonCosta/c-monkey.svg?branch=master)](https://travis-ci.org/KristonCosta/c-monkey)
[![codecov](https://codecov.io/gh/KristonCosta/c-monkey/branch/master/graph/badge.svg)](https://codecov.io/gh/KristonCosta/c-monkey)

Based on [Writing an Interpreter in Go](https://interpreterbook.com)

### Install Conan
pip install conan

### Build instructions 

```bash
mkdir build 
cd build 
conan install ..
conan build ..
./bin/main
```

### Test instructions 
Run either command in build directory:
```bash
ctest 
```
or 
```bash
./bin/CMonkeyTests
```

### Importing scripts
Set the `CMONKEY_INSTALL_DIR` path to point to the `lib` folder in this project root.
Save script to the `lib` folder of this dir.
You can import any script by using `@SCRIPT_NAME` in the interpreter. Refer to examples below.

### Examples
Set the `CMONKEY_INSTALL_DIR` path to point to the `lib` folder in this project root.

Build the project

Guesser game:
```
./bin/main 
>> @guesser
Loading /Users/kristoncosta/workspace/projects/c-monkey/lib/guesser
>> guess(100)
Guessed 50 but it was too low.
Guessed 75 but it was too low.
Guessed 88 but it was too low.
Guessed 94 but it was too low.
Guessed 97 but it was too low.
Guessed 99 but it was too low.
Got the correct answer! 100
null
```

Basic lib game:
```
./bin/main 
>> @prelude
Loading /Users/kristoncosta/workspace/projects/c-monkey/lib/prelude
>> map([1, 2, 3], fn(x) {x * 2})
[2, 4, 6]
>> reduce([1,2,3], 0, fn(x, y) { x + y})        
6
>> 
```


