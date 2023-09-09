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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define UART2_BASE 0xff1a0000

#define __stringify_1(x...)    #x
#define __stringify(x...)    __stringify_1(x)
/*
 * Unlike read_cpuid, calls to read_sysreg are never expected to be
 * optimized away or replaced with synthetic values.
 */
#define read_sysreg(r) ({                    \
    uint64_t __val;                        \
    asm volatile("mrs %0, " __stringify(r) : "=r" (__val));    \
    __val;                            \
})

//int __printf(1, 2) tom_printf(const char *fmt, ...);
int (*tom_printf)(const char *,...);
static int a=0x23;
static int b=0x0;
static int c;
int d= 0x34;
int e;
void _exit(int status)
{
   // (void *)(status);
    //return;
}
 /* :TODO:05/02/2023 02:11:18 PM:hwt: 使用malloc需实现_sbrk */
void* _sbrk(int increment)
{
    extern char end; // From linker script
    static char* heap_end = &end;

    char* current_heap_end = heap_end;
    heap_end += increment;
    return current_heap_end;
}
 /* :TODO:End---  */
 /* :TODO:05/02/2023 01:46:15 PM:hwt: printf 需要重载如下函数才能使用 */
/*-----------------------------------------------------------------------------
 *  printf
 *-----------------------------------------------------------------------------*/
void uart_putc_polled(char c)
{  
#define COM_UART_BASE       UART2_BASE
#define COM_UART_TSR     ((volatile unsigned int *)(COM_UART_BASE + 0x0014)) 
#define COM_UART_THR     ((volatile unsigned int *)(COM_UART_BASE + 0x0)) 
    while((*COM_UART_TSR&0x20) != 0x20);
        *COM_UART_THR = c;
}
int _lseek(int file, int ptr, int dir)
{
    return -1;
}
int _read(int file, char *ptr, int len)
{
 
}
int _fstat(int file)
{
    return 0;
}
 
int _isatty(int fd)
{
    return 0;
}
int _close(int file)   
{                      
    return -1;         
}
int _getpid(void)
{
  return 1;
}
int _kill(int pid, int sig)
{
    return -1;
}
int _write(int file, char *ptr, int len)
{
    int i;
    for (i = 0; i < len; ++i)
    {
        if (ptr[i] == '\n')
        {
            uart_putc_polled('\r');
        }
        uart_putc_polled(ptr[i]);
    }
    return len;
}
 /* :TODO:End---  */

int test(char *p)
{
    int i = 0;
    int currentel = (read_sysreg(CurrentEL) >> 2) & 0x3;

    printf("CurrentEL a: %d\n", currentel);
    printf("CurrentEL b: %d\n", (read_sysreg(CurrentEL) >> 2) & 0x3);

    tom_printf("strlen[%d]\n", strlen(p));
    tom_printf("a[0x%x]b[0x%x] c[0x%x] d[0x%x] e[0x%x]\n", a, b, c, d, e);
    tom_printf("bss:b[0x%x] c[0x%x] e[0x%x]\n", b, c, e);
    /* Entry point为 main,并没有从start.s初始化C库，所以这些需要初始化堆栈的都会挂掉. */
#if 1  
    
    /*-----------------------------------------------------------------------------
     *  1. start.s中实现了memset bss,和链接脚本中有.heap的字段即可(heap字段哪里有使用)
     *  2. c中需要实现_sbrk 在申请的时候堆指针++或者--
     *  3. 不用初始化c库即可使用malloc
     *-----------------------------------------------------------------------------*/
    char *ma = malloc(100);
    if (!ma){
        tom_printf("malloc is null\n");
    }else{
        tom_printf("malloc successed!\n");
        for (i=0; i<10; i++)
        {
            ma[i] = i&0xff;
            tom_printf("ma[addr[0x%x]:0x%x]\n", ma+i, ma[i]);
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
#if 0
    char *qq = (void *)0x900000;
    memset(qq, 1, 10);
    for (i =0; i<10; i++){
        tom_printf("qq[addr[0x%x0]:0x%x]\n", qq+i, qq[i]);
    }
#endif
    return 0;
}
int test_vector()
{
    int ret = 0;
    int *p = 0xffffffff;

    *p = 123;
    printf("=>%s \n", __func__);
    return ret;
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
    int i=0;
    char *p = "hello\0";
    tom_printf = (void *)0x7ff7aac4;//0x7ef7b6e4;//0x26b6d0;
    tom_printf("0x%lx : [%s]\n", p, p);   
    for (i=0; i<5; i++){
        tom_printf("i[%d]\n", i);   
    }
    //printf("这是什么\n");
    printf("printf[addr:0x%x] this is api printf test! i[%d]\n", &printf, i);
    printf("float[%f]\n", 1.23);
    test(p);
    test_vector();
    return EXIT_SUCCESS;
}
