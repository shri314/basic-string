# a sample basic string implementation
This is a sample header file containing an implementation similar to std::basic\_string, but without
a lot of overloads one usually gets. This is a pure exercise and has no advantages over the standard
library. Nevertheless the code aims to demonstrate how to handle allocations in such a fashion that
the strong exception guarantees are held, and also to demonstrate how a good use of RAII and copy
and swap idiom can result in DRY code.

### building
```
git clone https://github.com/shri314/basic-string.git
mkdir -p build_dir
cmake -DCMAKE_BUILD_TYPE=Debug -S basic-string -B build_dir
make -C build_dir
```

### running the tests
```
./build_dir/test || echo FAILED
```
