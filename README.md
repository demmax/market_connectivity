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
There are unit tests for `JsonIncrementalMessageConverter` and `ConsumerManager` classes.
Infrastructure code is expected to be tested by integration tests.  

Besides unit tests, there is also some sort of integration test in `integration_tests` directory.
It is basically a simple bash script, which runs server and a number of clients. After that it `cat`s 
servers stats file.
It can be run from `integration_tests` dir with 
```
./test.sh ../build/bin/mkt_conn_server ../build/bin/mkt_conn_client
```

## Protocol
Protocol is based on a simple json. `JsonIncrementalMessageConverter` is responsible for creating/parsing it.


## Design note
`JsonIncrementalMessageConverter`, `ConsumerManager` and `MultiQueueProcessor` could be passed to 
`TcpServer` either by interface pointer or by template. Since this is a sort of test task, I'm not 
aware of load profile and not sure which way is better. 
For simplicity, I just didn't do any of that at this point, but kept it low-coupled, 
and any of these approaches can be implemented easily. 
