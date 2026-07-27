/**
 * @file interrupt.c
 * @brief 中断分发实现
 * @description 模拟硬件中断控制器。关键设计：ISR 中禁止睡眠和复杂计算，
 *              只设置标志位，由主循环轮询处理。
 */
#include "interrupt.h"
#include "hal_reg.h"
#include <stdio.h>

static isr_handler_t s_isr_table[MAX_ISR_COUNT];
static uint32_t s_enabled_mask = 0;

void int_register(uint32_t irq_num, isr_handler_t handler)
{
    if (irq_num < MAX_ISR_COUNT) s_isr_table[irq_num] = handler;
}

void int_enable(uint32_t irq_num)
{
    if (irq_num < MAX_ISR_COUNT) s_enabled_mask |= (1U << irq_num);
}

void int_disable(uint32_t irq_num)
{
    if (irq_num < MAX_ISR_COUNT) s_enabled_mask &= ~(1U << irq_num);
}

void int_trigger(uint32_t irq_num)
{
    if (irq_num >= MAX_ISR_COUNT) return;
    if (!(s_enabled_mask & (1U << irq_num))) return; /* 未使能，忽略 */
    
    /* 模拟硬件：设置挂起寄存器 */
    hal_reg_set_bits(REG_OFFSET_INT_PENDING, (1U << irq_num));
    
    /* 立即分发（简化版，真实硬件有中断延迟） */
    if (s_isr_table[irq_num]) s_isr_table[irq_num]();
}

void int_dispatch(void)
{
    /* 主循环调用：处理所有挂起中断 */
    uint32_t pending = hal_reg_read(REG_OFFSET_INT_PENDING);
    uint32_t i;
    for (i = 0; i < MAX_ISR_COUNT; i++) {
        if (pending & (1U << i)) {
            /* 清除挂起位 */
            hal_reg_clear_bits(REG_OFFSET_INT_PENDING, (1U << i));
        }
    }
}

uint32_t int_get_pending(void)
{
    return hal_reg_read(REG_OFFSET_INT_PENDING);
}