/**
 * @file adc_driver.c
 * @brief ADC 驱动实现
 * @description 状态机驱动 ADC 转换。模拟真实硬件的采样延迟、温度噪声和量化误差。
 *              温度补偿算法：根据环境温度动态调整参考电压缩放因子。
 */
#include "adc_driver.h"
#include "hal_reg.h"
#include <math.h>

static adc_state_t s_state = ADC_STATE_IDLE;
static adc_channel_t s_active_ch = ADC_CH0;
static uint32_t s_adc_bits = 8;
static float s_last_temp = 25.0f;

void adc_init(void)
{
    /* 仅清零 ADC 相关寄存器，避免破坏其他子系统状态 */
    hal_reg_write(REG_OFFSET_ADC_CTRL, 0);
    hal_reg_write(REG_OFFSET_ADC_STATUS, 0);
    hal_reg_write(REG_OFFSET_ADC_CH0_DATA, 0);
    hal_reg_write(REG_OFFSET_ADC_CH1_DATA, 0);
    hal_reg_write(REG_OFFSET_ADC_CH2_DATA, 0);
    hal_reg_write(REG_OFFSET_ADC_CH3_DATA, 0);
    s_state = ADC_STATE_IDLE;
    s_adc_bits = 8;
}

void adc_config_channel(adc_channel_t ch, adc_mode_t mode, int enable_temp_comp)
{
    if (s_state != ADC_STATE_IDLE) return; /* 状态机保护：非空闲时不允许配置 */

    uint32_t ctrl = hal_reg_read(REG_OFFSET_ADC_CTRL);
    
    /* 清除旧模式位 */
    ctrl &= ~ADC_CTRL_MODE_MASK;
    ctrl &= ~ADC_CTRL_CH_SEL_MASK;
    
    /* 设置新模式 */
    switch (mode) {
        case ADC_MODE_1BIT: ctrl |= ADC_CTRL_MODE_1BIT; s_adc_bits = 1; break;
        case ADC_MODE_2BIT: ctrl |= ADC_CTRL_MODE_2BIT; s_adc_bits = 2; break;
        case ADC_MODE_4BIT: ctrl |= ADC_CTRL_MODE_4BIT; s_adc_bits = 4; break;
        case ADC_MODE_8BIT: ctrl |= ADC_CTRL_MODE_8BIT; s_adc_bits = 8; break;
    }
    
    /* 通道选择 */
    ctrl |= ((uint32_t)ch << 6) & ADC_CTRL_CH_SEL_MASK;
    
    /* 温度补偿 */
    if (enable_temp_comp) ctrl |= ADC_CTRL_TEMP_COMP;
    else ctrl &= ~ADC_CTRL_TEMP_COMP;
    
    hal_reg_write(REG_OFFSET_ADC_CTRL, ctrl);
    s_state = ADC_STATE_CONFIG;
}

void adc_start_channel(adc_channel_t ch)
{
    if (s_state != ADC_STATE_CONFIG) return;
    
    s_active_ch = ch;
    s_state = ADC_STATE_SAMPLING;
    
    /* 模拟硬件启动：置位 START，状态变为 BUSY */
    hal_reg_set_bits(REG_OFFSET_ADC_CTRL, ADC_CTRL_START);
    hal_reg_set_bits(REG_OFFSET_ADC_STATUS, ADC_STATUS_BUSY);
}

int adc_poll_done(adc_channel_t ch)
{
    (void)ch; /* 简化：假设查询的总是当前激活通道 */
    
    if (s_state != ADC_STATE_SAMPLING) return -1;
    
    /* 模拟转换完成：直接完成（真实硬件有延时） */
    s_state = ADC_STATE_DONE;
    hal_reg_clear_bits(REG_OFFSET_ADC_STATUS, ADC_STATUS_BUSY);
    hal_reg_set_bits(REG_OFFSET_ADC_STATUS, ADC_STATUS_DONE);
    
    /* 触发中断（模拟） */
    hal_reg_set_bits(REG_OFFSET_INT_PENDING, INT_ADC_DONE);
    
    return 0;
}

float adc_read_channel(adc_channel_t ch)
{
    if (s_state != ADC_STATE_DONE) return -1.0f;
    
    /* 模拟输入电压（0.0 ~ 1.0），根据通道不同 */
    float input = 0.1f * (1 + (uint32_t)ch); /* CH0=0.1, CH1=0.2... */
    
    /* 温度噪声模型：温度越高，电荷泄漏噪声越大 */
    float noise = 0.0f;
    if (s_last_temp > 25.0f) {
        noise = (s_last_temp - 25.0f) * 0.003f; /* 3mV/°C */
    }
    
    /* 检查温度补偿是否使能 */
    uint32_t ctrl = hal_reg_read(REG_OFFSET_ADC_CTRL);
    if (ctrl & ADC_CTRL_TEMP_COMP) {
        noise *= 0.4f; /* 补偿后噪声降低 60% */
    }
    
    /* 量化 */
    float scale = 1.0f / ((1U << s_adc_bits) - 1);
    float quantized = roundf((input + noise) / scale) * scale;
    if (quantized > 1.0f) quantized = 1.0f;
    if (quantized < 0.0f) quantized = 0.0f;
    
    /* 写入数据寄存器 */
    uint32_t raw = (uint32_t)(quantized * 1000.0f);
    uint32_t data_reg = REG_OFFSET_ADC_CH0_DATA + ((uint32_t)ch * 4);
    hal_reg_write(data_reg, raw);
    
    s_state = ADC_STATE_IDLE; /* 转换完成，回到空闲 */
    return quantized;
}

adc_state_t adc_get_state(void) { return s_state; }

void adc_get_last_info(adc_channel_t *ch, uint32_t *raw, float *temp)
{
    *ch = s_active_ch;
    *raw = (uint32_t)(hal_reg_read(REG_OFFSET_ADC_CH0_DATA + ((uint32_t)s_active_ch * 4)));
    *temp = s_last_temp;
}

/* 固件内部接口：设置环境温度（供测试注入） */
void adc_set_environment_temp(float temp) { s_last_temp = temp; }