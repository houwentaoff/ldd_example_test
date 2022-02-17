#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

struct buffer_object {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t handle;
    uint32_t size;
    uint8_t *vaddr;
    uint32_t fb_id;
};

struct buffer_object buf[4]={0};
static char *rgba_file[4] = {0};
static int  rgba_fd[4] = {0};
static unsigned char *frame_buffer[4]={0};
static int fill_frame_buffer(int idx)
{

    unsigned char buffer[4]={0};
    int i = 0;

    for (i=0; i<1920*1080; i++)
    {
        pread(rgba_fd[idx], buffer, 4, i*4);
        frame_buffer[idx][i*4 + 0]= buffer[(idx+3)%4];
        frame_buffer[idx][i*4 + 1]= buffer[(idx+2)%4];
        frame_buffer[idx][i*4 + 2]= buffer[(idx+1)%4];
        frame_buffer[idx][i*4 + 3]= buffer[(idx+0)%4];
    }
    return 0;
}
static int fill_drm(struct buffer_object *bo, int random)
{
    int i = 0;
    unsigned char buffer[4]={0};
    if (!bo){
        printf("bo is null\b");
        return -1;
    }
    memcpy(bo->vaddr, frame_buffer[random %4], 1920*1080*4);

    return 0;
}
static int modeset_create_fb(int fd, struct buffer_object *bo, int random)
{
    struct drm_mode_create_dumb create = {};
     struct drm_mode_map_dumb map = {};

    /* create a dumb-buffer, the pixel format is XRGB888 */
    create.width = bo->width;
    create.height = bo->height;
    create.bpp = 32;
    drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create);
    perror("create dump");

    /* bind the dumb-buffer to an FB object */
    bo->pitch = create.pitch;
    bo->size = create.size;
    bo->handle = create.handle;
    drmModeAddFB(fd, bo->width, bo->height, 24, 32, bo->pitch,
               bo->handle, &bo->fb_id);
    perror("addfb");

    /* map the dumb-buffer to userspace */
    map.handle = create.handle;
    drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map);
    perror("map dump");

    bo->vaddr = mmap(0, create.size, PROT_READ | PROT_WRITE,
            MAP_SHARED, fd, map.offset);
    perror("mmap");
    /* initialize the dumb-buffer with white-color */
    //memset(bo->vaddr, 150, bo->size);
    printf("bo size[%d]\n", bo->size);

    return 0;
}

static void modeset_destroy_fb(int fd, struct buffer_object *bo)
{
    struct drm_mode_destroy_dumb destroy = {};

    drmModeRmFB(fd, bo->fb_id);

    munmap(bo->vaddr, bo->size);

    destroy.handle = bo->handle;
    drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
}

int main(int argc, char **argv)
{
    int fd;
    drmModeConnector *conn;
    drmModeRes *res;
    uint32_t conn_id;
    uint32_t crtc_id;
    unsigned int random = 0;
    int usleep_time = 0;

    if (argc < 3){
        printf("eg:%s /dev/dri/card0 86 5000 1920x1080.rgba 1920x1080-dog.rgba 1920x1080.rgba 1920x1080-dog.rgba\n", argv[0]);
        exit(2);
    }
    fd = open(argv[1], O_RDWR | O_CLOEXEC);
    usleep_time = atoi(argv[3]);
    rgba_file[0] = argv[4];
    rgba_file[1] = argv[5];
    rgba_file[2] = argv[6];
    rgba_file[3] = argv[7];
    //drmSetClientCap(fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);

    res = drmModeGetResources(fd);
    perror("mode get res err!");
    crtc_id = res->crtcs[0];
    conn_id = res->connectors[0];
    printf("conn_id[%d] crtc_id[%d]\n", conn_id, crtc_id);
    printf("conn_id[%d] crtc_id[%d]\n", res->connectors[1], res->crtcs[1]);

    crtc_id = res->crtcs[1];
    conn_id = res->connectors[1];
    conn = drmModeGetConnector(fd, conn_id);
    for (int i =0; i<4; i++)
    {
        buf[i].width = conn->modes[1].hdisplay;
        buf[i].height = conn->modes[1].vdisplay;
    }
    printf("width[%d]height[%d]\n", buf[0].width, buf[0].height);
    for (int i=0; i<4; i++)
    {
        rgba_fd[i] = open(rgba_file[i], O_RDONLY);
    }

    crtc_id=atoi(argv[2]);	// 这个要对应
    for (int i=0; i<4; i++)
    {
        frame_buffer[i] = malloc(1920*1080*4);
        if (!frame_buffer[i]){
            perror("malloc err\n");
        }
        modeset_create_fb(fd, &buf[i], random);
        fill_frame_buffer(i);
    }

    while (1){
        fill_drm(&buf[random % 4], random);
        drmModeSetCrtc(fd, crtc_id, buf[random % 4].fb_id,
                0, 0, &conn_id, 1, &conn->modes[1]);
#if 0
        if (random % 4)
        {
//            fill_drm(&buf[0], random);
            drmModeSetCrtc(fd, crtc_id, buf[0].fb_id,
                    0, 0, &conn_id, 1, &conn->modes[1]);
        }
        else
        {
            fill_drm(&buf[1], random);
            drmModeSetCrtc(fd, crtc_id, buf[1].fb_id,
                    0, 0, &conn_id, 1, &conn->modes[1]);
        }
#endif
        random++;
//        printf("new frame random[%d]\n", random);
        if (usleep_time != 0)
        {
            usleep(usleep_time);
        }
    }
    perror("setcrtc!");
    getchar();
    for (int i=0; i<4; i++)
    {
        modeset_destroy_fb(fd, &buf[i]);
    }

    drmModeFreeConnector(conn);
    drmModeFreeResources(res);

    close(fd);

    return 0;
}
