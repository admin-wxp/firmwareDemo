#include <stdio.h>
#include "chip_reg.h"
#include "adc_ctrl.h"

int main(void){
    printf("=== Virtual CIM Chip Firmware Boot ===\n");
    /* 1.初始化 */
    adc_init();

    /* 2.配置ADC为4-bit模式 */
    adc_set_mode(ADC_MODE_4BIT_VAL);

    /* 3.使能温度补偿 */
    uint32_t ctrl = REG_READ(REG_ADC_CTRL);
    ctrl |= ADC_TEMP_COMP_EN;
    REG_WRITE(REG_ADC_CTRL, ctrl);

    /* 4.使能ADC */
    adc_enable();

    /* 5.模拟一次转换：输入0.5V，温度85℃ */
    float input = 0.5f;
    float temp = 85.0f;
    adc_start_convert(input, temp);

    if(adc_wait_ready(100) == 0){
        float res = adc_read_data();
        printf("Input: %.3fV, Temp: %.1fC, ADC Result: %.3fV\n", input, temp, res);
        printf("ADC Mode: %ubit, TempComp: %s\n", 
               (unsigned)adc_get_mode_bits(),
               (ctrl & ADC_TEMP_COMP_EN) ? "ON" : "OFF");
    }

    printf("=== Boot Complete ===\n");

    return 0;
}