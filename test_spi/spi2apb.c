/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  spi_drv.c
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  04/04/2022 09:31:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>

#include <linux/delay.h>
#include "spi2apb.h"

uint8_t int8_msb2lsb(uint8_t src)
{
    int i;
    uint8_t dst = 0;

    for (i = 0; i < 8; i++) {
        uint8_t t = 0;
        t = (((src >> i) & 0x01) << (7 - i));
        dst |= t;
    }

    return dst;
}

uint32_t int32_msb2lsb(uint32_t src)
{
    uint32_t dst = 0;
    int i;

    for (i = 0; i < 4; i++) {
        int shift = i * 8;
        dst = dst | (int8_msb2lsb((src >> shift) & 0xff) << shift);
    }

    return dst;
}

int spi2apb_lsb_w32(struct spi_device *spi,
        int32_t addr, int32_t data)
{
    int32_t write_cmd = APB_CMD_WRITE;
    struct spi_transfer write_cmd_packet = {
        .tx_buf = &write_cmd,
        .len    = sizeof(write_cmd),
    };
    struct spi_transfer addr_packet = {
        .tx_buf = &addr,
        .len    = sizeof(addr),
    };
    struct spi_transfer data_packet = {
        .tx_buf = &data,
        .len    = sizeof(data),
    };
    struct spi_message  m;

    write_cmd = int32_msb2lsb(write_cmd);
    addr = int32_msb2lsb(addr);
    data = int32_msb2lsb(data);

    spi_message_init(&m);
    spi_message_add_tail(&write_cmd_packet, &m);
    spi_message_add_tail(&addr_packet, &m);
    spi_message_add_tail(&data_packet, &m);
    return spi_sync(spi, &m);
}

#define SPI_CTRL0   0x11060000
#define SPI_ENR     0x11060008

/**
 * spi2apb_switch_to_msb - SPI2APB set Fist bit mode to MSB
 *
 * @spi: spi device
 * Context: can sleep
 *
 */
void spi2apb_switch_to_msb(struct spi_device *spi)
{
    spi2apb_lsb_w32(spi, SPI_ENR, 0);
    spi2apb_lsb_w32(spi, SPI_CTRL0, 0x108002);
}

/**
 * spi2apb_write - SPI2APB synchronous write
 *
 * @spi: spi device
 * @addr: apb resource address
 * @data: data buffer
 * @data_len: data buffer size, in bytes
 * Context: can sleep
 *
 * It returns zero on success, else a negative error code.
 */
int spi2apb_write(struct spi_device *spi,
        int32_t addr, const int32_t *data, size_t data_len)
{
    int32_t write_cmd = APB_CMD_WRITE;
    struct spi_transfer write_cmd_packet = {
        .tx_buf = &write_cmd,
        .len    = sizeof(write_cmd),
    };
    struct spi_transfer addr_packet = {
        .tx_buf = &addr,
        .len    = sizeof(addr),
    };
    struct spi_transfer data_packet = {
        .tx_buf = data,
        .len    = data_len,
    };
    struct spi_message  m;

    spi_message_init(&m);
    spi_message_add_tail(&write_cmd_packet, &m);
    spi_message_add_tail(&addr_packet, &m);
    spi_message_add_tail(&data_packet, &m);
    return spi_sync(spi, &m);
}

/**
 * spi2apb_w32 - SPI2APB synchronous 32-bit write
 *
 * @spi: spi device
 * @addr: apb resource address
 * @data: 32-bit data
 * Context: can sleep
 *
 * It returns zero on success, else a negative error code.
 */
int spi2apb_w32(struct spi_device *spi,
        int32_t addr, int32_t data)
{
    return spi2apb_write(spi, addr, &data, 4);
}

/**
 * spi2apb_read - SPI2APB synchronous read
 *
 * @spi: spi device
 * @addr: apb resource address
 * @data: data buffer [out]
 * @data_len: data buffer size, in bytes
 * Context: can sleep
 *
 * It returns zero on success, else a negative error code.
 */
int spi2apb_read(struct spi_device *spi,
        int32_t addr, int32_t *data, size_t data_len)
{
    int32_t real_len = MIN(data_len, APB_MAX_OP_BYTES);
    int32_t read_cmd = APB_CMD_READ | (real_len << 14 & 0xffff0000);
    int32_t read_begin_cmd = APB_CMD_READ_BEGIN;
    int32_t dummy = 0;
    struct spi_transfer read_cmd_packet = {
        .tx_buf = &read_cmd,
        .len    = sizeof(read_cmd),
    };
    struct spi_transfer addr_packet = {
        .tx_buf = &addr,
        .len    = sizeof(addr),
    };
    struct spi_transfer read_dummy_packet = {
        .tx_buf = &dummy,
        .len    = sizeof(dummy),
    };
    struct spi_transfer read_begin_cmd_packet = {
        .tx_buf = &read_begin_cmd,
        .len    = sizeof(read_begin_cmd),
    };
    struct spi_transfer data_packet = {
        .rx_buf = data,
        .len    = data_len,
    };
    struct spi_message  m;

    spi_message_init(&m);
    spi_message_add_tail(&read_cmd_packet, &m);
    spi_message_add_tail(&addr_packet, &m);
    spi_message_add_tail(&read_dummy_packet, &m);
    spi_message_add_tail(&read_begin_cmd_packet, &m);
    spi_message_add_tail(&data_packet, &m);
    return spi_sync(spi, &m);
}

/**
 * spi2apb_r32 - SPI2APB synchronous 32-bit read
 *
 * @spi: spi device
 * @addr: apb resource address
 * @data: 32-bit data buffer [out]
 * Context: can sleep
 *
 * It returns zero on success, else a negative error code.
 */
int spi2apb_r32(struct spi_device *spi,
        int32_t addr, int32_t *data)
{
    return spi2apb_read(spi, addr, data, 4);
}

/**
 * spi2apb_operation_query - SPI2APB last operation state query
 *
 * @spi: device from which data will be read
 * @state: last operation state [out]
 * Context: can sleep
 *
 * It returns zero on success, else a negative error code.
 */
int spi2apb_operation_query(struct spi_device *spi, int32_t *state)
{
    int32_t query_cmd = APB_CMD_QUERY;
    struct spi_transfer query_cmd_packet = {
        .tx_buf = &query_cmd,
        .len    = sizeof(query_cmd),
    };
    struct spi_transfer state_packet = {
        .rx_buf = state,
        .len    = sizeof(*state),
    };
    struct spi_message  m;

    spi_message_init(&m);
    spi_message_add_tail(&query_cmd_packet, &m);
    spi_message_add_tail(&state_packet, &m);
    spi_sync(spi, &m);

    return ((*state & APB_OP_STATE_ID_MASK) == APB_OP_STATE_ID) ? 0 : -1;
}

/**
 * spi2apb_state_query - SPI2APB system state query
 *
 * @spi: spi device
 * @state: system state [out]
 * Context: can sleep
 *
 * It returns zero on success, else a negative error code.
 */
int spi2apb_state_query(struct spi_device *spi, int32_t *state)
{
    int32_t query_cmd = APB_CMD_QUERY_REG2;
    struct spi_transfer query_cmd_packet = {
        .tx_buf = &query_cmd,
        .len    = sizeof(query_cmd),
    };
    struct spi_transfer state_packet = {
        .rx_buf = state,
        .len    = sizeof(*state),
    };
    struct spi_message  m;

    spi_message_init(&m);
    spi_message_add_tail(&query_cmd_packet, &m);
    spi_message_add_tail(&state_packet, &m);
    return spi_sync(spi, &m);
}

/**
 * spi2apb_interrupt_request - SPI2APB request a dsp interrupt
 *
 * @spi: spi device
 * @interrupt_num: interrupt identification
 * Context: can sleep
 *
 * It returns zero on success, else a negative error code.
 */
int spi2apb_interrupt_request(struct spi_device *spi,
        int32_t interrupt_num)
{
    int32_t write_reg1_cmd = APB_CMD_WRITE_REG1;
    struct spi_transfer write_reg1_cmd_packet = {
        .tx_buf = &write_reg1_cmd,
        .len    = sizeof(write_reg1_cmd),
    };
    struct spi_transfer reg1_packet = {
        .tx_buf = &interrupt_num,
        .len    = sizeof(interrupt_num),
    };
    struct spi_message  m;

    spi_message_init(&m);
    spi_message_add_tail(&write_reg1_cmd_packet, &m);
    spi_message_add_tail(&reg1_packet, &m);
    return spi_sync(spi, &m);
}

/**
 * _spi2apb_safe_write - SPI2APB synchronous write with state check
 *
 * @spi: spi device
 * @addr: apb resource address
 * @data: data buffer
 * @data_len: data buffer size, in bytes
 * Context: can sleep
 *
 * It returns zero on success, else operation state code.
 */
static int _spi2apb_safe_write(struct spi_device *spi,
        int32_t addr, const int32_t *data, size_t data_len)
{
    int32_t state = 0;
    int32_t try = 0;

    do {
        spi2apb_write(spi, addr, data, data_len);
        if (spi2apb_operation_query(spi, &state) != 0) {
            return -1;
        } else if ((state & APB_OP_STATE_MASK) == 0) {
            break;
        }

        if (try++ == APB_SAFE_OPERATION_TRY_MAX)
            break;
        udelay(APB_SAFE_OPERATION_TRY_DELAY_US);
    } while (1);

    return (state & APB_OP_STATE_MASK);
}

/**
 * spi2apb_safe_write - SPI2APB synchronous write with state check
 *
 * @spi: spi device
 * @addr: apb resource address
 * @data: data buffer
 * @data_len: data buffer size, in bytes
 * Context: can sleep
 *
 * It returns zero on success, else operation state code.
 */
int spi2apb_safe_write(struct spi_device *spi,
        int32_t addr, const int32_t *data, size_t data_len)
{
    int ret = 0;
    while (data_len > 0) {
        size_t slen = MIN(data_len, APB_MAX_OP_BYTES);

        ret = _spi2apb_safe_write(spi, addr, data, slen);
        if (ret) {
            break;
        }

        data_len = data_len - slen;
        data = (int32_t*)((int8_t*)data + slen);
        addr += slen;
    }
    return ret;
}

/**
 * spi2apb_safe_w32 - SPI2APB synchronous 32-bit write with state check
 *
 * @spi: spi device
 * @addr: apb resource address
 * @data: 32-bit data
 * Context: can sleep
 *
 * It returns zero on success, else operation state code.
 */
int spi2apb_safe_w32(struct spi_device *spi,
        int32_t addr, int32_t data)
{
    return spi2apb_safe_write(spi, addr, &data, 4);
}

/**
 * spi2apb_safe_read - SPI2APB synchronous read with state check
 *
 * @spi: spi device
 * @addr: apb resource address
 * @data: data buffer [out]
 * @data_len: data buffer size, in bytes
 * Context: can sleep
 *
 * It returns zero on success, else operation state code.
 */
int spi2apb_safe_read(struct spi_device *spi,
        int32_t addr, int32_t *data, size_t data_len)
{
    int32_t state = 0;
    int32_t try = 0;

    do {
        spi2apb_read(spi, addr, data, data_len);
        if (spi2apb_operation_query(spi, &state) != 0) {
            return -1;
        } else if ((state & APB_OP_STATE_MASK) == 0) {
            break;
        }

        if (try++ == APB_SAFE_OPERATION_TRY_MAX)
            break;
        udelay(APB_SAFE_OPERATION_TRY_DELAY_US);
    } while (1);

    return (state & APB_OP_STATE_MASK);
}

/**
 * spi2apb_safe_r32 - SPI2APB synchronous 32-bit read with state check
 *
 * @spi: spi device
 * @addr: apb resource address
 * @data: 32-bit data buffer [out]
 * Context: can sleep
 *
 * It returns zero on success, else operation state code.
 */
int spi2apb_safe_r32(struct spi_device *spi,
        int32_t addr, int32_t *data)
{
    return spi2apb_safe_read(spi, addr, data, 4);
}
