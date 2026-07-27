/**
 * @file main.c
 * @brief 芯片 Bring-up 主程序
 * @description 模拟真实芯片上电流程：
 *              1. BIST（内置自检）：扫描坏块
 *              2. 解锁 Flash 控制器
 *              3. ADC 校准（采样参考电压）
 *              4. 进入主循环，等待中断
 */
#include <stdio.h>
#include <string.h>
#include "hal_reg.h"
#include "adc_driver.h"
#include "flash_ctrl.h"
#include "interrupt.h"

/* 中断服务程序：ADC 完成 */
static volatile int g_adc_done_flag = 0;
void isr_adc_done(void) { g_adc_done_flag = 1; }

int main(void)
{
    printf("=== CIM Chip Bring-up Sequence ===\n");
    
    /* Step 1: 初始化 HAL（清零寄存器） */
    memset(g_reg_space, 0, sizeof(g_reg_space));
    
    /* Step 2: BIST - 扫描 Flash 坏块 */
    flash_init();
    const uint8_t* bad_blocks = flash_get_bad_block_table();
    int bad_count = 0;
    for (int i = 0; i < 16; i++) if (bad_blocks[i]) bad_count++;
    printf("[BIST] Bad blocks found: %d\n", bad_count);
    if (bad_count == 0) {
        hal_reg_set_bits(REG_OFFSET_STATUS, STATUS_BIT_BIST_PASS);
    }
    
    /* Step 3: 解锁 Flash */
    hal_reg_write(REG_OFFSET_FLASH_CMD, FLASH_CMD_UNLOCK);
    if (flash_unlock() == 0) {
        printf("[FLASH] Unlocked successfully\n");
    } else {
        printf("[FLASH] Unlock failed!\n");
        return -1;
    }
    
    /* Step 4: 初始化 ADC 并校准 */
    adc_init();
    int_register(0, isr_adc_done); /* IRQ0 = ADC DONE */
    int_enable(0);
    
    /* 校准：采样参考电压 0.5V */
    adc_config_channel(ADC_CH0, ADC_MODE_8BIT, 1);
    adc_start_channel(ADC_CH0);
    adc_poll_done(ADC_CH0);
    float calib = adc_read_channel(ADC_CH0);
    printf("[ADC] Calibration sample: %.3fV (expected ~0.5V)\n", calib);
    
    /* Step 5: 标记启动完成 */
    hal_reg_set_bits(REG_OFFSET_STATUS, STATUS_BIT_BOOT_DONE);
    printf("[BOOT] Done. Entering main loop.\n");
    
    /* 主循环：模拟固件运行 */
    for (int tick = 0; tick < 3; tick++) {
        int_dispatch(); /* 处理中断 */
        if (g_adc_done_flag) {
            printf("[MAIN] ADC conversion handled at tick %d\n", tick);
            g_adc_done_flag = 0;
        }
    }
    
    printf("=== Bring-up Complete ===\n");
    return 0;
}