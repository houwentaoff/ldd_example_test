# 调整最大频率
**内核版本 3.18**

## use intel pstate
`/etc/default/grub`:`GRUB_CMDLINE_LINUX_DEFAULT="quiet idle=poll"`: idle为休眠相关
inter pstate不能降频

### bios控制动态与否
P-STATE Coordination  ---> 必须为软件控制,若为硬件控制，则频率是动态变化的.不固定
EIST eist-智能降频技术最好打开
package C state limit 和 report ，最大cpu c state 似乎没有影响
* p-state 和 内核英特尔intel_pstate驱动匹配 Documentation/cpu-freq/intel-pstate.txt 
* cpu c state 大致是越低频率越高，内核根据该值动态调节cpu频率，代码忘记在哪里了

## use acpi-cpufreq
`/etc/default/grub`:`GRUB_CMDLINE_LINUX_DEFAULT="quiet idle=poll intel_pstate=disable"`

### acpi-cpufreq 可以降频
`/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies` : 根据此文件进行降频
`/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors` : userspace powersave conservative ondemand performance
`/sys/devices/system/cpu/cpu0/cpufreq/scaling_driver` : acpi-cpufreq

##  调频流程
1. 有两种类型的cpu：一种只需要给定调频范围，cpu会在该范围内自行确定运行频率；另一种需要软件指定具体的运行频率。

2. 对第一种cpu，cpufreq policy中会指定频率范围policy->{min, max}，之后通过setpolicy接口，使其生效即可。

3. 对第二种cpu，cpufreq policy在指定频率范围的同时，会指明使用的governor。governor在启动后，会动态的（例如启动一个timer，监测系统运行情况，并根据负荷调整频率），或者静态的（直接设置为某一个合适的频率值），设定cpu运行频率。

## 常用的governor介绍
1. Performance

性能优先的governor，直接将cpu频率设置为policy->{min,max}中的最大值。

2. Powersave

功耗优先的governor，直接将cpu频率设置为policy->{min,max}中的最小值。

3. Userspace

由用户空间程序通过scaling_setspeed文件修改频率。

4. Ondemand

根据CPU的当前使用率，动态的调节CPU频率。

5. Conservative

类似Ondemand，不过频率调节的会平滑一下，不会忽然调整为最大值，又忽然调整为最小值

## PS
* 调频也可以用百分比进行限制


## 疑问
* 在使用`intel pstate` 时，为何会触发`acpi_pm_read`函数,该函数不是和`acpi-cpufreq`强相关么?
*


