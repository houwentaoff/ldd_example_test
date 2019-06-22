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

## 打过rt补丁的系统中的spinlock的不同之处
* rt是允许抢占的系统，在spinlock的实现中可以看到如果获取不到锁此处便是一个抢占点，优先级高的中断便可以强势插入嵌套。
* 由于打过rt补丁的系统，spin会被抢占，所以使用原始锁`raw_spin_lock`代替`spin_lock`，`raw_spin_lock_irqsave`代替`spin_lock_irqsave`
* 如果要保证非线程化中断`handle`中不被其它中断抢占，所以使用`raw_spin_lock`
* `wake_up_interruptible_all`在非线程化的中断中也不能使用，此时必须使用`wake_up_all_locked`，原因是`wake_up_interruptible_all`中使用了`spin_lock_irqsave`和`spin_unlock_irqrestore`该API会导致允许抢占，并且会导致内核警告
```c
irqreturn_t __handle_irq_event_percpu(struct irq_desc *desc, unsigned int *flags)
{
	irqreturn_t retval = IRQ_NONE;
	unsigned int irq = desc->irq_data.irq;
	struct irqaction *action;

	for_each_action_of_desc(desc, action) {
		irqreturn_t res;

		trace_irq_handler_entry(irq, action);
        /*
         * 若为线程化中断此处均为irq_default_primary_handler
         * 若为非线程化中断，此处为中断申请函数中的handle
         */
		res = action->handler(irq, action->dev_id);
        
		trace_irq_handler_exit(irq, action, res);
        /*
         * 注意 下面3行代码，如果我们在非线程化的中断中使用spin_unlock_irq会导致如下的wran_once必然打印
         *  使用wake_up_interruptible_all,唤醒信号量也会导致此警告发生
         */
		if (WARN_ONCE(!irqs_disabled(),"irq %u handler %pF enabled interrupts\n",
			      irq, action->handler))
			local_irq_disable();
        ....
}
```
