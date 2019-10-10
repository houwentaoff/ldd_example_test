# 线程化中断对整个驱动的影响

## X86 linux4.10 中 线程化中断靠wakeup唤醒，handle名存实亡，源码分析如下

1. 汇编代码`arch/x86/entry/entry_64.S` 中
``` asm
	/*
	 * The interrupt stubs push (~vector+0x80) onto the stack and
	 * then jump to common_interrupt.
	 */
	.p2align CONFIG_X86_L1_CACHE_SHIFT
common_interrupt:
	ASM_CLAC
	addq	$-0x80, (%rsp)			/* Adjust vector to [-256, -1] range */
	interrupt do_IRQ

```
2. 可以看到此处调用`do_IRQ`函数进入中断处理
`__visible unsigned int __irq_entry do_IRQ(struct pt_regs *regs)
`-->`handle_irq`-->`...`-->`handle_level_irq`-->`handle_irq_event`-->
`handle_irq_event_percpu`-->
``` c
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
         * 为何会为该函数,见下个代码片段
         */
		res = action->handler(irq, action->dev_id);
        
		trace_irq_handler_exit(irq, action, res);
                /*
                 * 注意 下面3行代码，如果我们在非线程化的中断中使用spin_unlock_irq会导致如下的wran_once必然打印而线程化中断则不会出现该现象？
                 */
		if (WARN_ONCE(!irqs_disabled(),"irq %u handler %pF enabled interrupts\n",
			      irq, action->handler))
			local_irq_disable();

		switch (res) {
		case IRQ_WAKE_THREAD:
			/*
			 * Catch drivers which return WAKE_THREAD but
			 * did not set up a thread function
			 */
			if (unlikely(!action->thread_fn)) {
				warn_no_thread(irq, action);
				break;
			}

			__irq_wake_thread(desc, action);//依靠wake up唤醒被线程化的中断

			/* Fall through to add to randomness */
		case IRQ_HANDLED:
			*flags |= action->flags;
			break;

		default:
			break;
		}

		retval |= res;
	}

	return retval;
}

```
线程化的中断 该函数被申请中断函数所调用
```c 
static int irq_setup_forced_threading(struct irqaction *new)
{
	if (!force_irqthreads)
		return 0;
	if (new->flags & (IRQF_NO_THREAD | IRQF_PERCPU | IRQF_ONESHOT))
		return 0;

	new->flags |= IRQF_ONESHOT;

	/*
	 * Handle the case where we have a real primary handler and a
	 * thread handler. We force thread them as well by creating a
	 * secondary action.
	 */
	if (new->handler != irq_default_primary_handler && new->thread_fn) {
		/* Allocate the secondary action */
		new->secondary = kzalloc(sizeof(struct irqaction), GFP_KERNEL);
		if (!new->secondary)
			return -ENOMEM;
		new->secondary->handler = irq_forced_secondary_handler;
		new->secondary->thread_fn = new->thread_fn;
		new->secondary->dev_id = new->dev_id;
		new->secondary->irq = new->irq;
		new->secondary->name = new->name;
	}
	/* Deal with the primary handler */
	set_bit(IRQTF_FORCED_THREAD, &new->thread_flags);
	new->thread_fn = new->handler;
    /*
     * handler 和 thread_fn 和申请中断函数的request_threaded_irq中的handle和thread_fn是匹配的，
     * 强制线程化后，handle会赋值给new->thread_fn，而new->thread_fn会赋值给new->secondary->thread_fn，
     * 即是handle实际已不复存在，被赋值一个返回IRQ_WAKE_THREAD的irq_default_primary_handler函数
     */
	new->handler = irq_default_primary_handler;
	return 0;
}
```
## 结论
1. 线程化后的中断反应不及时原因为触发后并不是立即执行，而需要RT调度中进行调度并执行
2. 申请中断若handle和threan_fn均赋值则实际表现为2个线程
