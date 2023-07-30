# 该文为bare-metal编译时创建mmu页表的脚本.

## 使用  
+ 配置文件为`mmu.json` 脚本内容目前为4k页表方便加速仿真.

## 为何需要mmu  
+ 在`arm`环境中如`memset`函数会使用到`cache`相关指令如果该范围不
  为cache会导致异常中断产生,`aarch64-none-elf-gcc 编译器`.在不同
  编译器下有不同表现,如`ds5/ads`的`armclang`的memset实现有所不同.  
  
## 优点  
+ 仿真平台如使用`cpu`动态去初始化`mmu`页表会耗时`n`分钟，该行为大大加速仿真.  
+ 在编译时确认mmu可用于自动化生成各种mmu配置组合，便于大范围`mmu`全覆盖测试. 
 