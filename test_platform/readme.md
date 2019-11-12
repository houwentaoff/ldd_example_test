# bind and unbind

在platform中可以手动调用驱动的remove函数`echo aaa > /sys/bus/platform/drivers/aaa/unbind`
也可以手动触发驱动的probe函数`echo aaa /sys/bus/platform/drivers/caam_jr/bind`

## bind

见源码 drivers/base/bus.c:bind_store
```c
static ssize_t bind_store(struct device_driver *drv, const char *buf,
			  size_t count);
```
该函数会找到echo进来字符串对应的device，并和当前driver进行bind，然后调用driver->probe(platform中的remove)
```c
	if (dev->bus->probe) {
		ret = dev->bus->probe(dev);
		if (ret)
			goto probe_failed;
	} else if (drv->probe) {
		ret = drv->probe(dev);//dev为解析dts中的device时建立的device
		if (ret)
			goto probe_failed;
	}
```

## unbind

见源码 drivers/base/bus.c:unbind_store
```c
static ssize_t unbind_store(struct device_driver *drv, const char *buf,
			    size_t count);
```
该函数会找到echo进来字符串对应的device，并获取其绑定的driver，然后调用driver->remove(platform中的remove)
```c
    drv = dev->driver;
    if (dev->bus && dev->bus->remove)
        dev->bus->remove(dev);
    else if (drv->remove)
        drv->remove(dev);
```

