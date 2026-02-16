/**
 * @file      example_basic.c
 * @brief     AP3216C 示例代码源文件
 * @author    huenrong (sgyhy1028@outlook.com)
 * @date      2026-02-16 16:22:25
 *
 * @copyright Copyright (c) 2026 huenrong
 *
 */

#include <stdio.h>
#include <unistd.h>

#include "ap3216c.h"

/**
 * @brief 程序入口
 *
 * @param[in] argc: 参数个数
 * @param[in] argv: 参数列表
 *
 * @return 成功: 0
 * @return 失败: 其它
 */
int main(int argc, char *argv[])
{
    int ret = ap3216c_init();
    if (ret != 0)
    {
        printf("ap3216c_init failed.\n");

        return -1;
    }

    while (true)
    {
        uint16_t ir_data = 0;
        uint16_t als_data = 0;
        uint16_t ps_data = 0;

        ret = ap3216c_read_data(&ir_data, &als_data, &ps_data);
        if (ret != 0)
        {
            printf("read ap3216c data failed\n");

            return -1;
        }

        printf("current ir: %d, current als: %d, current ps: %d\n", ir_data, als_data, ps_data);

        sleep(1);
    }

    return 0;
}
