/**
 * @file hal_reg.h
 * @brief 硬件抽象层 - 寄存器定义
 * @description 定义虚拟 CIM 芯片的寄存器映射、位域掩码和访问宏。
 *              固件工程师通过此文件与硬件"对话"，不直接使用硬编码地址。
 */

 #ifndef HAL_REG_H
 #define HAL_REG_H
 #include <stdint.h>
 #include <stddef.h>

 /* 虚拟芯片寄存器基地址 */
 #define CHIP_REG_BASE      0x40000000U

 /* 寄存器偏移量定义 */
 #define REG_OFFSET_CHIP_ID         0x00
 #define REG_OFFSET_STATUS          0x04
 #define REG_OFFSET_ADC_CTRL        0x10
 #define REG_OFFSET_ADC_STATUS      0x14
 #define REG_OFFSET_ADC_CH0_DATA    0x20
 #define REG_OFFSET_ADC_CH1_DATA    0x24
 #define REG_OFFSET_ADC_CH2_DATA    0x28
 #define REG_OFFSET_ADC_CH3_DATA    0x2C
 #define REG_OFFSET_FLASH_CMD       0x40
 #define REG_OFFSET_FLASH_STATUS    0x44
 #define REG_OFFSET_FLASH_ADDR      0x48
 #define REG_OFFSET_FLASH_DATA      0x4C
 #define REG_OFFSET_INT_MASK        0x60
 #define REG_OFFSET_INT_PENDING     0x64

 /* STATUS 寄存器位域 */
 #define STATUS_BIT_BOOT_DONE       (1U << 0)
 #define STATUS_BIT_BIST_PASS       (1U << 1)
 #define STATUS_BIT_ERROR           (1U << 31)

 /* ADC_CTRL 寄存器位域 */
 #define ADC_CTRL_EN             (1U << 0)
 #define ADC_CTRL_START          (1U << 1)
 #define ADC_CTRL_MODE_MASK      (0x7U << 2)
 #define ADC_CTRL_MODE_1BIT      (0x0U << 2)
 #define ADC_CTRL_MODE_2BIT      (0x1U << 2)
 #define ADC_CTRL_MODE_4BIT      (0x2U << 2)
 #define ADC_CTRL_MODE_8BIT      (0x3U << 2)
 #define ADC_CTRL_TEMP_COMP      (1U << 5)
 #define ADC_CTRL_CH_SEL_MASK    (0x3U << 6)  /* 通道选择 0-3 */

 /* ADC_STATUS 寄存器位域 */
 #define ADC_STATUS_BUSY            (1U << 0)
 #define ADC_STATUS_DONE            (1U << 1)
 #define ADC_STATUS_OVERRUN         (1U << 2)

 /* FLASH_CMD 寄存器命令码 */
 #define FLASH_CMD_UNLOCK           0xAA55U
 #define FLASH_CMD_ERASE            0xEE11U
 #define FLASH_CMD_PROGRAM          0xBB66U
 #define FLASH_CMD_VERIFY           0xCC77U

 /* FLASH_STATUS 位域 */
 #define FLASH_STATUS_LOCKED        (1U << 0)
 #define FLASH_STATUS_ERASING       (1U << 1)
 #define FLASH_STATUS_PROGRAMMING   (1U << 2)
 #define FLASH_STATUS_VERIFY_FAIL   (1U << 3)
 
 /* INT 中断位域 */
 #define INT_ADC_DONE               (1U << 0)
 #define INT_FLASH_DONE             (1U << 1)
 #define INT_ERROR                  (1U << 31)

 /* 寄存器总数 */
 #define REG_COUNT                  32

 /* 外部可见的寄存器内存空间（模拟硬件寄存器） */
 extern uint32_t g_reg_space[REG_COUNT];

 /* 安全的寄存器读写接口声明 */
 uint32_t   hal_reg_read(uint32_t offset);
 void       hal_reg_write(uint32_t offset, uint32_t value);
 void       hal_reg_set_bits(uint32_t offset, uint32_t bits);
 void       hal_reg_clear_bits(uint32_t offset, uint32_t bits);

 /* 错误检测接口（供测试/诊断使用） */
 int        hal_reg_had_error(void);
 void       hal_reg_clear_error(void);

 #endif