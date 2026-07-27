/**
 * @file hal_reg.c
 * @brief 硬件抽象层 - 寄存器访问实现
 * @description 提供带边界检查的 MMIO 读写。真实硬件中，非法地址访问会触发 Bus Fault；
 *              这里通过软件检查模拟该保护机制。
 */

 #include "hal_reg.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>

 /* 模拟4KB寄存器空间 */
 uint32_t g_reg_space[REG_COUNT];

 /* 错误标记：非法访问时置位，供 Python ctypes 读取 */
 static int g_hal_error = 0;

 int hal_reg_had_error(void)  { return g_hal_error; }
 void hal_reg_clear_error(void) { g_hal_error = 0; }

 uint32_t hal_reg_read(uint32_t offset){
    uint32_t idx = offset / 4;
    if (idx >= REG_COUNT)
    {
        fprintf(stderr, "[HAL ERROR] Illegal register read at offset 0x%08X\n", offset);
        g_hal_error = 1;
        return 0xDEADBEEFU;
    }
    return g_reg_space[idx];
 }

 void hal_reg_write(uint32_t offset, uint32_t value){
    uint32_t idx = offset / 4;
    if(idx >= REG_COUNT){
        fprintf(stderr, "[HAL ERROR] Illegal register write at offset 0x%08X\n", offset);
        g_hal_error = 1;
        return;
    }
    g_reg_space[idx] = value;
 }

 void hal_reg_set_bits(uint32_t offset, uint32_t bits){
    uint32_t val = hal_reg_read(offset);
    val |= bits;
    hal_reg_write(offset, val);
 }

 void hal_reg_clear_bits(uint32_t offset, uint32_t bits){
    uint32_t val = hal_reg_read(offset);
    val &= ~bits;
    hal_reg_write(offset, val);
 }


