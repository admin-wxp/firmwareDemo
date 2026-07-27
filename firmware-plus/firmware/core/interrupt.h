/**
 * @file interrupt.h
 * @brief 中断管理
 * @description 模拟 NVIC 行为：支持 ISR 注册、中断使能/禁用、软件触发。
 *              固件最佳实践：ISR 里只做标记（置 flag），主循环处理实际工作。
 */
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

#define MAX_ISR_COUNT   32

typedef void (*isr_handler_t)(void);

/* 注册中断服务函数 */
void int_register(uint32_t irq_num, isr_handler_t handler);

/* 使能/禁用特定中断 */
void int_enable(uint32_t irq_num);
void int_disable(uint32_t irq_num);

/* 触发中断（模拟硬件中断） */
void int_trigger(uint32_t irq_num);

/* 主循环调用：分发待处理中断 */
void int_dispatch(void);

/* 获取当前挂起的中断位图 */
uint32_t int_get_pending(void);

#endif /* INTERRUPT_H */