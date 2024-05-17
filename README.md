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
// Apple M1, CLBlast commit: e0c06a9ac1da141c2cb684b4a10ce457b3328360
scale: 2100, max_diff: 572.165283
scale: 1550, max_diff: 428.167358
scale: 1275, max_diff: 355.969208
sacle: 1137, OK
scale: 1206, max_diff: 339.924744
scale: 1171, max_diff: 325.418304
scale: 1154, max_diff: 324.447571
sacle: 1145, OK
sacle: 1149, OK
sacle: 1151, OK
scale: 1152, max_diff: 322.837982
// Results are back to OK after commenting out Apple M1 tuning results in CLBlast
```
