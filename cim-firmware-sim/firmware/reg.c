#include "reg.h"
#include <stdio.h>

// 模拟寄存器内存（MMIO 的简化版）
static uint32_t reg_mem[16] = {0};

void reg_write(uint32_t offset, uint32_t value) {
    if (offset < 64) {
        reg_mem[offset / 4] = value;
        printf("[FW] Write REG[0x%02X] = 0x%08X\n", offset, value);
    }
}

uint32_t reg_read(uint32_t offset) {
    if (offset < 64) {
        uint32_t val = reg_mem[offset / 4];
        printf("[FW] Read  REG[0x%02X] = 0x%08X\n", offset, val);
        return val;
    }
    return 0;
}

// 位运算配置：设置 ADC 精度（清掉旧位，写入新位）
void adc_set_mode(uint32_t bits) {
    uint32_t ctrl = reg_read(REG_CTRL);
    ctrl &= ~ADC_BITS_MASK;                    // 清除 bit[5:3]
    ctrl |= ((bits & 0x7) << ADC_BITS_SHIFT);  // 设置新值
    reg_write(REG_CTRL, ctrl);
}

// 位运算配置：使能指定 Tile
void array_enable(uint32_t tile_mask) {
    reg_write(REG_ARRAY_EN, tile_mask);
}

void chip_enable(void) {
    uint32_t ctrl = reg_read(REG_CTRL);
    ctrl |= CTRL_EN;  // 置位使能
    reg_write(REG_CTRL, ctrl);
}

// 模拟轮询等待（固件常用模式）
uint32_t chip_wait_ready(void) {
    int timeout = 1000;
    while (timeout--) {
        uint32_t status = reg_read(REG_STATUS);
        if (status & STATUS_READY) return 0;
        if (status & STATUS_ERR) return 1;
    }
    return 2; // timeout
}