
/*-----------------------------------------------------------------------------
 *  前提条件:
 *  1. WDT看门狗不能在电路中被关掉(如被开机强制拉低接下拉电阻).
 *  2. 在uboot中将WDT端口置高.
 *  3. 一般上电有电路会检查看门狗电路,如此时没有置高会导致软件在没有喂狗的情况下,
 *     内核卡住.并不会重启.注意是内核卡住不是app.
 *  4. 直接使用/dev/watchdog设备即可.内核有个选项是只要一旦打开就不会关闭看门狗的
 *     配置
 *  5. 看门狗的定时器溢出时间由芯片手册决定比如nuc970的最多8s,而fsl imx6最多为120s.
 *     这里描述的不是精确值
 *  6. 窗口看门狗的特性是精度更高比如40ms.
 *  7. 看门狗还可用来唤醒cpu(还未进行尝试)
 *-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>

int main(void)
{

        int ii = 10, j;
        int fd = open("/dev/watchdog", O_RDWR);
        if (fd == -1) {
                perror("watchdog");
                exit(EXIT_FAILURE);
        }

        printf("Open watchdog ok\n");

	// Ping WDT for 10 times, then let system reset
        while (1) {
        	printf("ii = %d\n", ii);
                if(ii-- > 0)
                        ioctl(fd, WDIOC_KEEPALIVE, 0);
                sleep(1);
        }


        /*-----------------------------------------------------------------------------
         *  也可以通过write喂狗
         *-----------------------------------------------------------------------------*/
#if 1
        ret = write(fd, "\0", 1);
        if (ret != 1) {
            ret = -1;
            break;
        }
        sleep(10);
#endif

    /*-----------------------------------------------------------------------------
    *  设置超时和获取
    *-----------------------------------------------------------------------------*/

    int timeout = 45;
    ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
    printf("The timeout was set to %d seconds\n", timeout);
    ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
    printf("The timeout was set to %d seconds\n", timeout);

    close(fd);
    return 0;
}

/*-----------------------------------------------------------------------------
 *  下面为drivers/watchdog/nuc970_wdt.c 可以看到利用ioctl进行设超时时间,设置>=8
 *  超时时间才为8,否则为2,timeout<2则为0.5s
 *-----------------------------------------------------------------------------*/
#if 1
/*
 ** Assumming 32k crytal is configured as the watchdog clock source,
 ** the time out interval can be calculated via following formula:
 ** WTISreal time interval (formula)
 ** 0x05((2^ 14 + 1024) * (32k crystal freq))seconds = 0.53 sec
 ** 0x06((2^ 16 + 1024) * (32k crystal freq))seconds = 2.03 sec
 ** 0x07((2^ 18 + 1024) * (32k crystal freq))seconds = 8.03 sec
 *
 */

static int nuc970wdt_set_timeout(struct watchdog_device *wdd, unsigned int timeout)
{
    unsigned int val;

    Unlock_RegWriteProtect();
    val = __raw_readl(REG_WDT_CR);
    val &= ~WTIS;
    if(timeout < 2) {
        val |= 0x5 << 8;
    } else if (timeout < 8) {
        val |= 0x6 << 8;
    } else {
        val |= 0x7 << 8;
    }

    __raw_writel(val, REG_WDT_CR);
    Lock_RegWriteProtect();
    return 0;
}
#endif
