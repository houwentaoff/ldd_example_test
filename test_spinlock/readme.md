# 理解中断

## 理解如下3个API

**local_irq_enable**
**preempt_enable**
**local_bh_enable**

## 需要理解如下问题
* spinlock的实现使用了上面哪个api
* 为何中断中要间接调用`local_irq_disable`关闭中断
* 为何spinlock中要先关中断再关抢占
* 为何spinlock中不允许睡眠
