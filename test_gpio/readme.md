# 如何使能gpio

## 如果你只是使用芯片厂商提供的芯片
### 如高通
1. pinctrl.dtsi中把 muxpin和active/sleep的状态全部写好了; 用户只需在自己的dts中写入 test-gpios = <&tlmm 116 0>；即可。
如gpio1 的function功能有4种 分别是i2c_scl,spi_clk,uart_tx,gpio1；在dts中体现如下
pinctrl.dtsi: 描述了所有i2c_scl spi_clk, uart_tx的pins和function 的关系
qupv3.dtsi:描述了所有i2c_scl spi_clk, uart_tx的设备节点但是这里都是 disable状态，如果你要使用i2c_scl需要在i2c的节点 上面写上status="okay",写上之后就不能使用对应的gpio功能了。
如要使用gpio功能就需要将qupv3.dtsi中对应的 i2c/spi/uart 写为 disabled状态。

