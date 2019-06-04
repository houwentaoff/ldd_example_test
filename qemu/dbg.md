## 调试手段

### top
`top -H -p pid` 查看各个线程 进入后F可以查看更多选项如绑定的cpu

### gdb
`sudo kill -6 pid`
`sudo gdb ./test ./core.12345`
`gdb --args ./a.out V1 V2 V3`
#### 生成core文件相关代码
```c
#include <sys/resource.h>

#define EnableCoreDumps()\
{\
struct rlimit limit;\
limit.rlim_cur = RLIM_INFINITY;\
limit.rlim_max = RLIM_INFINITY;\
setrlimit(RLIMIT_CORE, &limit);\
}
…
int main()
{
EnableCoreDumps();
mkdir("./cores",0775);//mkdir -p ../cores
system("busybox sysctl -w kernel.core_pattern=./cores/core.%e-%p-%t");//在./cores目录中生成 core.test….

return 0;
}
```

### gcore
* 使用gcore转存进程映像和上下文
`sudo gcore pid`
`sudo cgdb ./test_main -c ./core.18528`

### perf
`sudo perf top -p pid`
#### 来源
* 源码在`kernel/tool/perf`

### strace
`sudo strace -p pid`
