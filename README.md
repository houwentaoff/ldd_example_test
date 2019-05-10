# ldd_examle_test
**ldd的测试例子**

## Qemu
1. 便于调试kernel的qemu配置和环境.

## 测试例子

1. 测试input设备.
2. 测试netlink.
3. 测试platform.
4. 测试proc.
5. 测试sysfs.
6. 测试usb device gadget.
7. 测试completion. (一种同步机制)
8. 测试spinlock.
9. 测试misc.
10. 测试wait_queue.(和完成量相关)
11. 测试tasklet & tasklet hi.
12. 测试mmap.
13. 测试线程化中断.
14. 测试schedule/signal_pending/set_current_state.
15. 增加网卡转发例子.
16. 测试tap网络的例子.
17. 测试timer例子.
18. 测试C2 interface例子.
19. 测试FW API例子.
20. 增加有自定义其它判断条件的等待队列.
21. 增加Qualcomm Fuse Programmable Read Only Memory(QFPROM).
22. 增加电源管理模块demo regulator.
23. 增加多线程阻塞IO,list_head.

## 其它测试例子

1. 测试芯片内部看门狗（硬件看门狗，内核飞掉也能重启）.
