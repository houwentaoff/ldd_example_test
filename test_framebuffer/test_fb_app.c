/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_fb_app.c
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  03/17/2018 07:18:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <linux/kd.h>/*新添加的，用于进行图形模式时ioctl使用*/
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

typedef struct
{
    char cfType[2];         /* 文件类型, 必须为 "BM" (0x4D42) */
    char cfSize[4];         /* 文件的大小(字节) */
    char cfReserved[4];     /* 保留, 必须为 0 */
    char cfoffBits[4];      /* 位图阵列相对于文件头的偏移量(字节) */
} BITMAPFILEHEADER;       /* 文件头结构 */

typedef struct
{
    char ciSize[4];         /* size of BITMAPINFOHEADER */
    char ciWidth[4];        /* 位图宽度(像素) */
    char ciHeight[4];       /* 位图高度(像素) */
    char ciPlanes[2];       /* 目标设备的位平面数, 必须置为1 */
    char ciBitCount[2];     /* 每个像素的位数, 1,4,8或24 */
    char ciCompress[4];     /* 位图阵列的压缩方法,0=不压缩 */
    char ciSizeImage[4];    /* 图像大小(字节) */
    char ciXPelsPerMeter[4];/* 目标设备水平每米像素个数 */
    char ciYPelsPerMeter[4];/* 目标设备垂直每米像素个数 */
    char ciClrUsed[4];      /* 位图实际使用的颜色表的颜色数 */
    char ciClrImportant[4]; /* 重要颜色索引的个数 */
} BITMAPINFOHEADER;       /* 位图信息头结构 */

typedef struct
{
    char rgbBlue;
    char rgbGreen;
    char rgbRed;
    char rgbReserved;
} RGBQUAD;

BITMAPFILEHEADER FileHead;
BITMAPINFOHEADER InfoHead;
RGBQUAD rgbquad;

char *fbp = 0;
int xres = 0;
int yres = 0;
int bits_per_pixel = 0;
int tty;
int tty_mode_was;

int set_tty_graphics( void );
int set_tty_text( void );
int  show_bmp  ( char *bmpfile );
long chartolong( char * string, int length );



void printVariableInfo (struct fb_var_screeninfo vinfo)  
{  
    printf ("Variable screen info:\n"  
            "\txres:%d\n"  
            "\tyres:%d\n"  
            "\txres_virtual:%d\n"  
            "\tyres_virtual:%d\n"  
            "\tyoffset:%d\n"  
            "\txoffset:%d\n"  
            "\tbits_per_pixel:%d\n"  
            "\tgrayscale:%d\n"  
            "\tred: offset:%2d, length: %2d, msb_right: %2d\n"  
            "\tgreen: offset:%2d, length: %2d, msb_right: %2d\n"  
            "\tblue: offset:%2d, length: %2d, msb_right: %2d\n"  
            "\ttransp: offset:%2d, length: %2d, msb_right: %2d\n"  
            "\tnonstd:%d\n"  
            "\tactivate:%d\n"  
            "\theight:%d\n"  
            "\twidth:%d\n"  
            "\taccel_flags:0x%x\n"  
            "\tpixclock:%d\n"  
            "\tleft_margin:%d\n"  
            "\tright_margin: %d\n"  
            "\tupper_margin:%d\n"  
            "\tlower_margin:%d\n"  
            "\thsync_len:%d\n"  
            "\tvsync_len:%d\n"  
            "\tsync:%d\n"  
            "\tvmode:%d\n"  
            "\n",  
        vinfo.xres, vinfo.yres, vinfo.xres_virtual, vinfo.yres_virtual,  
        vinfo.xoffset, vinfo.yoffset, vinfo.bits_per_pixel,  
        vinfo.grayscale, vinfo.red.offset, vinfo.red.length,  
        vinfo.red.msb_right,vinfo.green.offset, vinfo.green.length,  
        vinfo.green.msb_right, vinfo.blue.offset, vinfo.blue.length,  
        vinfo.blue.msb_right, vinfo.transp.offset, vinfo.transp.length,  
        vinfo.transp.msb_right, vinfo.nonstd, vinfo.activate,  
        vinfo.height, vinfo.width, vinfo.accel_flags, vinfo.pixclock,  
        vinfo.left_margin, vinfo.right_margin, vinfo.upper_margin,  
        vinfo.lower_margin, vinfo.hsync_len, vinfo.vsync_len,  
        vinfo.sync, vinfo.vmode);  
}  

/******************************************************************************
 *
 ******************************************************************************/
int main( int argc, char *argv[] )
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd)
    {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed information.\n");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading variable information.\n");
        exit(3);
    }
    printVariableInfo(vinfo);
    //printf("分辨率:%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );
    xres = vinfo.xres;
    yres = vinfo.yres;
    bits_per_pixel = vinfo.bits_per_pixel;

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
            fbfd, 0);
    if ((int)fbp == -1)
    {
        printf("Error: failed to map framebuffer device to memory.\n");
        exit(4);
    }
#if 0
    if (set_tty_graphics())
    {
        show_bmp( argv[1] );
    }
    sleep( 4 );
    set_tty_text();
#endif

    munmap(fbp, screensize);

    close(fbfd);
    return 0;
}

/******************************************************************************
 *
 ******************************************************************************/
int show_bmp( char *bmpfile )
{
    FILE *fp;
    int rc;
    int ciBitCount, ciWidth, ciHeight;
    int line_x, line_y;
    long int location = 0, BytesPerLine = 0;
    char tmp[1024*10];

    /* 打开位图文件 */
    fp = fopen( bmpfile, "rb" );
    if (fp == NULL)
    {
        return( -1 );
    }

    /* 读取位图文件头 */
    rc = fread( &FileHead, 1, sizeof(BITMAPFILEHEADER), fp );
    if ( rc != sizeof( BITMAPFILEHEADER ) )
    {
        fclose( fp );
        return( -2 );
    }

    /* 判断位图的类型 */
    if (memcmp(FileHead.cfType, "BM", 2) != 0)
    {
        fclose( fp );
        return( -3 );
    }

    /* 读取位图信息头 */
    rc = fread( (char *)&InfoHead, 1, sizeof(BITMAPINFOHEADER), fp );
    if ( rc != sizeof(BITMAPINFOHEADER) )
    {
        fclose( fp );
        return( -4 );
    }

    ciWidth    = (int) chartolong( InfoHead.ciWidth,    4 );
    ciHeight   = (int) chartolong( InfoHead.ciHeight,   4 );
    ciBitCount = (int) chartolong( InfoHead.ciBitCount, 4 );

    line_x = line_y = 0;
    /*
       while( !feof( fp ) )
       {
       rc = fread( (char *)&rgbquad, 1, sizeof(RGBQUAD), fp );
       if ( rc != sizeof(RGBQUAD) )
       {
       break;
       }

       location = line_x * bits_per_pixel / 8 + (ciHeight - line_y - 1) * xres * bits_per_pixel / 8;

     *(fbp + location) = rgbquad.rgbBlue;
     *(fbp + location + 1) = rgbquad.rgbGreen;
     *(fbp + location + 2) = rgbquad.rgbRed;
     *(fbp + location + 3) = rgbquad.rgbReserved;

     line_x++;
     if ( line_x == ( ciWidth - 1 ) )
     {
     line_x = 0;
     line_y++;
     }
     }
     */
    BytesPerLine = (ciWidth * ciBitCount + 31) / 32 * 4;

    while( !feof( fp ) )
    {
        rc = fread( tmp, 1, BytesPerLine, fp );
        if ( rc != BytesPerLine )
        {
            break;
        }

        location = (ciHeight - line_y - 1) * xres * bits_per_pixel / 8;
        memcpy( (fbp + location) , tmp, BytesPerLine );

        line_y++;
    }
    fclose( fp );
    return( 0 );
}

/******************************************************************************
 *
 ******************************************************************************/
long chartolong( char * string, int length )
{
    long number;

    if (length <= 4)
    {
        memset( &number, 0x00, sizeof(long) );
        memcpy( &number, string, length );
    }

    return( number );
}

/******************************************************************************
 *
 ******************************************************************************/
int set_tty_graphics( void )
{
    int ret = 1;

    if(0 > (tty = open("/dev/tty0", O_RDWR)))
    {
        printf( "ERROR: /dev/tty0 open failed\n" );
        ret = 0;
    }
    else if(0 > ioctl(tty, KDGETMODE, &tty_mode_was))
    {
        printf( "ERROR: /dev/tty0 get mode failed\n");
        close(tty);
        ret = 0;
    }
    else if((KD_GRAPHICS != tty_mode_was)
            && (0 > ioctl(tty, KDSETMODE, KD_GRAPHICS)))
    {
        printf( "ERROR: /dev/tty0 set graphics failed\n");
        close(tty);
        ret = 0;
    }
    else
    {
        printf("tty mode was ");
        switch(tty_mode_was)
        {
            case KD_TEXT     :  printf("KD_TEXT"     ); break;
            case KD_GRAPHICS :  printf("KD_GRAPHICS" ); break;
            default          :  printf("IMPOSSIBLE!" ); break;
        }
        printf("\n");
        printf("tty mode set to KD_GRAPHICS\n");
        fflush(stdout);
        close(tty);
    }
    return( ret );
}

/******************************************************************************
 *
 ******************************************************************************/
int set_tty_text( void )
{
    int ret = 1;

    if(0 > (tty = open("/dev/tty0", O_RDWR)))
    {
        printf( "ERROR: /dev/tty0 open failed\n" );
        ret = 0;
    }
    else if(0 > ioctl(tty, KDSETMODE, tty_mode_was))
    {
        printf( "ERROR: /dev/tty0 reset mode failed\n");
        close(tty);
        ret = 0;
    }
    else
    {
        printf("tty mode set to ");
        switch(tty_mode_was)
        {
            case KD_TEXT     :  printf("KD_TEXT"     ); break;
            case KD_GRAPHICS :  printf("KD_GRAPHICS" ); break;
            default          :  printf("IMPOSSIBLE!" ); break;
        }
        printf("\n");
        fflush(stdout);
        close(tty);
    }
    return( ret );
}
