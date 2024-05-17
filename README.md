Test GEMM accuracy between CLBlast and OpenBLAS.

## Build

Get and build OpenBLAS:

```
git clone https://github.com/OpenMathLib/OpenBLAS.git
cd OpenBLAS
make -j4
make PREFIX=$(realpath .)/install install
```

Get and build CLBlast:

```
git clone https://github.com/CNugteren/CLBlast
cd CLBlast
cmake -B build -DCMAKE_INSTALL_PREFIX=build/install .
cmake --build build --target install -j4
```

Build this repo:

```
git clone https://github.com/fengyuentau/test-clblast.git
cd test-clblast
export CMAKE_PREFIX_PATH=/path/to/OpenBLAS/install:/path/to/CLBlast/build/install
cmake -B build .
cmake --build build
```

## Run

```
cd test-clblast
./build/main
```

## Results

```
// Apple M1
scale: 1250, max_diff: 348.821442
sacle: 1125, OK
scale: 1187, max_diff: 330.725189
scale: 1156, max_diff: 320.881348
sacle: 1140, OK
sacle: 1148, OK
scale: 1152, max_diff: 323.137970
sacle: 1150, OK
sacle: 1151, OK
// Results are back to OK after commenting out Apple M1 tuning results in CLBlast
```
