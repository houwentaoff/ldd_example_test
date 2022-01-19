/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  hello.c
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  01/16/2022 12:06:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */


#include	<stdlib.h>
#include <string.h>
//int __printf(1, 2) tom_printf(const char *fmt, ...);
int (*tom_printf)(const char *,...);
static int a=0x23;
static int b=0x0;
static int c;
int d= 0x34;
int e;
void _exit(int status)
{
    (void *)(status);
}
int test(char *p)
{
    int i = 0;
    tom_printf("strlen[%d]\n", strlen(p));
    tom_printf("a[0x%x]b[0x%x] c[0x%x] d[0x%x] e[0x%x]\n", a, b, c, d, e);
    /* Entry point为 main,并没有从start.s初始化C库，所以这些需要初始化堆栈的都会挂掉. */
#if 0  //挂掉
    char *ma = malloc(100);
    if (!ma){
        tom_printf("ma is null\n");
    }else{
        tom_printf("malloc successed!\n");
        for (i=0; i<10; i++)
        {
            ma[i] = i&0xff;
            tom_printf("ma[0x%x]\n", ma[i]);
        }

        memset(ma, 0, 100);
        tom_printf("over 1\n");
        for (i=0; i<20; i++)
        {
            tom_printf("[0x%x]", ma[i]);
        }
        tom_printf("over 2\n");
    }
#endif
    char *qq = (void *)0x900000;
    memset(qq, 1, 10);
    for (i =0; i<10; i++){
        tom_printf("qq[0x%x]\n", qq[i]);
    }
    return 0;
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
//int main (void )
{
    int i=0;
    char *p = "hello\0";
    tom_printf = (void *)0x7ef7b6e4;//0x26b6d0;
    tom_printf("0x%lx : [%s]\n", p, p);   
    for (i=0; i<5; i++){
        tom_printf("i[%d]\n", i);   
    }
    test(p);
    return EXIT_SUCCESS;
}
