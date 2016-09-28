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

    // sync (0,0,0)
    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    if (write(fd, &event, sizeof(event)) < 0) {
        perror("send_event error\n");
    }
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
        send_event(fd, EV_KEY/* 1 */, 57, 1);  // send volume-down key down event
        send_event(fd, EV_KEY/* 1 */, 57, 0);  // send volume-down key up event
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

