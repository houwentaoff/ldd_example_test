# spi 总线，设备端的设备和驱动和设备树

1. match的代码  
设备端
```c
static const struct spi_device_id arizona_spi_ids[] = {
	{ "wm5102", WM5102 },
	{ "wm5110", WM5110 },
	{ },
};
MODULE_DEVICE_TABLE(spi, arizona_spi_ids);
static const struct of_device_id arizona_of_match[] = {
	{ .compatible = "adi,ad9548" },
	{ },
};
static struct spi_driver arizona_spi_driver = {
	.driver = {
		.name	= "arizona",
		.owner	= THIS_MODULE,
		.pm	= &arizona_pm_ops,
		.of_match_table	= of_match_ptr(arizona_of_match),
	},
	.probe		= arizona_spi_probe,
	.remove		= arizona_spi_remove,
	.id_table	= arizona_spi_ids,
};
```
总线端
```c
struct bus_type spi_bus_type = {
	.name		= "spi",
	.dev_attrs	= spi_dev_attrs,
	.match		= spi_match_device,
	.uevent		= spi_uevent,
	.pm		= &spi_pm,
};
...
static int spi_match_device(struct device *dev, struct device_driver *drv)
{
	const struct spi_device	*spi = to_spi_device(dev);
	const struct spi_driver	*sdrv = to_spi_driver(drv);

	/* Attempt an OF style match */
    /* 匹配设备树，比较的函数为 此函数，该函数仅仅对设备树中一个设备节点中单/多个compatible spi_driver->driver.of_match_table (of_match_ptr(arizona_of_match))中进行了比较 
     * 此处比较和spi_driver->id_table无关.
     */
	if (of_driver_match_device(dev, drv))
		return 1;

	/* Then try ACPI */
	if (acpi_driver_match_device(dev, drv))
		return 1;
    /*
     * 如果存在id_table则不继续比较spi->modalias和drv->name
     * 该代码比较的是spidev->modalias和 每个id_table[*]->name,比较失败则直接返回.
     */
	if (sdrv->id_table)
		return !!spi_match_id(sdrv->id_table, spi);

	return strcmp(spi->modalias, drv->name) == 0;
}
```
2. dts文件描述:  
文档参考`Documentation\devicetree\bindings\spi\spi-bus.txt`中的`required`属性  
```dts
&spi0 {
    status = "okay";
    num-cs = <2>;
    aabbcc:ad9542@0{
        compatible = "adi,ad9542";
        reg=<0>;
        spi-max-frequency = <1000000>;
    }
    aabbdd:ad9528@0{
        compatible = "adi,ad9528";
        reg=<1>;
        spi-max-frequency = <1000000>;
    }    
}
```
