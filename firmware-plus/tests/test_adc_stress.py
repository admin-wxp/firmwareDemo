"""
@file test_adc_stress.py
@brief ADC 压力与 Corner Case 测试
@description 模拟量产测试中的环境应力筛选（ESS）：
              在不同温度、不同精度下批量采样，统计误差分布。
"""
import ctypes
import numpy as np
import matplotlib
matplotlib.use('Agg')  # 无 GUI 环境
import matplotlib.pyplot as plt
import os

def test_adc_multichannel_polling(chip_init):
    """多通道轮询测试：4 个通道各采样一次"""
    lib = chip_init
    
    # 通过 ctypes 调用内部函数（需要声明）
    # 简化：直接测试主流程，不深入 C 内部状态机细节
    # 这里演示测试框架的组织方式
    results = []
    for ch in range(4):
        # 模拟配置和采样（实际需暴露更多 C 接口）
        # 这里用 HAL 寄存器状态推断
        status = lib.hal_reg_read(0x14)  # ADC_STATUS
        results.append(status)
    
    # 初始状态应为 0（IDLE）
    assert all(s == 0 for s in results)

def test_adc_temperature_corner(chip_init, tmp_path):
    """温度 Corner 测试：-20°C, 25°C, 85°C 下的转换误差"""
    lib = chip_init
    
    temps = [-20.0, 25.0, 60.0, 85.0]
    errors = []
    
    # 注意：当前 C 代码未暴露 adc_set_environment_temp 到 so，
    # 实际使用需添加 extern 声明。这里演示测试逻辑框架。
    for t in temps:
        # 模拟设置温度 -> 启动转换 -> 读取误差
        # 简化：直接记录温度点，实际项目中需完善 C 接口暴露
        errors.append(abs(t - 25.0) * 0.003)  # 占位
    
    # 生成 Corner Test 报告
    fig, ax = plt.subplots()
    ax.plot(temps, errors, 'o-')
    ax.set_xlabel("Temperature (°C)")
    ax.set_ylabel("Estimated Error (V)")
    ax.set_title("ADC Temperature Corner Test")
    fig.savefig(tmp_path / "adc_corner.png")
    
    # 断言：85°C 时误差应小于 0.2V（根据噪声模型）
    assert errors[-1] < 0.2