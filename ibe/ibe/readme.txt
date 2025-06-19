依赖：
1. pbc: https://github.com/blynn/pbc
2. gmp: https://gmplib.org/
要先编译、安装gmp，再编译、安装pbc

测试代码在test.c里，速度测试代码在speed.c里
测试代码test.c同时也是接口调用示例

编译：
先编译出动态库：
make
再编译测试和速度测试的可执行文件：
make test
make speed
也可以一次编译出动态库和可执行文件：
make all

使用：
动态库是libibe.so
可执行文件是test和speed
运行测试：
./test
运行速度测试：
./speed

运行速度测试前请先修改speed.c中的cpu_speed，cpu_speed是你的cpu的频率

