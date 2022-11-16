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
#include <assert.h>

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
drmModeModeInfo mminfo[]={
    
    /* 800*480*/
    {
        .name = "800x480",
        .hdisplay = 800, .hsync_start = 852, .hsync_end = 864, .htotal = 916, 
        .vdisplay = 480, .vsync_start = 488, .vsync_end = 538, .vtotal = 588, 
        .clock = 32316,
    },
    /* 800*480*/
    {
        .name = "800x480",
        .hdisplay = 800, .hsync_start = 808, .hsync_end = 850, .htotal = 861, 
        .vdisplay = 480, .vsync_start = 488, .vsync_end = 558, .vtotal = 588, 
        .clock = 30376,
    },
    /* 848*480*/
    {
        .name = "848x480x60x33750vid",
        .hdisplay = 848, .hsync_start = 864, .hsync_end = 976, .htotal = 1088, 
        .vdisplay = 480, .vsync_start = 486, .vsync_end = 494, .vtotal = 517, 
        .clock = 33750,
    },
    /* 1920*1080 */
    {
        .name = "1920x1080x60x148500vid",
        .hdisplay = 1920, .hsync_start = 2008, .hsync_end = 2052, .htotal = 2200, 
        .vdisplay = 1080, .vsync_start = 1084, .vsync_end = 1089, .vtotal = 1125, 
        .clock = 148500,
    },
    /* 1920*1080 */
    {
        .name = "1920x1080",
        .hdisplay = 1920, .hsync_start = 2008, .hsync_end = 2052, .htotal = 2200, 
        .vdisplay = 1080, .vsync_start = 1084, .vsync_end = 1089, .vtotal = 1125, 
        .clock = 148500,
    },
    /* 600*240 */
    {
        .name = "600x240",
        .hdisplay = 600, .hsync_start = 608, .hsync_end = 658, .htotal = 661, 
        .vdisplay = 240, .vsync_start = 288, .vsync_end = 338, .vtotal = 388, 
        .clock = 33750,
    },
};
static int fill_frame_buffer(int idx, int timing)
{

    unsigned char buffer[4]={0};
    int i = 0;
    int line_id = 0;
    int parten_cnt = 10;
    for (i=0; i<mminfo[timing].hdisplay * mminfo[timing].vdisplay; i++)
    {
    #if 0
        line_id = i / mminfo[timing].hdisplay;
        if (line_id < 10){        
            //pread(rgba_fd[idx], buffer, 4, i*4);
            frame_buffer[idx][i*3 + 0]= 0x2a + line_id;//(i % (0x0fa)) + 1;//0x00;//buffer[(idx+3)%4];b
            //frame_buffer[idx][i*3 + 1]= 0x5a;//buffer[(idx+2)%4];g
            frame_buffer[idx][i*3 + 1]= 0x2a + line_id;//(i % (0x0fa)) + 1;//buffer[(idx+2)%4];g
            frame_buffer[idx][i*3 + 2]= 0x2a + line_id;//buffer[(idx+1)%4];r
        }
        else{
            frame_buffer[idx][i*3 + 0]= 0x5a;//(i % (0x0fa)) + 1;//0x00;//buffer[(idx+3)%4];b
            //frame_buffer[idx][i*3 + 1]= 0x5a;//buffer[(idx+2)%4];g
            frame_buffer[idx][i*3 + 1]= 0x5a;//(i % (0x0fa)) + 1;//buffer[(idx+2)%4];g
            frame_buffer[idx][i*3 + 2]= 0x5a;//buffer[(idx+1)%4];r
        }
        //frame_buffer[idx][i*4 + 3]= 0x00;//buffer[(idx+0)%4];
        if (line_id == 0){
            if (i > 3){
                frame_buffer[idx][i*3 + 0]= 0xaa + line_id;//(i % (0x0fa)) + 1;//0x00;//buffer[(idx+3)%4];b
                frame_buffer[idx][i*3 + 1]= 0xab + line_id;//(i % (0x0fa)) + 1;//buffer[(idx+2)%4];g
                frame_buffer[idx][i*3 + 2]= 0xac + line_id;//buffer[(idx+1)%4];r
            }
        }
        #else
        
        pread(rgba_fd[idx], buffer, 4, i*4);
#if 0
            frame_buffer[idx][i*3 + 0]= 146;//b
            frame_buffer[idx][i*3 + 1]= 146;//g
            frame_buffer[idx][i*3 + 2]= 146;//r
            if (i % 800 == 0){
                frame_buffer[idx][i*3 + 0] = 0xf2;
                frame_buffer[idx][i*3 + 1] = 0xf2;
                frame_buffer[idx][i*3 + 2] = 0xf2;
                if (i != 0){
                   frame_buffer[idx][ (i-800)*3 + 3] = 0xf2; 
                   frame_buffer[idx][ (i-800)*3 + 4] = 0xf2; 
                   frame_buffer[idx][ (i-800)*3 + 5] = 0xf2; 
                }
            }
            if (i % 800 == 799){
                frame_buffer[idx][i*3 + 0] = 0xa5;
                frame_buffer[idx][i*3 + 1] = 0xa5;
                frame_buffer[idx][i*3 + 2] = 0xa5;
    }
#else
    #if 0
            frame_buffer[idx][i*3 + 0]= 0 ;//b
            frame_buffer[idx][i*3 + 1]= 0xff;//(i % (0x0fa)) + 1;//buffer[(idx+2)%4];g
            frame_buffer[idx][i*3 + 2]= 0;//buffer[(idx+1)%4];r
    #else
            frame_buffer[idx][i*3 + 0]= buffer[2];//b
            frame_buffer[idx][i*3 + 1]= buffer[1];//(i % (0x0fa)) + 1;//buffer[(idx+2)%4];g
            frame_buffer[idx][i*3 + 2]= buffer[0];//buffer[(idx+1)%4];r
    #endif            
#endif
        #endif
    }
    
    //frame_buffer[idx][ 3*(mminfo[timing].hdisplay * mminfo[timing].vdisplay - 2) +2 ] = 0x6a;
    //frame_buffer[idx][ 3*(mminfo[timing].hdisplay * mminfo[timing].vdisplay - 1) + 0] = 0x6a;
    //frame_buffer[idx][ 3*(mminfo[timing].hdisplay * mminfo[timing].vdisplay - 1) + 1] = 0x6a;
    //frame_buffer[idx][ 3*(mminfo[timing].hdisplay * mminfo[timing].vdisplay - 1) + 2] = 0x6a;
    return 0;
}
static int fill_drm(struct buffer_object *bo, int random, int timing)
{
    int i = 0;
    unsigned char buffer[4]={0};
    if (!bo){
        printf("bo is null\b");
        return -1;
    }
    memcpy(bo->vaddr, frame_buffer[random %4], mminfo[timing].hdisplay * mminfo[timing].vdisplay*3);

    return 0;
}
static int modeset_create_fb(int fd, struct buffer_object *bo, int random)
{
    struct drm_mode_create_dumb create = {};
     struct drm_mode_map_dumb map = {};

    /* create a dumb-buffer, the pixel format is XRGB888 */
    create.width = bo->width;
    create.height = bo->height;
    if (bo->width == 1920)
        create.bpp = 24;
    else
        create.bpp = 24;
    drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create);
    perror("create dump");

    /* bind the dumb-buffer to an FB object */
    //create.pitch = create.width * 3;
    
    bo->pitch = create.pitch;
    
    bo->size = create.size;
    bo->handle = create.handle;
    drmModeAddFB(fd, bo->width, bo->height, 24/*create.bpp*/, create.bpp, bo->pitch,
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
static void dump_drm(drmModeRes *res)
{
    printf("=>%s\n", __func__);
    for (int i=0; i<res->count_fbs; i++){
        printf("fbs[%d]\n", res->fbs[i]);
    }
    for (int i=0; i<res->count_crtcs; i++){
        printf("crtcs[%d]\n", res->crtcs[i]);
    }
    for (int i=0; i<res->count_connectors; i++){
        printf("connectors[%d]\n", res->connectors[i]);
    }
    for (int i=0; i<res->count_encoders; i++){
        printf("encoders[%d]\n", res->encoders[i]);
    }
    printf("<=%s\n", __func__);
}
static void dump_mode(drmModeModeInfo *mode)
{
    printf("  %s %d %d %d %d %d %d %d %d %d %d",
           mode->name,
           mode->vrefresh,
           mode->hdisplay,
           mode->hsync_start,
           mode->hsync_end,
           mode->htotal,
           mode->vdisplay,
           mode->vsync_start,
           mode->vsync_end,
           mode->vtotal,
           mode->clock);

    //printf(" flags: ");
    //mode_flag_str(mode->flags);
    //printf("; type: ");
    //mode_type_str(mode->type);
    printf("\n");
}

static void init_timing(int id, drmModeModeInfo *ptr)
{
    /*
    {
        .pixelclock = 40*1000*1000,
        .width = 800,
        .height = 480,

        .hfrontporch = 8,
        .hsync = 50,
        .hbackporch = 3,

        .vfrontporch = 8,
        .vsync = 50,
        .vbackporch = 50,
    },

    */

    int i = id;
    dump_mode(ptr);
    ptr->hdisplay = mminfo[i].hdisplay;
    ptr->hsync_start = mminfo[i].hsync_start;
    ptr->hsync_end = mminfo[i].hsync_end;
    ptr->htotal = mminfo[i].htotal;
    
    ptr->vdisplay = mminfo[i].vdisplay;
    ptr->vsync_start = mminfo[i].vsync_start;
    ptr->vsync_end = mminfo[i].vsync_end;
    ptr->vtotal = mminfo[i].vtotal;
    //ptr->clock = 20000;//mminfo[i].clock;
    //strcpy(ptr->name, mminfo[i].name);
    dump_mode(ptr);
    return;
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
    int init_resultion = 0;
    int i = 0;
    int j = 0;
    int mode_id = 0;
    
    if (argc < 3){
        printf("eg:%s /dev/dri/card0 crtc_id:86 5000 1920x1080.rgba 1920x1080-dog.rgba 1920x1080.rgba 1920x1080-dog.rgba init_timing_id\n", argv[0]);
        printf("eg:800x480 ok: %s /dev/dri/card0 135 5000 800x480.rgba 800x480.rgba 800x480.rgba 800x480.rgba 0 2\n", argv[0]);
        exit(2);
    }
    fd = open(argv[1], O_RDWR | O_CLOEXEC);
    usleep_time = atoi(argv[3]);
    rgba_file[0] = argv[4];
    rgba_file[1] = argv[5];
    rgba_file[2] = argv[6];
    rgba_file[3] = argv[7];
    init_resultion = atoi(argv[8]);
    mode_id = atoi(argv[9]);
    //drmSetClientCap(fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);

    res = drmModeGetResources(fd);
    assert(res);

    printf("count_fbs[%d] count_crtcs[%d]count_connectors[%d]count_encoders[%d]\n",
                    res->count_fbs, res->count_crtcs,
                    res->count_connectors, res->count_encoders);
    
    dump_drm(res);
    crtc_id = res->crtcs[0];
    
    printf("=>%s line[%d]\n", __func__, __LINE__);
    
    for (i=0; i<1; i++){
        conn_id = res->connectors[i];
        conn = drmModeGetConnector(fd, conn_id);
        printf("i[%d]conn_id[%d]count_modes[%d]\n", i, conn_id, conn->count_modes);
        for (j=0; j<conn->count_modes; j++){
            printf("width[%d]height[%d]\n",
                conn->modes[j].hdisplay,
                conn->modes[j].vdisplay);
                dump_mode(&conn->modes[j]);
        }

    }
    printf("<=%s line[%d]\n", __func__, __LINE__);

    conn_id = res->connectors[0];
    
    printf("=>%s line[%d] conn_id[%d] crtc_id[%d]\n",
            __func__, __LINE__, conn_id, crtc_id);

    //crtc_id = res->crtcs[1];
    //conn_id = res->connectors[1];
    conn = drmModeGetConnector(fd, conn_id);
    printf("count_modes[%d]\n", conn->count_modes);
    for (int i=0; i<conn->count_modes; i++){
        printf("width[%d]height[%d]\n",
            conn->modes[i].hdisplay,
            conn->modes[i].vdisplay);
    }
    printf("over\n");

    
    for (i =0; i<4; i++)
    {
        if (mminfo[init_resultion].hdisplay == 800){
            buf[i].width = 800;//conn->modes[0].hdisplay;
            buf[i].height = 480;//conn->modes[0].vdisplay;
        }
        else if (mminfo[init_resultion].hdisplay == 600){
            buf[i].width = 600;//conn->modes[0].hdisplay;
            buf[i].height = 240;//conn->modes[0].vdisplay;
        }
        else{
            buf[i].width = conn->modes[mode_id].hdisplay;
            buf[i].height = conn->modes[mode_id].vdisplay;
        }
    }
    printf("width[%d]height[%d]\n", buf[0].width, buf[0].height);
    for (int i=0; i<4; i++)
    {
        rgba_fd[i] = open(rgba_file[i], O_RDONLY);
    }

    crtc_id=atoi(argv[2]);	// 这个要对应
    for (int i=0; i<4; i++)
    {
        frame_buffer[i] = malloc(mminfo[init_resultion].hdisplay * mminfo[init_resultion].vdisplay * 4);

        if (!frame_buffer[i]){
            perror("malloc err\n");
        }
        modeset_create_fb(fd, &buf[i], random);
        fill_frame_buffer(i, init_resultion);
    }
    
    //dump_mode(&conn->modes[mode_id]);
    
    if (mminfo[init_resultion].hdisplay == 800){
        init_timing(init_resultion, &conn->modes[mode_id]);
    }else{
        ;//init_timing(init_resultion, &conn->modes[mode_id]);
    }
    printf("mminfo[init_resultion].hdisplay[%d]\n", mminfo[init_resultion].hdisplay);
    while (1){
        fill_drm(&buf[random % 4], random, init_resultion);
        if (mminfo[init_resultion].hdisplay == 800){
        drmModeSetCrtc(fd, crtc_id, buf[random % 4].fb_id,
                    0, 0, &conn_id, 1, &conn->modes[mode_id]);
        }
        if (mminfo[init_resultion].hdisplay == 600){
            drmModeSetCrtc(fd, crtc_id, buf[random % 4].fb_id,
                    0, 0, &conn_id, 1, &conn->modes[3]);
        }
        else{
            drmModeSetCrtc(fd, crtc_id, buf[random % 4].fb_id,
                    0, 0, &conn_id, 1, &conn->modes[mode_id]);
        }
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
        //random++;
        //printf("new frame random[%d]\n", random);
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
