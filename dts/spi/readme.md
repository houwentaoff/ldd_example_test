# spi 总线，设备端的设备和驱动和设备树

## 1. match的代码  
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
## 2. dts文件描述:  
文档参考`Documentation\devicetree\bindings\spi\spi-bus.txt`中的`required`属性  
```dts
&spi0 {
    status = "okay";
    num-cs = <2>;
    aabbcc:ad9542@0{
        compatible = "adi,ad9542";
        reg=<0>; /* 表示spi第几个设备 */
        spi-max-frequency = <1000000>;
    }
    aabbdd:ad9528@0{
        compatible = "adi,ad9528";
        reg=<1>;
        spi-max-frequency = <1000000>;
    }    
}
### spi-gpio
. 打开linux-spi-gpio后，并不用调用spi_register_board_info去注册platform_device用如下dts描述就行
  在spi-gpio.c中看到`master->bits_per_word_mask = SPI_BPW_RANGE_MASK(1, 32);` 这一句表明gpio支持
  任意位的数据发送，若有些设备msg是非8/16/32位组成如9位，就依赖于spi-controller这个属性.gpio-spi
  是gpio模拟的所以支持所有位;
  有如下的dts配置，还差一步`spi-gpio.c : #define spidelay(nsecs)	do {} while (0)`你需要重新实现spidelay
  否则波形的周期是不正确的.

```dts
spi_gpio:spi-gpio {
	compatible = "spi-gpio";
	#address-cells = <1>;
	#size-cells = <0>;
	gpio-miso = <&gpio 1 0>;/* 该配置会被`if (test_bit(FLAG_ACTIVE_LOW, &desc->flags))value = !value;`解析，0表明写1情况下gpio的active_low的值，若这里写位1会导致发送数据位是反的 */
	gpio-mosi = <&gpio 2 0>;
	gpio-sck = <&gpio 3 0>;
	cs-gpios = <&gpio 4 0>;

	num-chipselects = <1>;
	status = "okay";

	eeprom@0{
		compatible = "eeprom-93xx46";
		reg= <0>;
		spi-max-frequency = <10000>;
		spi-cs-high;
		data-size = <16>;
	};
};
```
#### spi-gpio match的时间点
spi_gpio_probe 在gpio初始化后靠dts和driver-deferred-probe进行触发，注册spi-controller时再扫描其下的node进行device驱动的probe
### eeprom-93xx46 match的时间点:注册spi-master的时候就会进行对该master下的node进行match并执行对应的probe
注意driver-deferred-probe机制spi-gpio driver也是靠driver-deferred-probe延后进行probe
`spi_gpio_probe`->`devm_spi_register_master`->of_register_spi_devices:ctlr
->dev.of_node->of_register_spi_device->spi_add_device->device_add->
bus_probe_device(dev)->driver_probe_device:这里会唤醒 events deferred_probe_work_func去 ->eeprom-93xx46:probe


### spi slave
/* 如下为spi slave的dts配置,支持slave的驱动并不多，需要自己修改,spi-slave
 * 自己在spi-probe代码中用 of_property_read_bool(np, "spi-slave");进行解析
 * 并使用spi_alloc_slave..进行注册.Slave模式下调用ioctl会阻塞（master提供
 * 时钟,slave无时钟不会推数据出去），并且会有spireset
 * 操作所以 需自行添加接口
 */
&spi1 {   /* spi1 */
    status = "okay";
    spi-slave;
    rx_ready-gpios = <&gpio 11 0>;/*GPIO_ACTIVE_LOW>;*/
    tx_ready-gpios = <&gpio 22 0>;/*GPIO_ACTIVE_LOW>;*/
    spidev1:slave@0 {
        compatible = "test-spi";
        reg = <0>;
        spi-max-frequency = <1000000>;
    };
};
```
## 3.用户态的spi
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
    
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);/* 部分spi支持发送指定位数的msg如9位(默认为8位), 该位由驱动中的`master->bits_per_word_mask = SPI_BPW_RANGE_MASK(1, 32);`决定  */
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
        compatible = "spidev"; /* 匹配spidev 生成 /dev/spi1.0 这种文件 */
        reg = <0>;
        spi-max-frequency = <1000000>;
    };
    ad1234:ad1234@0{
        compatible = "adi,adrv9009"; /* 匹配自己写的 adi,adrv9009驱动，此时reg 有用否? */
        spi-max-frequency = <1000000>;
        reg = <0>;
    }
```
