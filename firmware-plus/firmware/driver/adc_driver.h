/**
 * @file adc_driver.h
 * @brief ADC多通道驱动接口
 * @description 支持 4 通道轮询、可配置精度（1/2/4/8 bit）、温度补偿。
 *              使用状态机管理 ADC 生命周期，避免在转换过程中被误配置。
 */

 #ifndef ADC_DRIVER_H
 #define ADC_DRIVER_H
 #include <stdint.h>

 /* ADC 工作模式（精度） */
 typedef enum{
    ADC_MODE_1BIT = 1,
    ADC_MODE_2BIT = 2,
    ADC_MODE_4BIT = 4,
    ADC_MODE_8BIT = 8,
 } adc_mode_t;

 /* ADC 通道 */
 typedef enum {
    ADC_CH0 = 0,
    ADC_CH1,
    ADC_CH2,
    ADC_CH3,
    ADC_CH_MAX
 } adc_channel_t;

 /* ADC 状态机状态 */ 
 typedef enum {
    ADC_STATE_IDLE = 0,
    ADC_STATE_CONFIG,
    ADC_STATE_SAMPLING,
    ADC_STATE_DONE,
    ADC_STATE_ERROR
 } adc_state_t;

 /* 初始化 */
 void adc_init(void);

 /* 配置通道参数 */
 void adc_config_channel(adc_channel_t ch, adc_mode_t mode, int enable_temp_comp);

 /* 启动指定通道采样 */
 void adc_start_channel(adc_channel_t ch);

 /* 轮询等待转换完成（非阻塞检查） */
 int adc_poll_done(adc_channel_t ch);

 /* 读取通道结果（必须在DONE后调用） */
 float adc_read_channel(adc_channel_t ch);

 /* 获取当前状态 */
 adc_state_t adc_get_state(void);

 /* 获取最近一次转换的原始信息（用于调试） */
 void adc_get_last_info(adc_channel_t *ch, uint32_t *raw, float *temp);

 #endif