## Build

Project uses [`conan`](https://conan.io) to install dependencies, so it needs to be installed.
If you have `python3`, it simply can be installed by 
```
pip3 install conan
```

Build command for Linux:
```bash
git clone https://github.com/demmax/market_connectivity
cd market_connectivity/ && mkdir build && cd build
conan install .. --remote conancenter --build missing --settings build_type=Release --settings compiler.libcxx=libstdc++11
cmake .. && make -j8
```

It will generate 3 targets: `mkt_conn_server`, `mkt_conn_client` and `mkt_conn_unit_tests`.
Unit tests can be run by 
```bash
bin/mkt_conn_unit_tests
```

## Tests
Besides unit tests, there is also some sort of integration test in `integration_tests` directory.
It is basically a simple bash script, which runs server and a number of clients. After that it `cat`s 
servers stats file.
It can be run from `integration_tests` dir with 
```
./test.sh ../build/bin/mkt_conn_server ../build/bin/mkt_conn_client
```
