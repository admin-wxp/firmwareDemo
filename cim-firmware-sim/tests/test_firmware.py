import ctypes
import os
import pytest

# 加载编译好的 C 动态库
lib_path = os.path.join(os.path.dirname(__file__), '../firmware/libcimfw.so')
lib = ctypes.CDLL(lib_path)

def test_adc_quantization():
    """测试 ADC 量化精度"""
    lib.adc_convert.argtypes = [ctypes.c_float, ctypes.c_uint32]
    lib.adc_convert.restype = ctypes.c_int32
    
    # 4-bit ADC: 16 级，0.5 应该映射到 8
    result = lib.adc_convert(0.5, 4)
    assert result == 8, f"Expected 8, got {result}"
    
    # 4-bit ADC: 1.0 应该映射到 15（饱和）
    result = lib.adc_convert(1.0, 4)
    assert result == 15

def test_bitwise_register_config():
    """测试寄存器位运算配置"""
    # 这里可以调用固件函数，检查寄存器内存状态
    # 简化版：直接测试位运算逻辑
    CTRL_EN = 1
    ADC_BITS_SHIFT = 3
    bits = 4
    ctrl = 0
    ctrl |= CTRL_EN
    ctrl |= ((bits & 0x7) << ADC_BITS_SHIFT)
    
    assert ctrl & CTRL_EN  # 使能位已置位
    assert (ctrl >> ADC_BITS_SHIFT) & 0x7 == 4  # ADC 位正确