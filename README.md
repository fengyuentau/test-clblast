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

Sgemm:

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
// Accuracy is fine after commenting out Apple M1 tuning results in CLBlast
```

(Fixed via https://github.com/CNugteren/CLBlast/pull/543) Cgemm:

```
// Intel(R) UHD Graphics 770, CLBlast commit: e0c06a9ac1da141c2cb684b4a10ce457b3328360
scale: 550, real_max_diff: 318.123413, imag_max_diff: 320.759399
scale: 325, real_max_diff: 196.814056, imag_max_diff: 191.424683
sacle: 212, OK
scale: 268, real_max_diff: 162.766602, imag_max_diff: 165.656494
sacle: 240, OK
sacle: 254, OK
scale: 261, real_max_diff: 162.882080, imag_max_diff: 161.465424
scale: 257, real_max_diff: 155.064240, imag_max_diff: 157.468262
sacle: 255, OK
scale: 256, real_max_diff: 159.027313, imag_max_diff: 166.023514
// Accuracy is fine after commenting out Intel(R) UHD Graphics 770 tuning results in CLBlast
```
