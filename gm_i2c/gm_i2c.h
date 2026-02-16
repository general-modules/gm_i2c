/**
 * @file      gm_i2c.h
 * @brief     I2C 模块头文件
 * @author    huenrong (sgyhy1028@outlook.com)
 * @date      2026-02-16 16:21:52
 *
 * @copyright Copyright (c) 2026 huenrong
 *
 */

#ifndef __GM_I2C_H
#define __GM_I2C_H

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define GM_I2C_VERSION_MAJOR 1
#define GM_I2C_VERSION_MINOR 0
#define GM_I2C_VERSION_PATCH 0

// I2C 对象
typedef struct _gm_i2c gm_i2c_t;

/**
 * @brief 创建 I2C 对象
 *
 * @return 成功: I2C 对象
 * @return 失败: NULL
 */
gm_i2c_t *gm_i2c_create(void);

/**
 * @brief 初始化 I2C 对象
 *
 * @note 1. 该函数支持重复调用，重复调用时会关闭并重新打开 I2C 设备
 *       2. 调用该函数前必须确保没有其他线程正在使用该 I2C 对象，否则可能导致未定义行为
 *       3. I2C 总线速率由内核/硬件配置决定，用户态无法修改
 *
 * @param[in,out] gm_i2c  : I2C 对象
 * @param[in]     i2c_name: I2C 设备名称（如 /dev/i2c-0）
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int gm_i2c_init(gm_i2c_t *gm_i2c, const char *i2c_name);

/**
 * @brief 销毁 I2C 对象
 *
 * @note 1. 调用该函数前必须确保没有其他线程正在使用该 I2C 对象，否则可能导致未定义行为
 *       2. 销毁后，该 I2C 对象将不再可用
 *
 * @param[in,out] gm_i2c: I2C 对象
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int gm_i2c_destroy(gm_i2c_t *gm_i2c);

/**
 * @brief 向无寄存器地址的 I2C 设备写入数据/命令
 *
 * @note 仅支持 7-bit 从设备地址
 *
 * @param[in,out] gm_i2c    : I2C 对象
 * @param[in]     slave_addr: 从设备地址
 * @param[in]     data      : 待写入的数据/命令
 * @param[in]     len       : 待写入的数据/命令长度
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int gm_i2c_write_data(gm_i2c_t *gm_i2c, const uint16_t slave_addr, const uint8_t *data, const size_t len);

/**
 * @brief 从无寄存器地址的 I2C 设备读取数据
 *
 * @note 仅支持 7-bit 从设备地址
 *
 * @param[in,out] gm_i2c    : I2C 对象
 * @param[in]     slave_addr: 从设备地址
 * @param[out]    data      : 读取到的数据
 * @param[in]     len       : 指定读取数据的长度
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int gm_i2c_read_data(gm_i2c_t *gm_i2c, const uint16_t slave_addr, uint8_t *data, const size_t len);

/**
 * @brief 向有寄存器地址的 I2C 设备写入数据/命令，即写 I2C 从设备寄存器
 *
 * @note 仅支持 7-bit 从设备地址
 *
 * @param[in,out] gm_i2c    : I2C 对象
 * @param[in]     slave_addr: 从设备地址
 * @param[in]     reg_addr  : 从设备寄存器地址
 * @param[in]     data      : 待写入的数据/命令
 * @param[in]     len       : 待写入的数据/命令长度
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int gm_i2c_write_data_sub(gm_i2c_t *gm_i2c, const uint16_t slave_addr, const uint8_t reg_addr, const uint8_t *data,
                          const size_t len);

/**
 * @brief 从有寄存器地址的 I2C 设备读取数据，即读 I2C 从设备寄存器
 *
 * @note 仅支持 7-bit 从设备地址
 *
 * @param[in,out] gm_i2c    : I2C 对象
 * @param[in]     slave_addr: 从设备地址
 * @param[in]     reg_addr  : 从设备寄存器地址
 * @param[out]    data      : 读取到的数据
 * @param[in]     len       : 指定读取数据的长度
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int gm_i2c_read_data_sub(gm_i2c_t *gm_i2c, const uint16_t slave_addr, const uint8_t reg_addr, uint8_t *data,
                         const size_t len);

#ifdef __cplusplus
}
#endif

#endif // __GM_I2C_H
