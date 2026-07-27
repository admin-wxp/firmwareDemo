"""
ADC Corner Case 自动化测试
模拟固件测试岗的日常工作：批量配置、采集数据、记录异常
"""
import ctypes
import os
import json
import numpy as np
import matplotlib.pyplot as plt

#加载编译好的共享库
LIB_PATH = os.path.join(os.path.dirname(__file__) , '..', 'firmware', 'libadcsim.so')
lib = ctypes.CDLL(os.path.abspath(LIB_PATH))

# 定义c函数的接口
lib.adc_init.argtypes = []



lib.adc_init.restype = None

lib.adc_set_mode.argtypes = [ctypes.c_int]
lib.adc_set_mode.restype = None

lib.adc_enable.argtypes = []
lib.adc_enable.restype = None

lib.adc_start_convert.argtypes = [ctypes.c_float, ctypes.c_float]
lib.adc_start_convert.restype = None

lib.adc_read_data.restype = ctypes.c_float

lib.adc_get_mode_bits.restype = ctypes.c_uint32


class ADCTestFramework:
    """模拟芯片测试框架"""
    
    def __init__(self):
        self.results = []
    
    def run_single(self, input_volt, temperature, adc_bits, temp_comp=False):
        """运行单次测试"""
        lib.adc_init()
        
        # 映射 Python 参数到 C 枚举
        mode_map = {1: 1, 2: 2, 4: 4, 8: 8}
        lib.adc_set_mode(mode_map.get(adc_bits, 4))
        
        if temp_comp:
            # 通过直接操作寄存器空间使能温度补偿（模拟固件行为）
            # 这里简化：在 C 库中通过全局变量暴露，或重新编译
            # 实际项目中会用更优雅的接口
            pass  # 当前 C 代码默认在 main 中演示，测试框架中简化
        
        lib.adc_enable()
        lib.adc_start_convert(input_volt, temperature)
        
        result = lib.adc_read_data()
        actual_bits = lib.adc_get_mode_bits()
        
        error = abs(result - input_volt)
        
        record = {
            "input": input_volt,
            "temperature": temperature,
            "adc_bits": actual_bits,
            "output": round(result, 4),
            "error": round(error, 4),
            "pass": error < 0.05  # 5% 误差阈值
        }
        self.results.append(record)
        return record
    
    def run_corner_suite(self):
        """运行 Corner Case 测试集"""
        print("=" * 60)
        print("Starting ADC Corner Case Test Suite...")
        print("=" * 60)
        
        # Corner Cases: 不同温度 × 不同精度 × 不同输入
        temperatures = [-20.0, 25.0, 60.0, 85.0]
        adc_modes = [2, 4, 8]  # 1bit 误差太大，通常不测
        inputs = [0.1, 0.3, 0.5, 0.7, 0.9]
        
        total = 0
        passed = 0
        
        for temp in temperatures:
            for bits in adc_modes:
                for vin in inputs:
                    r = self.run_single(vin, temp, bits)
                    total += 1
                    if r["pass"]:
                        passed += 1
                    status = "PASS" if r["pass"] else "FAIL"
                    print(f"[{status}] T={temp:3.0f}C | {bits}bit | "
                          f"Vin={vin:.1f}V | Vout={r['output']:.3f}V | "
                          f"Err={r['error']:.3f}V")
        
        print("=" * 60)
        print(f"Total: {total}, Passed: {passed}, Failed: {total-passed}")
        print(f"Pass Rate: {passed/total*100:.1f}%")
        return self.results
    
    def generate_report(self, output_dir="docs"):
        """生成测试报告（可视化）"""
        os.makedirs(output_dir, exist_ok=True)
        
        temps = [r["temperature"] for r in self.results]
        errors = [r["error"] for r in self.results]
        bits = [r["adc_bits"] for r in self.results]
        
        # 按精度分组画图
        plt.figure(figsize=(10, 6))
        for b in sorted(set(bits)):
            x = [r["temperature"] for r in self.results if r["adc_bits"] == b]
            y = [r["error"] for r in self.results if r["adc_bits"] == b]
            plt.scatter(x, y, label=f"{b}-bit ADC", alpha=0.6, s=60)
        
        plt.xlabel("Temperature (°C)")
        plt.ylabel("Conversion Error (V)")
        plt.title("ADC Conversion Error vs Temperature (Corner Test)")
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, "adc_corner_test.png"), dpi=150)
        print(f"Report saved to {output_dir}/adc_corner_test.png")
        
        # 保存 JSON 原始数据
        with open(os.path.join(output_dir, "test_raw.json"), "w") as f:
            json.dump(self.results, f, indent=2)


if __name__ == "__main__":
    fw = ADCTestFramework()
    fw.run_corner_suite()
    fw.generate_report()