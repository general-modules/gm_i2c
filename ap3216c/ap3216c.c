/**
 * @file      ap3216c.c
 * @brief     AP3216C 驱动源文件
 * @author    huenrong (sgyhy1028@outlook.com)
 * @date      2026-02-16 16:22:02
 *
 * @copyright Copyright (c) 2026 huenrong
 *
 */

#include <stdio.h>

#include "ap3216c.h"
#include "gm_i2c.h"

// AP3216C I2C 设备地址
#define AP3216C_I2C_DEV_NAME   "/dev/i2c-0"
// AP3216C I2C 器件地址
#define AP3216C_I2C_SLAVE_ADDR 0x1E

// AP3316C 寄存器
#define AP3216C_SYSTEM_CONFIG 0x00 // 配置寄存器
#define AP3216C_INT_STATUS    0x01 // 中断状态寄存器
#define AP3216C_INT_CLEAR     0x02 // 中断清除寄存器
#define AP3216C_IR_DATA_LOW   0x0A // IR 数据低字节
#define AP3216C_IR_DATA_HIGH  0x0B // IR 数据高字节
#define AP3216C_ALS_DATA_LOW  0x0C // ALS 数据低字节
#define AP3216C_ALS_DATA_HIGH 0x0D // ALS 数据高字节
#define AP3216C_PS_DATA_LOW   0x0E // PS 数据低字节
#define AP3216C_PS_DATA_HIGH  0x0F // PS 数据高字节

// AP3216C 模式
#define POWER_DOWN                0x00 // 关闭电源
#define ALS_FUNCTION_ACTIVE       0x01 // 激活 ALS 模式
#define PS_IR_FUNCTION_ACTIVE     0x02 // 激活 PS, IR 模式
#define ALS_PS_IR_FUNCTION_ACTIVE 0x03 // 激活 ALS, PS, IR 模式
#define SW_RESET                  0x04 // 复位
#define ALS_FUNCTION_ONCE         0x05 // ALS 模式单次采集
#define PS_IR_FUNCTION_ONCE       0x06 // PS, IR 模式单次采集
#define ALS_PS_IR_FUNCTION_ONCE   0x07 // ALS, PS, IR 模式单次采集

static gm_i2c_t *s_gm_i2c = NULL;

int ap3216c_init(void)
{
    s_gm_i2c = gm_i2c_create();
    if (s_gm_i2c == NULL)
    {
        printf("create gm_i2c failed\n");

        return -1;
    }

    int ret = gm_i2c_init(s_gm_i2c, AP3216C_I2C_DEV_NAME);
    if (ret < 0)
    {
        printf("gm_i2c_init failed. ret: %d\n", ret);

        return -2;
    }

    // 初始化 ap3216c
    // 复位 ap3216c
    uint8_t ap3216c_config = SW_RESET;
    ret = gm_i2c_write_data_sub(s_gm_i2c, AP3216C_I2C_SLAVE_ADDR, AP3216C_SYSTEM_CONFIG, &ap3216c_config, 1);
    if (ret < 0)
    {
        printf("gm_i2c_write_data_sub failed. ret: %d\n", ret);

        return -3;
    }

    // 复位以后至少延时 10ms
    usleep(20 * 1000);

    // 开启 ALS、PS+IR
    ap3216c_config = ALS_PS_IR_FUNCTION_ACTIVE;
    ret = gm_i2c_write_data_sub(s_gm_i2c, AP3216C_I2C_SLAVE_ADDR, AP3216C_SYSTEM_CONFIG, &ap3216c_config, 1);
    if (ret < 0)
    {
        printf("gm_i2c_write_data_sub failed. ret: %d\n", ret);

        return -4;
    }

    // 配置后需要延时一段时间才生效
    usleep(350 * 1000);

    return 0;
}

int ap3216c_read_data(uint16_t *ir_data, uint16_t *als_data, uint16_t *ps_data)
{
    uint8_t read_data[6] = {0};

    for (uint8_t i = 0; i < 6; i++)
    {
        int ret = gm_i2c_read_data_sub(s_gm_i2c, AP3216C_I2C_SLAVE_ADDR, AP3216C_IR_DATA_LOW + i, &read_data[i], 1);
        if (ret < 0)
        {
            printf("gm_i2c_read_data_sub failed. ret: %d, index: %d\n", ret, i);

            return -1;
        }
    }

    // IR_OF 位为1, 数据无效
    if (read_data[0] & 0x80)
    {
        *ir_data = 0;
    }
    // 获取 IR 传感器的数据
    else
    {
        *ir_data = (uint16_t)((read_data[1] << 2) | (read_data[0] & 0x03));
    }

    // 获取 ALS 传感器的数据
    *als_data = (uint16_t)((read_data[3] << 8) | read_data[2]);

    // PS_OF 位为1, 数据无效
    if (read_data[4] & 0x40)
    {
        *ps_data = 0;
    }
    // 获取 PS 传感器的数据
    else
    {
        *ps_data = (uint16_t)(((read_data[5] & 0x3F) << 4) | (read_data[4] & 0x0F));
    }

    return 0;
}
