/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_mmap_app.c
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  02/26/2017 09:29:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
    int fd;
    unsigned char *pmap = NULL;
    int i = 0;

    fd = open("/dev/housir_mmap", O_RDWR);
    if (fd < 0)
    {
        perror("open error!\n");
        return -1;
    }
    pmap = (unsigned char *)mmap(NULL, 512, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (pmap == MAP_FAILED)
    {
        perror("mmap fail!\n");
        return -1;
    }
    for (i=0; i<10; i++)
    {
        printf("0x%x ", pmap[i]);
        pmap[i] = 0x88;
    }
    printf("\n");
    while (1)
    {
        sleep(1);
    }
    munmap(pmap, 512);
    close(fd);
    return EXIT_SUCCESS;
}
