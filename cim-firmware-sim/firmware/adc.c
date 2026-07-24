#include "adc.h"
#include <math.h>

int32_t adc_convert(float analog_value, uint32_t adc_bits) {
    int32_t levels = (1 << adc_bits) - 1;
    float scale = 1.0f / levels;
    int32_t quantized = (int32_t)roundf(analog_value * levels);
    if (quantized > levels) quantized = levels;
    if (quantized < 0) quantized = 0;
    return quantized;
}

float array_mvm(const float *input, const float *weights, int size) {
    float sum = 0.0f;
    for (int i = 0; i < size; i++) {
        sum += input[i] * weights[i];  // 模拟欧姆定律累加
    }
    return sum;
}
