# Build

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make VERBOSE=1
```


Show symbols

```sh
objdump -t external-Linux/h2o-6dda7d6f21610ecd5256543384fa4b4b345a88ac/build/libh2o-evloop.a

ldconfig -p

```
