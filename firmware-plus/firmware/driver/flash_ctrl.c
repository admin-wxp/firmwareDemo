/**
 * @file flash_ctrl.c
 * @brief NOR Flash 控制器实现
 * @description 严格模拟 NOR Flash 的物理特性：
 *              1. 必须先擦除再编程（不能直接覆盖）
 *              2. 擦写次数有限（耐久度 10^4 次）
 *              3. 存在出厂坏块（BIST 检测）
 */
#include "flash_ctrl.h"
#include "hal_reg.h"
#include <string.h>
#include <stdio.h>

#define FLASH_PAGE_SIZE     256
#define FLASH_PAGE_COUNT    16
#define FLASH_MAX_ERASE     10000U  /* 模拟耐久度 */

static flash_state_t s_flash_state = FLASH_STATE_IDLE;
static uint32_t s_erase_count[FLASH_PAGE_COUNT] = {0};
static uint8_t  s_flash_mem[FLASH_PAGE_COUNT][FLASH_PAGE_SIZE];
static uint8_t  s_bad_blocks[FLASH_PAGE_COUNT] = {0};

/* 模拟出厂坏块：第 3 页和第 7 页为坏块 */
static const uint8_t s_factory_bad[FLASH_PAGE_COUNT] = {
    0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0
};

void flash_init(void)
{
    s_flash_state = FLASH_STATE_IDLE;
    memset(s_erase_count, 0, sizeof(s_erase_count));
    memset(s_flash_mem, 0xFF, sizeof(s_flash_mem)); /* 擦除后全 1 */
    memcpy(s_bad_blocks, s_factory_bad, sizeof(s_bad_blocks));
    
    /* 写入状态寄存器：初始锁定 */
    hal_reg_write(REG_OFFSET_FLASH_STATUS, FLASH_STATUS_LOCKED);
}

int flash_unlock(void)
{
    if (s_flash_state != FLASH_STATE_IDLE) return -1;
    
    uint32_t cmd = hal_reg_read(REG_OFFSET_FLASH_CMD);
    if (cmd != FLASH_CMD_UNLOCK) {
        /* 命令错误，模拟硬件拒绝 */
        return -2;
    }
    
    s_flash_state = FLASH_STATE_UNLOCKED;
    hal_reg_clear_bits(REG_OFFSET_FLASH_STATUS, FLASH_STATUS_LOCKED);
    return 0;
}

int flash_erase_page(uint32_t page_addr)
{
    if (s_flash_state != FLASH_STATE_UNLOCKED) return -1;
    
    uint32_t page = page_addr / FLASH_PAGE_SIZE;
    if (page >= FLASH_PAGE_COUNT) return -3;
    
    if (s_bad_blocks[page]) return -4; /* 坏块，拒绝擦除 */
    
    s_flash_state = FLASH_STATE_ERASING;
    hal_reg_set_bits(REG_OFFSET_FLASH_STATUS, FLASH_STATUS_ERASING);
    
    /* 模拟擦除延时和耐久度 */
    s_erase_count[page]++;
    if (s_erase_count[page] > FLASH_MAX_ERASE) {
        s_bad_blocks[page] = 1; /* 耐久度耗尽，标记坏块 */
        s_flash_state = FLASH_STATE_ERROR;
        return -5;
    }
    
    memset(s_flash_mem[page], 0xFF, FLASH_PAGE_SIZE);
    
    s_flash_state = FLASH_STATE_UNLOCKED; /* 擦除后保持解锁 */
    hal_reg_clear_bits(REG_OFFSET_FLASH_STATUS, FLASH_STATUS_ERASING);
    return 0;
}

int flash_program(uint32_t addr, uint8_t data)
{
    if (s_flash_state != FLASH_STATE_UNLOCKED) return -1;
    
    uint32_t page = addr / FLASH_PAGE_SIZE;
    uint32_t offset = addr % FLASH_PAGE_SIZE;
    
    if (page >= FLASH_PAGE_COUNT) return -3;
    if (s_bad_blocks[page]) return -4;
    
    s_flash_state = FLASH_STATE_PROGRAMMING;
    hal_reg_set_bits(REG_OFFSET_FLASH_STATUS, FLASH_STATUS_PROGRAMMING);
    
    /* NOR Flash：只能把 1 改成 0，不能直接把 0 改成 1（必须先擦除） */
    s_flash_mem[page][offset] &= data;
    
    s_flash_state = FLASH_STATE_UNLOCKED;
    hal_reg_clear_bits(REG_OFFSET_FLASH_STATUS, FLASH_STATUS_PROGRAMMING);
    return 0;
}

int flash_verify(uint32_t addr, uint8_t expected)
{
    if (s_flash_state != FLASH_STATE_UNLOCKED) return -1;
    
    uint32_t page = addr / FLASH_PAGE_SIZE;
    uint32_t offset = addr % FLASH_PAGE_SIZE;
    
    s_flash_state = FLASH_STATE_VERIFYING;
    
    uint8_t actual = s_flash_mem[page][offset];
    int pass = (actual == expected);
    
    if (!pass) {
        hal_reg_set_bits(REG_OFFSET_FLASH_STATUS, FLASH_STATUS_VERIFY_FAIL);
    }
    
    s_flash_state = FLASH_STATE_UNLOCKED;
    return pass ? 0 : -1;
}

flash_state_t flash_get_state(void) { return s_flash_state; }

const uint8_t* flash_get_bad_block_table(void) { return s_bad_blocks; }