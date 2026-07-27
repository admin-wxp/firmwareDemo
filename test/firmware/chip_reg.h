#ifndef CHIP_REG_H
#define CHIP_REG_H

#include <stdint.h>

/*
 * 虚拟芯片寄存器的基地址（模拟MMIO）
 * 真实的硬件中，这些地址由芯片手册定义
 */

 #define REG_BASE 0x40000000U

 /* 寄存器偏移 */
#define REG_ADC_CTRL    0x00 /* ADC 控制寄存器 */
#define REG_ADC_STATUS  0x04 /* ADC 状态寄存器 */
#define REG_ADC_DATA    0x08 /* ADC 数据寄存器 */
#define REG_ARRAY_EN    0x10 /* 阵列使能寄存器 */

/*
 * ADC_CTRL 位域定义
 * bit[0]:   ADC 使能
 * bit[3:1]: ADC 精度模式 (000=1bit, 001=2bit, 010=4bit, 011=8bit)
 * bit[4]:   温度补偿使能
 */
#define ADC_EN_BIT      (1U << 0)
#define ADC_MODE_MASK   (0x7U << 1)
#define ADC_MODE_1BIT   (0x0U << 1)
#define ADC_MODE_2BIT   (0x1U << 1)
#define ADC_MODE_4BIT   (0x2U << 1)
#define ADC_MODE_8BIT   (0x3U << 1)
#define ADC_TEMP_COMP_EN    (1U << 4)

/* ADC_STATUS 位域*/
#define ADC_STATUS_READY    (1U << 0)   /* 转换完成 */
#define ADC_STATUS_ERROR    (1U << 1)   /* 转换错误 */

/* 模拟寄存器内存空间（真实硬件中是物理寄存器） */
extern uint32_t g_reg_space[16];

/* 辅助宏：读写寄存器（模拟MMIO） */
#define REG_READ(offset)        (g_reg_space[(offset) / 4])
#define REG_WRITE(offset, val)  (g_reg_space[(offset) / 4] = (val))

#endif /* CHIP_REG_H */
