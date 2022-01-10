# Lua Profiler
[<img src="https://img.shields.io/github/license/Patrick08T/LuaProfiler">](https://github.com/Patrick08T/LuaProfiler)

This tool depends on cmake3.0+, lua5.3+ and [FlameGraph](https://github.com/brendangregg/FlameGraph.git)
Please check your environment. If you have already installed the cmake3.0+, please cancel the installation of cmake3.10 in build.sh.

## Theory
Like Perf, this tool can collect the function call stack in lua VM by frequency, and she can output a flamegraph.

## Usage
install lua5.3+
```
curl -R -O http://www.lua.org/ftp/lua-5.3.x.tar.gz
tar zxf lua-5.3.x.tar.gz
cd lua-5.3.x
make linux test
make install
cd ..
```

install FlameGraph
```
git clone https://github.com/brendangregg/FlameGraph.git
```

build project
```
./build.sh
```

and then, you will get a libluaperf.so in ./bin/debug/ directory

## Test
```
cp ./bin/debug/libluaperf.so ./
lua ./sample/sample.lua
./FlameGraph/flamegraph.pl samling.log > perf.svg
```
![image](https://user-images.githubusercontent.com/18464261/148522352-6f5734f3-ee38-4174-978c-99f353c81d6a.png)

