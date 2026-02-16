/**
 * @file      ap3216c.h
 * @brief     AP3216C 驱动头文件
 * @author    huenrong (sgyhy1028@outlook.com)
 * @date      2026-02-16 16:22:09
 *
 * @copyright Copyright (c) 2026 huenrong
 *
 */

#ifndef __AP3216C_H
#define __AP3216C_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief  初始化 AP3216C
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int ap3216c_init(void);

/**
 * @brief 读取 AP3216C 数据
 *
 * @param[out] ir_data : 读取到的 IR 数据
 * @param[out] als_data: 读取到的 ALS 数据
 * @param[out] ps_data : 读取到的 PS 数据
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int ap3216c_read_data(uint16_t *ir_data, uint16_t *als_data, uint16_t *ps_data);

#ifdef __cplusplus
}
#endif

#endif // __AP3216C_H
