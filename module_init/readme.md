# 模块初始化宏module_init(aabbcc);

该宏内核一个模块中只能使用一次，存在`module_spi_driver(ad9523_driver);`的不能使用,原因为一个模块初始化函数只允许一个，而
`module_spi_driver` `module_i2c_driver`其中展开后如下，已经调用了module_init.
spi.h
```c
#define module_spi_driver(__spi_driver) \
	module_driver(__spi_driver, spi_register_driver, \
			spi_unregister_driver)
.....
#define module_driver(__driver, __register, __unregister, ...) \
static int __init __driver##_init(void) \
{ \
	return __register(&(__driver) , ##__VA_ARGS__); \
} \
module_init(__driver##_init); \
static void __exit __driver##_exit(void) \
{ \
	__unregister(&(__driver) , ##__VA_ARGS__); \
} \
module_exit(__driver##_exit);
```

```c
static int __init aaa_init(void)
{
    return 0;
}
static void __exit aaa_exit(void)
{
}
module_init(aaa_init);
module_exit(aaa_exit);
```
