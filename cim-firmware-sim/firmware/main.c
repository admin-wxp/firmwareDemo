#include "reg.h"
#include <stdio.h>

int main() {
    printf("=== CIM Firmware Boot ===\n");
    
    // 1. 配置 ADC 为 4-bit
    adc_set_mode(4);
    
    // 2. 使能 Tile0 和 Tile1
    array_enable(ARRAY_EN_TILE0 | ARRAY_EN_TILE1);
    
    // 3. 启动芯片
    chip_enable();
    
    // 4. 模拟设置 Ready 状态（测试用）
    reg_write(REG_STATUS, STATUS_READY);
    
    // 5. 等待就绪
    if (chip_wait_ready() == 0) {
        printf("Chip ready, start inference.\n");
    } else {
        printf("Chip init failed!\n");
        return 1;
    }
    
    return 0;
}