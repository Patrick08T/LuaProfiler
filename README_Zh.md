# Lua Profiler
[<img src="https://img.shields.io/github/license/Patrick08T/LuaProfiler">](https://github.com/Patrick08T/LuaProfiler)

注意: 本库依赖于 cmake3.0+, lua5.3+ 和 [FlameGraph](https://github.com/brendangregg/FlameGraph.git)


## 原理
类似于Perf, 这个工具能收集Lua虚拟机中的函数调用栈, 并将结果输出成火焰图.

## 使用教程
安装 lua5.3+
```
curl -R -O http://www.lua.org/ftp/lua-5.3.x.tar.gz
tar zxf lua-5.3.x.tar.gz
cd lua-5.3.x
make linux test
make install
cd ..
```

安装 FlameGraph
```
git clone https://github.com/brendangregg/FlameGraph.git
```

编译项目
```
./build.sh
```

然后, 你就可以在 ./bin/debug/ 目录下看到 libluaperf.so 文件了

## 测试
```
cp ./bin/debug/libluaperf.so ./
lua ./sample/sample.lua
./FlameGraph/flamegraph.pl samling.log > perf.svg
```
![image](https://user-images.githubusercontent.com/18464261/148522352-6f5734f3-ee38-4174-978c-99f353c81d6a.png)

