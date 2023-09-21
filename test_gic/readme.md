#ARMV8 GICV3

##EL0  
  `userspace`
##EL1  
  `nsec:linux-kernel:REE`
  `sec:TEE trustOS`
##EL2  
  `虚拟机?`
##EL3  
  `ATF`
##FIQ  
`fiq`称为中断转发器，用于在不同`elx`和`sec/nsec`下发生非当前环境的中断时的中断路由操作`el3:group0` `group1:sec/nsec`.  
`fiq`的值如下，大致根据不同值进行转发到不同elx  
  `1021`:
  `102.`
##IRQ  


##关键寄存器SCR_EL3

`SCR_EL3.NS` : `0:sec  1:nsec`
`SCR_EL3.IRQ`: 来了个非`el3`的中断时的处理 `1:route to el3   0:no route` 此处如何理解?
`SCR_EL3.FIQ`: 来了个非`el3`的中断时的处理 `1:route to el3   0:no route`

##elx切换
在`el0123`切换的时候`cpu`会根据`scr_el3`和`group0/ groupsec/nsec`进行重新发中断  

##SMC指令
`smc`在中断里的使用场景:
