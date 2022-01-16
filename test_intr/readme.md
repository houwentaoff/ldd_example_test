# 中断

## 软中断执行点，调度检查点，中断DESC

  irq_exit中会检查软中断标记位并调用软中断
  调用schedule会导致任务调度，选取一个新的任务进行执行，在如下所有抢占点若条件满足今年会导致任务调度
## preempt抢占点
  preempt_enable 是一个内核抢占点，若preempt_count_dec_and_test满足则提供抢占;自旋锁实现会调用preempt_enable所以有自旋锁的地方都是抢占点
  在执行中断完后如`handle_fasteoi_irq``handle_edge_irq`末尾会有`raw_spin_lock`，该api会调用preempt_enable也是一个抢占点
  中断执行完后，会执行软中断，在软中断的末尾会调用wakeup_softirqd->preempt_enable也是一个抢占点