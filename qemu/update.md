**内核版本更新**

# 内核下载
* 内核rt补丁地址  
    https://mirrors.edge.kernel.org/pub/linux/kernel/projects/rt/  
* 内核源码
    https://mirrors.edge.kernel.org/pub/linux/kernel/  

# 打rt补丁
 `cd $KDIR`     
 `while read line do echo $line ;patch -p1 < ../patches/$line; done <../patches/series`  
 
# 内核升级
`make modules_install`升级驱动  
`make install` 升级内核
