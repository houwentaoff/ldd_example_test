/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  types.h
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  2024/06/23 13:29:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */
#ifndef __TYPES_H__
#define __TYPES_H__
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define CONFIG_64BIT

#ifdef CONFIG_64BIT
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif /* CONFIG_64BIT */

#ifndef BITS_PER_LONG_LONG
#define BITS_PER_LONG_LONG 64
#endif

#define BIT(nr)             (UL(1) << (nr))

#define BIT_ULL(nr)         (ULL(1) << (nr))
#define BIT_MASK(nr)        (UL(1) << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)        ((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)    (ULL(1) << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)    ((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE       8
#define ISB asm volatile ("isb sy" : : : "memory")
#define DSB asm volatile ("dsb sy" : : : "memory")
#define DMB asm volatile ("dmb sy" : : : "memory")

#define _dmb(opt)    asm volatile("dmb " #opt : : : "memory")
#define _dsb(opt)    asm volatile("dsb " #opt : : : "memory")

#define isb()   ISB
#define dsb()   DSB
#define dmb()   DMB

//#define mb()        _dsb(sy)
#define rmb()        _dsb(ld)
#define wmb()        _dsb(st)

#define dma_mb()    _dmb(osh)
#define dma_rmb()    _dmb(oshld)
#define dma_wmb()    _dmb(oshst)

#define smp_mb()    _dmb(ish)

# define barrier() __asm__ __volatile__("": : :"memory")

/*
 * TODO: The kernel offers some more advanced versions of barriers, it might
 * have some advantages to use them instead of the simple one here.
 */
#define mb()        dsb()
#define __iormb()   dmb()
#define __iowmb()   dmb()

/*
 * Generic virtual read/write.  Note that we don't support half-word
 * read/writes.  We define __arch_*[bl] here, and leave __arch_*w
 * to the architecture specific code.
 */
#define __arch_getb(a)          (*(volatile unsigned char *)(a))
#define __arch_getw(a)          (*(volatile unsigned short *)(a))
#define __arch_getl(a)          (*(volatile unsigned int *)(a))
#define __arch_getq(a)          (*(volatile unsigned long long *)(a))

#define __arch_putb(v,a)        (*(volatile unsigned char *)(a) = (v))
#define __arch_putw(v,a)        (*(volatile unsigned short *)(a) = (v))
#define __arch_putl(v,a)        (*(volatile unsigned int *)(a) = (v))
#define __arch_putq(v,a)        (*(volatile unsigned long long *)(a) = (v))

/*
 * Generic IO read/write.  These perform native-endian accesses.
 */
#define writeb(v,c) ({ uint8_t  __v = v; __iowmb(); __arch_putb(__v,c); __v; })
#define writew(v,c) ({ uint16_t __v = v; __iowmb(); __arch_putw(__v,c); __v; })
#define writel(v,c) ({ uint32_t __v = v; __iowmb(); __arch_putl(__v,c); __v; })
#define writeq(v,c) ({ uint64_t __v = v; __iowmb(); __arch_putq(__v,c); __v; })

#define readb(c)    ({ uint8_t  __v = __arch_getb(c); __iormb(); __v; })
#define readw(c)    ({ uint16_t __v = __arch_getw(c); __iormb(); __v; })
#define readl(c)    ({ uint32_t __v = __arch_getl(c); __iormb(); __v; })
#define readq(c)    ({ uint64_t __v = __arch_getq(c); __iormb(); __v; })
#define hang()      do{}while(1)
#define bugs()      hang()

#endif
