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

struct buffer_object buf;

static int modeset_create_fb(int fd, struct buffer_object *bo)
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
    int i = 0;
    for (i=0; i<1920*1080; i++)
    {
        bo->vaddr[i*4 + 0]= 0x00;
        bo->vaddr[i*4 + 1]= 0xff;
        bo->vaddr[i*4 + 2]= 0x00;
        bo->vaddr[i*4 + 3]= 0x00;
    }
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

    fd = open("/dev/dri/card1", O_RDWR | O_CLOEXEC);

    //drmSetClientCap(fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);

    res = drmModeGetResources(fd);
    perror("mode get res err!");
    crtc_id = res->crtcs[0];
    conn_id = res->connectors[0];

    conn = drmModeGetConnector(fd, conn_id);
    buf.width = conn->modes[0].hdisplay;
    buf.height = conn->modes[0].vdisplay;

    modeset_create_fb(fd, &buf);
    crtc_id=74;	// 这个要对应
    drmModeSetCrtc(fd, crtc_id, buf.fb_id,
            0, 0, &conn_id, 1, &conn->modes[0]);
    perror("setcrtc!");
    getchar();

    modeset_destroy_fb(fd, &buf);

    drmModeFreeConnector(conn);
    drmModeFreeResources(res);

    close(fd);

    return 0;
}
