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
## 用户态的spi
```c
int openspi()
{
    char        dev_node[16];
    int fd = 0;
    int ret = 0;
    int mode = 0, bits = 8, speed = 100 * 1000 * 1000;
    //mode |= SPI_LSB_FIRST;

    sprintf(dev_node, "/dev/spidev%d.0", 1);
    fd = open(dev_node, O_RDWR);
    if (fd < 0) {
        perror("open");
        return retVal=-1;
    }
    *(int *)devHalInfo = fd;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret < 0) {
        perror("SPI_IOC_WR_MODE");
        return retVal=-2;
    }
    printf("-->%s:line[0x%x]spi mode[0x%x]\n", __func__, __LINE__, mode);
    
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret < 0) {
        perror("SPI_IOC_WR_BITS_PER_WORD");
        return retVal=-3;
    }
    
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret < 0) {
        perror("SPI_IOC_WR_MAX_SPEED_HZ");
        return retVal=-4;
    }
    return ret;
}
/* 该函数和内核的spi_write_then_read 对应*/
static int spi_write_then_read(int fd,
		const unsigned char *txbuf, unsigned n_tx,
		unsigned char *rxbuf, unsigned n_rx)
{
	int ret = 0;

	struct spi_ioc_transfer tr[2] = {
		{
			.tx_buf = (unsigned long)txbuf,
			.len = n_tx,
		}, {
			.rx_buf = (unsigned long)rxbuf,
			.len = n_rx,
		},
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
	if (ret == 1) {
		printf("%s: Can't send spi message\n\r", __func__);
		return -1;
	}

	return ret;
}
```
```dts
&spi1 {
    status = "okay";
/*下文可以发现 都用到了spi1.0 这种情况会在dts解析生成设备文件的时候，先后进行，ad1234会提示无法注册进设备因reg一样*/
    spidev0: spi@0 {           
        compatible = "spidev"; 
        reg = <0>;
        spi-max-frequency = <1000000>;
    };
    ad1234:ad1234@0{
        compatible = "adi,adrv9009";
        spi-max-frequency = <1000000>;
        reg = <0>;
    }
```
