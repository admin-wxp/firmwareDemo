#ifndef ADC_H
#define ADC_H

#include <stdint.h>

// 模拟 ADC 转换：输入模拟值，根据精度量化
int32_t adc_convert(float analog_value, uint32_t adc_bits);

// 模拟 MVM 计算（阵列乘加）
float array_mvm(const float *input, const float *weights, int size);

#endif