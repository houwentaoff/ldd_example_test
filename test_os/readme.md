# 移植OS

## threadX  
+ `os`中有现成的`smp`代码，直接使用即可  
+ 找到对于`a55`下的`smp`目录，其他目录均可删除掉  
+ `start.S`中默认是`core0`为主`core`，若自己使用不是需要修改  
+ `mmu`可使用`os`的`demo`,而后修改为自己的  
+ `ld`文件需要修改`gic`和外设对应的地址  

### 需要添加
+ 定时器. 给`OS`以`tick`    
+ 其它`core`上电和重写`warmingrst`寄存器用来设在其它`core`上电后的地址  
+ `core`的个数，一般默认为`4` 还有对应的`mask`.  
+ `mmu enable`就能使用串口对应的物理地址等.`disable`的时候注意`ldr`是要访问对齐地址的否则会异常   
+ 注意`threadx` 线程的堆栈大小是否会超界.  
+ 部分宏需要打开，如`OS`运行在`el3/el1` 是不是`cortexA-v2`架构等:不知道那些打开，t32debug时单步挂异常能看到  

做了上述操作后系统默认的demo基本就可以跑起来了,比较简单`3`天工作量即可  

## freeRTOS(非SMP) riscV     
+ 软中断  该功能在任务切换时会使用
    启动位置 vPortYield   
  ```asm
    crrsi a0, mie, 8
    li t0, 2
    la t1, 0x10402000
    sw t0, 0(t1)
    li t0, 2
    la t1, 0x10401000
    sw t0, 0(t1)
  ...loop: 等待mip中断触发
  ```
    触发位置 软中断
  ```asm
      备份所有寄存器
      la a1, pxCurrerntTCB ;加载tcb
      ld a1, (a1)
      sd sp. (a1)
      jal vTaskSwitchContext ;调用vTaskSwitchContext
      ...
      call sw_irq_claim_interrupt ;清除软中断
      li a0, 0x1
      call sw_irq_complete_interrupt; 清除软中断
      ...
  ```  
+ tick（mtime中断）
```asm
   在中调用mtimer 中断指向的handler   
```
```C
void tick_irq_handler(void *arg)
{
    csi_tick_increase();
    csi_coret_config((soc_get_coretim_freq() / CONFIG_SYSTICK_HZ), CORET_IRQn);//配置下一次tick中断
#if CONFIG_AOS_OSAL
    extern void aos_sys_tick_handler(void);
    aos_sys_tick_handler();//tom this
#else
#ifdef CONFIG_KERNEL_FREERTOS
a    extern void xPortSysTickHandler(void);
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_RTTHREAD)
c    extern void rt_tick_increase(void);
    rt_tick_increase();
#else
#endif
#endif /* end CONFIG_AOS_OSAL */
}
void CORET_IRQHandler(void)//tom tick mtimer intr
{
#if defined(CONFIG_SMP) && CONFIG_SMP
    g_irq_nested_level[csi_get_cpu_id()]++;
#else
    g_irq_nested_level++;
#endif
    CSI_INTRPT_ENTER();
    tick_irq_handler(NULL);
    CSI_INTRPT_EXIT();
#if defined(CONFIG_SMP) && CONFIG_SMP
    g_irq_nested_level[csi_get_cpu_id()]--;
#else
    g_irq_nested_level--;
#endif
}
    
```
+ 其它中断
  如下可临时使用no_target_int处理
  ```asm
      csrr t0, mcause
      li   t1, 0x8000000000000003
      beq  t0, t1, msoftirq
      li   t1, 0x8000000000000007
      beq  t0, t1, mtimeirq
   no_target_int:
      j no_target_int
  ```
修改下ld文件和对应的__heap_start/__heap_end,bss,重写串口打印函数printf/_write函数即可让OS跑起来，这里不需要tick中断也能跑起来，msoft是必须实现的，否则无法进行任务切换。其它不需要用到的中断需屏蔽 
整体大约3天工作量。
