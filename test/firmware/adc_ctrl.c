#include "adc_ctrl.h"
#include "chip_reg.h"
#include <math.h>
#include <string.h>

/* 模拟芯片寄存器内存（4KB 空间） */
uint32_t g_reg_space[16];

/* 内部状态 */
static uint32_t s_adc_bits = 8;

void adc_init(void){
    memset(g_reg_space, 0, sizeof(g_reg_space));
    s_adc_bits = 8;
}

void adc_set_mode(adc_mode_t mode){
    uint32_t ctrl = REG_READ(REG_ADC_CTRL);

    /* 清除模式位，再设置新值（标准位运算：先清后置） */
    ctrl &= ~ADC_MODE_MASK;

    switch (mode)
    {
        case ADC_MODE_1BIT_VAL: ctrl |= ADC_MODE_1BIT; s_adc_bits = 1; break;
        case ADC_MODE_2BIT_VAL: ctrl |= ADC_MODE_2BIT; s_adc_bits = 2; break;
        case ADC_MODE_4BIT_VAL: ctrl |= ADC_MODE_4BIT; s_adc_bits = 4; break;
        case ADC_MODE_8BIT_VAL: ctrl |= ADC_MODE_8BIT; s_adc_bits = 8; break;
    }

    REG_WRITE(REG_ADC_CTRL, ctrl);
}

void adc_disable(void){
    uint32_t ctrl = REG_READ(REG_ADC_CTRL);
    ctrl &= ~ADC_EN_BIT;    /* 清零禁用 */
    REG_WRITE(REG_ADC_CTRL, ctrl);
}

void adc_enable(void){
    uint32_t ctrl = REG_READ(REG_ADC_CTRL);
    ctrl |= ADC_EN_BIT; /* 置位使能 */
    REG_WRITE(REG_ADC_CTRL, ctrl);
}

void adc_start_convert(float input_voltage, float temperature){
    /* 模拟 ADC 转换过程：
     * 1.根据精度确定量化台阶
     * 2.加入温度相关噪声（模拟 NOR Flash 电荷泄漏）
     * 3.写入数据寄存器，置位 READY
     */
    uint32_t ctrl = REG_READ(REG_ADC_CTRL);
    int use_temp_comp = (ctrl & ADC_TEMP_COMP_EN) ? 1 : 0;

    /* 温度噪声模型：温度越高，噪声越大 */
    float noise = 0.0f;
    if(temperature > 25.0f){
        noise = (temperature - 25.0f) * 0.002f; /* 每度 2mV 噪声 */
    }

    /* 如果有温度补偿，抵消部分噪声 */
    if (use_temp_comp)
    {
        noise *= 0.5f;
    }

    /* 量化：假设输入范围是0~1V，s_adc_bits 级台阶 */
    float scale = 1.0f / ((1U << s_adc_bits) - 1);
    float quantized = roundf((input_voltage + noise) / scale) * scale;
    
    /* 钳制到有效范围 */
    if (quantized > 1.0f)
    {
        quantized = 1.0f;
    }
    if (quantized < 0.0f)
    {
        quantized = 0.0f;
    }

    /* 写入数据寄存器（模拟把 float 按 1/1000 缩放存 uint32） */
    uint32_t data_raw = (uint32_t)(quantized * 1000.0f);
    REG_WRITE(REG_ADC_DATA, data_raw);
    
    /* 置位 READY */
    uint32_t status = REG_READ(REG_ADC_STATUS);
    status |= ADC_STATUS_READY;
    REG_WRITE(REG_ADC_STATUS, status);
}

int adc_wait_ready(uint32_t timeout_ms){
    /* 模拟轮询：真实固件中会用延时或中断 */
    (void)timeout_ms; /* 简化：立即完成 */
    uint32_t status = REG_READ(REG_ADC_STATUS);
    return (status & ADC_STATUS_READY) ? 0 : -1;
}

float adc_read_data(void){
    uint32_t raw = REG_READ(REG_ADC_DATA);
    return (float)raw / 1000.0f;
}

uint32_t adc_get_mode_bits(void){
    return s_adc_bits;
}

