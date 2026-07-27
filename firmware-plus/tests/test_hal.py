"""
@file test_hal.py
@brief HAL 单元测试
@description 测试寄存器读写、位运算、非法地址保护。
              固件测试的基础：如果 HAL 层不可靠，上层驱动全部不可信。
"""
import pytest

def test_reg_read_write(chip_init):
    """测试基本读写"""
    lib = chip_init
    lib.hal_reg_write(0x00, 0x12345678)
    assert lib.hal_reg_read(0x00) == 0x12345678

def test_reg_set_clear_bits(chip_init):
    """测试位运算：先写 0x00，置位 bit0 得 0x01，清零 bit0 得 0x00"""
    lib = chip_init
    lib.hal_reg_write(0x04, 0x00)
    
    # 模拟固件中的 set_bits
    val = lib.hal_reg_read(0x04)
    lib.hal_reg_write(0x04, val | 0x01)
    assert lib.hal_reg_read(0x04) == 0x01
    
    val = lib.hal_reg_read(0x04)
    lib.hal_reg_write(0x04, val & ~0x01)
    assert lib.hal_reg_read(0x04) == 0x00

def test_reg_boundary_protection(chip_init):
    """测试非法地址访问：应打印错误但不崩溃"""
    lib = chip_init
    lib.hal_reg_clear_error()           # 清除之前的错误标记
    result = lib.hal_reg_read(0xFFFF)   # 远超 128 字节寄存器空间
    assert lib.hal_reg_had_error() == 1 # C 层检测到非法访问
    assert result == 0xDEADBEEF          # 错误标记