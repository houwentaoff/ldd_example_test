## 调试

### 使用usbmon (linux)
* 依赖CONFIG_DEBUG_FS和CONFIG_USB_MON内核选项,打开Device Drivers->USB Support->USB Verbose Debug Messages则会将usb子系统所有的dev_dbg输出结果导出来

```sh
mount -t debugfs none_debugs /sys/kernel/debug/
ls -l /sys/kernel/debug/usbmon/
```
### 使用BUS Hound (win)

### 内核文档
* `KDIR/Documentation/usb/usbmon.txt`
