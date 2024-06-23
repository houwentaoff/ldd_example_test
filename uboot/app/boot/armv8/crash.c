/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  crash.c
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  06/22/24 23:30:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */
#include "types.h"
#include "sysreg.h"
int printf(const char *format, ...);
#define MAX_BIN_PRINT    (64)
static char binary[MAX_BIN_PRINT]={0};
char *hex2binarystr(int num, int len) {
    unsigned int mask = 1 << (len - 1);//(sizeof(num) * 8 - 1);
    int i = 0;
    for (i=0; i<MAX_BIN_PRINT;i++){
        binary[i] = 0;
    }
    i = 0;
    while (mask) {
        binary[i++] = (num & mask) ? '1' : '0';
        //uart_printf((num & mask) ? '1' : '0'); // 打印当前位的二进制值
        mask >>= 1; // 移动掩码到下一位
    }
    return binary;
}

/*-----------------------------------------------------------------------------
 *  EC-CODE
 *  0b000000    Unknown reason
 *  ....
 *  0b001110    Illegal Execution state
 *  0b100000    Instruction Abort  ref spec
 *  0b100001    Instruction Abort  ref spec
 *  0b100010    PC alignment fault
 *  0b100100    Data Abort ref spec
 *  0b100101    Data Abort ref spec
 *  0b100110    SP alignment fault
 *-----------------------------------------------------------------------------*/

uint64_t _x30;
/* 根据自己需求修改,临时打印需要查看的寄存器的值 */
uint64_t _xuser;
void do_crash()
{
    int elx = 0;
    uint64_t elr,ec, ESR_ELx;

    asm volatile (
          "ldr x0, =_x30\n"
          "str x30, [x0]\n"  // 将X0寄存器的值读取到x0_value变量
          : // 输出操作数，"=r" 表示将结果存储在通用寄存器中
          :
          : "memory"
      );
  asm volatile (
        "ldr x0, =_xuser\n"
        "str x23, [x0]\n"  // 将X0寄存器的值读取到x0_value变量
        : // 输出操作数，"=r" 表示将结果存储在通用寄存器中
        :
        : "memory"
    );
    elx = get_elx();
    
    switch(elx){
        case 1:
            ESR_ELx = read_sysreg(ESR_EL1);
            elr = read_sysreg(ELR_EL1);
            break;
        case 2:
            ESR_ELx = read_sysreg(ESR_EL2);
            elr = read_sysreg(ELR_EL2);
            break;
        case 3:
            ESR_ELx = read_sysreg(ESR_EL3);
            elr = read_sysreg(ELR_EL3);
            break;
    }
    ec = (ESR_ELx >> 26) & 0x3f;
    printf("\t!dump crash!\n");
    printf("\tel[0x%llx] \n", elx);
    printf("\telrx[0x%llx] \n", elr);
    printf("\tx30[0x%llx] \n", _x30);
    printf("\txuser[0x%llx] \n", _xuser);
    printf("ESR_ELx[0x%llx]EC[0x%llx]-[0b%s] ", ESR_ELx, ec, hex2binarystr(ec, 6));
    bugs();
    return;
}
