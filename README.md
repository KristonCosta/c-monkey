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
