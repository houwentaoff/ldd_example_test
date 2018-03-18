#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>


typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef long LONG;


typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
}__attribute__((packed)) BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
}__attribute__((packed)) BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
}__attribute__((packed)) RGBQUAD;


#define FRAME_BUFFER_PATH                "/dev/fb0"


int main(int argc, char *argv[])
{  
        int i;
        int img_fd, fb_fd;
        int data_size;
        char *img_buf;
        struct fb_var_screeninfo var_info;
        BITMAPFILEHEADER file_head;
        BITMAPINFOHEADER info_head;
        //RGBQUAD rgb_quad;
        
        if (argc != 2)
        {
                printf("usage %s bmp_file\n", argv[0]);
                return -1;
        }        
        
        /*open files*/
        fb_fd = open(FRAME_BUFFER_PATH, O_RDWR);
        if (fb_fd < 0)
        {
                perror("open framebuff");
                return -1;
        }
        if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &var_info) < 0)
        {
                perror("ioctl FBIOGET_VSCREENINFO");
                close(img_fd);
                return 0;
        }
        printf("xres %d, yres %d\n", var_info.xres, var_info.yres);
        
        img_fd = open(argv[1], O_RDWR | O_CREAT, 0644);
        if (img_fd < 0)
        {
                perror("open image");
                close(img_fd);
                return -1;
        }
        
        data_size = var_info.xres*var_info.yres*(var_info.bits_per_pixel/8);
        /*initialize bmp structs*/
        file_head.bfType = 0x4d42;
        file_head.bfSize = sizeof(file_head) + sizeof(info_head) + data_size;
        file_head.bfReserved1 = file_head.bfReserved2 = 0;
        file_head.bfOffBits = sizeof(file_head) + sizeof(info_head);
        
        info_head.biSize = sizeof(info_head);
        info_head.biWidth = var_info.xres;
        info_head.biHeight = var_info.yres;
        info_head.biPlanes = 1;
        info_head.biBitCount = var_info.bits_per_pixel;
        info_head.biCompression = 0;
        info_head.biSizeImage = 0;
        info_head.biXPelsPerMeter = 0;
        info_head.biYPelsPerMeter = 0;
        info_head.biClrUsed = 0;
        info_head.biClrImportant = 0;
        
        img_buf = (char *)malloc(data_size);
        if (img_buf == NULL)
        {
                printf("malloc failed!\n");
                close(fb_fd);
                close(img_fd);
                return -1;
        }
  
  /*read img data and */
  read(fb_fd, img_buf, data_size);
  
  write(img_fd, &file_head, sizeof(file_head));
  write(img_fd, &info_head, sizeof(info_head));
  
  /*revese img and write to file*/
  for (i = 0; i < var_info.yres; i++)
  {
          write(img_fd, img_buf + var_info.xres*(var_info.yres-i-1)*(var_info.bits_per_pixel/8), 
                  var_info.xres*(var_info.bits_per_pixel/8));
  }
  
  close(fb_fd);
        close(img_fd);
        return 0;
}
