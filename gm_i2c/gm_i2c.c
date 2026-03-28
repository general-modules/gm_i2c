/**
 * @file      gm_i2c.c
 * @brief     I2C 模块源文件
 * @author    huenrong (sgyhy1028@outlook.com)
 * @date      2026-02-16 16:21:45
 *
 * @copyright Copyright (c) 2026 huenrong
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "gm_i2c.h"

// I2C 对象
struct _gm_i2c_t
{
    int fd;
    pthread_mutex_t mutex;
};

gm_i2c_t *gm_i2c_create(void)
{
    gm_i2c_t *gm_i2c = (gm_i2c_t *)malloc(sizeof(gm_i2c_t));
    if (gm_i2c == NULL)
    {
        return NULL;
    }

    gm_i2c->fd = -1;
    pthread_mutex_init(&gm_i2c->mutex, NULL);

    return gm_i2c;
}

int gm_i2c_init(gm_i2c_t *gm_i2c, const char *i2c_name)
{
    if (gm_i2c == NULL)
    {
        return -1;
    }

    if (i2c_name == NULL)
    {
        return -2;
    }

    pthread_mutex_lock(&gm_i2c->mutex);
    if (gm_i2c->fd != -1)
    {
        close(gm_i2c->fd);
        gm_i2c->fd = -1;
    }

    int fd = open(i2c_name, O_RDWR);
    if (fd < 0)
    {
        pthread_mutex_unlock(&gm_i2c->mutex);

        return -3;
    }

    gm_i2c->fd = fd;
    pthread_mutex_unlock(&gm_i2c->mutex);

    return 0;
}

int gm_i2c_destroy(gm_i2c_t *gm_i2c)
{
    if (gm_i2c == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&gm_i2c->mutex);
    if (gm_i2c->fd != -1)
    {
        close(gm_i2c->fd);
    }

    pthread_mutex_unlock(&gm_i2c->mutex);
    pthread_mutex_destroy(&gm_i2c->mutex);
    free(gm_i2c);

    return 0;
}

int gm_i2c_write_data(gm_i2c_t *gm_i2c, const uint16_t slave_addr, const uint8_t *data, const size_t len)
{
    if (gm_i2c == NULL)
    {
        return -1;
    }

    if (slave_addr > 0x7F)
    {
        return -2;
    }

    if (data == NULL)
    {
        return -3;
    }

    if (len == 0)
    {
        return -4;
    }

    pthread_mutex_lock(&gm_i2c->mutex);
    if (gm_i2c->fd < 0)
    {
        pthread_mutex_unlock(&gm_i2c->mutex);

        return -5;
    }

    struct i2c_msg msg = {0};
    msg.addr = slave_addr;
    msg.flags = 0; // 0: write
    msg.len = len;
    msg.buf = (uint8_t *)data;

    struct i2c_rdwr_ioctl_data ioctl_data = {0};
    ioctl_data.msgs = &msg;
    ioctl_data.nmsgs = 1;

    if (ioctl(gm_i2c->fd, I2C_RDWR, &ioctl_data) < 0)
    {
        pthread_mutex_unlock(&gm_i2c->mutex);

        return -6;
    }

    pthread_mutex_unlock(&gm_i2c->mutex);

    return 0;
}

int gm_i2c_read_data(gm_i2c_t *gm_i2c, const uint16_t slave_addr, uint8_t *data, const size_t len)
{
    if (gm_i2c == NULL)
    {
        return -1;
    }

    if (slave_addr > 0x7F)
    {
        return -2;
    }

    if (data == NULL)
    {
        return -3;
    }

    if (len == 0)
    {
        return -4;
    }

    pthread_mutex_lock(&gm_i2c->mutex);
    if (gm_i2c->fd < 0)
    {
        pthread_mutex_unlock(&gm_i2c->mutex);

        return -5;
    }

    // 从 I2C 从设备读取数据
    struct i2c_msg msg = {0};
    msg.addr = slave_addr;
    msg.flags = I2C_M_RD;
    msg.len = len;
    msg.buf = (uint8_t *)data;

    struct i2c_rdwr_ioctl_data ioctl_data = {0};
    ioctl_data.msgs = &msg;
    ioctl_data.nmsgs = 1;

    if (ioctl(gm_i2c->fd, I2C_RDWR, &ioctl_data) < 0)
    {
        pthread_mutex_unlock(&gm_i2c->mutex);

        return -6;
    }

    pthread_mutex_unlock(&gm_i2c->mutex);

    return 0;
}

int gm_i2c_write_data_sub(gm_i2c_t *gm_i2c, const uint16_t slave_addr, const uint8_t reg_addr, const uint8_t *data,
                          const size_t len)

{
    if (gm_i2c == NULL)
    {
        return -1;
    }

    if (slave_addr > 0x7F)
    {
        return -2;
    }

    if (data == NULL)
    {
        return -3;
    }

    if (len == 0)
    {
        return -4;
    }

    pthread_mutex_lock(&gm_i2c->mutex);
    if (gm_i2c->fd < 0)
    {
        pthread_mutex_unlock(&gm_i2c->mutex);

        return -5;
    }

    uint8_t reg = reg_addr;
    struct i2c_msg msgs[2] = {0};

    msgs[0].addr = slave_addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = slave_addr;
    msgs[1].flags = 0;
    msgs[1].len = len;
    msgs[1].buf = (uint8_t *)data;

    struct i2c_rdwr_ioctl_data ioctl_data = {0};
    ioctl_data.msgs = msgs;
    ioctl_data.nmsgs = 2;

    if (ioctl(gm_i2c->fd, I2C_RDWR, &ioctl_data) < 0)
    {
        pthread_mutex_unlock(&gm_i2c->mutex);

        return -6;
    }

    pthread_mutex_unlock(&gm_i2c->mutex);

    return 0;
}

int gm_i2c_read_data_sub(gm_i2c_t *gm_i2c, const uint16_t slave_addr, const uint8_t reg_addr, uint8_t *data,
                         const size_t len)
{
    if (gm_i2c == NULL)
    {
        return -1;
    }

    if (slave_addr > 0x7F)
    {
        return -2;
    }

    if (data == NULL)
    {
        return -3;
    }

    if (len == 0)
    {
        return -4;
    }

    pthread_mutex_lock(&gm_i2c->mutex);
    if (gm_i2c->fd < 0)
    {
        pthread_mutex_unlock(&gm_i2c->mutex);

        return -5;
    }

    uint8_t reg = reg_addr;
    struct i2c_msg msgs[2] = {0};

    msgs[0].addr = slave_addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = slave_addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = len;
    msgs[1].buf = data;

    struct i2c_rdwr_ioctl_data ioctl_data = {0};
    ioctl_data.msgs = msgs;
    ioctl_data.nmsgs = 2;

    if (ioctl(gm_i2c->fd, I2C_RDWR, &ioctl_data) < 0)
    {
        pthread_mutex_unlock(&gm_i2c->mutex);

        return -6;
    }

    pthread_mutex_unlock(&gm_i2c->mutex);

    return 0;
}
