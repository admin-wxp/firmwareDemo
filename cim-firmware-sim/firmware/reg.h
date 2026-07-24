#ifndef REG_H
#define REG_H

#include <stdint.h>

// 模拟芯片基地址
#define CIM_BASE_ADDR 0x40000000

// 控制寄存器偏移
#define REG_CTRL        0x00
#define REG_ADC_CFG     0x04
#define REG_ARRAY_EN    0x08
#define REG_STATUS      0x0C

// 位域定义（位运算核心）
#define CTRL_EN         (1 << 0)        // 芯片使能
#define CTRL_MODE_MLC   (0 << 1)        // MLC 模式 (2-bit)
#define CTRL_MODE_SLC   (1 << 1)        // SLC 模式 (1-bit)
#define CTRL_MODE_MASK  (3 << 1)        // 模式掩码

#define ADC_BITS_SHIFT  3
#define ADC_BITS_MASK   (0x7 << ADC_BITS_SHIFT)  // ADC 精度: 1-8 bit

#define ARRAY_EN_TILE0  (1 << 0)
#define ARRAY_EN_TILE1  (1 << 1)
#define ARRAY_EN_TILE2  (1 << 2)
#define ARRAY_EN_TILE3  (1 << 3)

#define STATUS_READY    (1 << 0)
#define STATUS_BUSY     (1 << 1)
#define STATUS_ERR      (1 << 2)

// 固件 API
void reg_write(uint32_t offset, uint32_t value);
uint32_t reg_read(uint32_t offset);
void adc_set_mode(uint32_t bits);
void array_enable(uint32_t tile_mask);
void chip_enable(void);
uint32_t chip_wait_ready(void);

#endif