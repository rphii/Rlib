# Rlib

Commonly used functions by rphii.

## How to compile
See https://github.com/rphii/bd.

## Library
| Header                   | Source                   | Description                                |
|--------------------------|--------------------------|--------------------------------------------|
| [`rstr.h`](src/rstr.h)   | [`rstr.c`](src/rstr.c)   | basic, quite versatile string structure    |
| [`r2str.h`](src/r2str.h) | [`r2str.c`](src/r2str.c) | handy for fast formattet text file output  |
| [`rfile.h`](src/rfile.h) | [`rfile.c`](src/rfile.c) | basic file input / output                  |
| [`map.h`](src/map.h)     | [`map.c`](src/map.c)     | generic map / dictionary                   |

## Examples
- [`rstr_ex`](examples/rstr_ex.c) Basic usage of Rstr
- [`r2str_ex`](examples/r2str_ex.c) Basic usage of R2str
- [`writecomp`](examples/writecomp.c) Benchmark of writing to files
- [`map1_ex`](examples/map1_ex.c) Using Map with basic types `Map<int,int>`
- [`map2_ex`](examples/map2_ex.c) Using Map with custom structure `Map<Rstr,Rstr>`
- [`map3_ex`](examples/map3_ex.c) Using Map with mixed structure `Map<int,Rstr>`
- [`map4_ex`](examples/map4_ex.c) Using Map with mixed structure `Map<Rstr,int>`
- [`map5_ex`](examples/map5_ex.c) Using Map with basic types `Map<char[],int>`
