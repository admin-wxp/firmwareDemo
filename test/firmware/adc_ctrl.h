#ifndef ADC_CTRL_H
#define ADC_CTRL_H

#include <stdint.h>

/* ADC 工作模式 */
typedef enum {
    ADC_MODE_1BIT_VAL = 1,
    ADC_MODE_2BIT_VAL = 2,
    ADC_MODE_4BIT_VAL = 4,
    ADC_MODE_8BIT_VAL = 8
} adc_mode_t;

/* 初始化 ADC 控制器 */
void adc_init(void);

/* 配置 ADC 模式（使用位运算） */
void adc_set_mode(adc_mode_t mode);

/* 使能/禁用 ADC */
void adc_enable(void);
void adc_disable(void);

/* 启动一次转换（模拟） */
void adc_start_convert(float input_voltage, float temperature); 

/* 等待转换完成（轮询状态寄存器） */
int adc_wait_ready(uint32_t timeout_ms);

/* 读取转换结果 */
float adc_read_data(void);

/* 获取当前配置的 ADC 位数 */
uint32_t adc_get_mode_bits(void);

#endif /* ADC_CTRL_H */