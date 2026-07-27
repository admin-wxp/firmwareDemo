/**
 * @file flash_ctrl.h
 * @brief NOR Flash 控制器接口
 * @description NOR Flash 的写入必须遵循"解锁→擦除→编程→验证"四步序列。
 *              本驱动通过状态机严格管理该流程，防止误操作损坏数据。
 */
#ifndef FLASH_CTRL_H
#define FLASH_CTRL_H

#include <stdint.h>

/* Flash 操作状态 */
typedef enum {
    FLASH_STATE_IDLE = 0,
    FLASH_STATE_UNLOCKED,
    FLASH_STATE_ERASING,
    FLASH_STATE_PROGRAMMING,
    FLASH_STATE_VERIFYING,
    FLASH_STATE_DONE,
    FLASH_STATE_ERROR
} flash_state_t;

/* 初始化 Flash 控制器 */
void flash_init(void);

/* 解锁 Flash（必须先解锁才能擦写） */
int flash_unlock(void);

/* 擦除指定页（模拟：页大小 256 字节） */
int flash_erase_page(uint32_t page_addr);

/* 编程（写入）数据 */
int flash_program(uint32_t addr, uint8_t data);

/* 验证写入数据 */
int flash_verify(uint32_t addr, uint8_t expected);

/* 获取状态 */
flash_state_t flash_get_state(void);

/* 获取坏块表（模拟出厂坏块） */
const uint8_t* flash_get_bad_block_table(void);

#endif /* FLASH_CTRL_H */