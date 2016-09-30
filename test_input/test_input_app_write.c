/*
20150101
just a simple input test code
lei_wang
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>
/**
 * @brief 
 *
 * @param fd
 */
void event_sync(int fd)
{
    struct input_event event;
    static int i=0;

    if (fd <= fileno(stderr)) 
    {
        return;
    }
    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);

    event.type = EV_SYN;
    /* EV_SYN + SYN_CONFIG 会触发input_dev->event()函数 input_dev->grab->handler->event()还不知怎么触发
     * EV_SYN + SYN_REPORT 不会触发input_dev->event() 
     * */
    event.code = SYN_CONFIG;// 引起快速中断？...SYN_REPORT; 但是read会延迟
    event.value = i++;//0;//用这个 传value 不用EV_KEY中的code和value
    if (write(fd, &event, sizeof(event)) < 0) 
    {
        perror("send_event error\n");
    }
}
void send_event(int fd, unsigned short type, unsigned short code, int value) 
{
    struct input_event event;
    int len;

    printf("SendEvent call (%d,%d,%d,%d)\n", fd, type, code, value);

    if (fd <= fileno(stderr)) return;

    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);

    // event (type, code, value)
    event.type = type;
    event.code = code;
    event.value = value;
    if (write(fd, &event, sizeof(event)) < 0) {
        perror("send_event error\n");
    }
#if 0
    // sync (0,0,0)
    event.type = EV_SYN;
    event.code = SYN_CONFIG;//SYN_REPORT;
    event.value = 0;
    if (write(fd, &event, sizeof(event)) < 0) {
        perror("send_event error\n");
    }
#endif
}

int main()
{
	int fd;
	int version;
	int ret;
	struct input_event ev;
	
	fd = open("/dev/input/event0", O_RDWR);
	if (fd < 0) {
		printf("open file failed\n");
		exit(1);
	}

	ioctl(fd, EVIOCGVERSION, &version);
	printf("evdev driver version is 0x%x: %d.%d.%d\n",
					version, version>>16, (version>>8) & 0xff, version & 0xff);
	while (1) {
//        send_event(fd, EV_KEY/* 1 */, 57, 1);  // send volume-down key down event key 一个down 一个up才会上报
//        send_event(fd, EV_KEY/* 1 */, 57, 0);  // send volume-down key up event
        event_sync(fd);
//        send_event(fd, EV_KEY/* 1 */, 57, 1);  // send volume-down key down event
        sleep(3);
//		ret = write(fd, &ev, sizeof(struct input_event));
//		if (ret < 0) {
//			printf("read event error!\n");
//			exit(1);
//		}
		
//		if (ev.type == EV_KEY)
//			printf("type %d,code %d, value %d\n", ev.type, ev.code, ev.value);
	}
	
	return 0;
}

