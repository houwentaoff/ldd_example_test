# trace 功能 
## t32 
`trace` 分为 `onchip`和`TPIU` 2种 均需根据`coresight`架构图和`coresight`文档中的`etm` `tpiu`等的地址来按需配置脚本才能使用 
## 功能  
 当发生同步异常`sync`或者系统异常`SERR` 时使用下图中的图形能看到程序在挂掉之前的运行的指令方便问题的定位. 此时通过看elr只能看到进异常最后一次的函数并不知道整体的函数堆栈和调用关系.
 `sync`: data abort 发生时刻和挂掉时刻一致.
 `serr`: 来自外部abort 比如访问权限不够的模块地址.

## 配置脚本  
`trace.cmm` 

## onchip 
  buffer大小较小.比如每个core 1K.
  使用截图如下 ![trace-debug](https://github.com/houwentaoff/images/blob/master/trace32/trace-onchip.png)
  coresight架构截图如下 ![trace-debug](https://github.com/houwentaoff/images/blob/master/trace32/coresight.png)

## TPIU  
  上`G`的`buffer`大小(外部设备专有的TPIU插口)
  选项不是`onchip`而是旁边的`analyzer`其它一样,多一个TPIU的设备插在TPIU接口上（debug和TPIU会同时插上,t32是2个调试器会拼接在一起）.

## 其它
使用自动扫描不一定能扫描出这些etb等的地址信息，因为romtable里面这些信息不是必须的
```
设计coresight的时候，可以在funnel前面挂上ETM model来仿真外部的ETM;这样，rom就会记录这些eTM的地址和id了;生成RTL的时候，删除这些eTM model即可;否则，的确是无法自动扫描到core里的ETM
```
