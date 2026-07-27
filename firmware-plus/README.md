# firmware-plus — 虚拟 CIM 芯片固件框架

## 项目概述

本项目是一个**虚拟 CIM（Compute-In-Memory）芯片固件**参考实现，包含完整的 HAL 层、外设驱动、中断管理和 pytest 测试框架。设计用于：
- 固件工程师学习芯片 Bring-up 流程
- 驱动开发与 HAL 层交互实践
- 量产测试逻辑验证（BIST、ADC 校准、Flash 耐久度）

---

## 模块架构

```
firmware-plus/
├── firmware/
│   ├── hal/                ← 硬件抽象层 (Hardware Abstraction Layer)
│   │   ├── hal_reg.h       → 寄存器地址/位域宏定义 (32个32bit寄存器)
│   │   └── hal_reg.c       → 带边界检查的寄存器读写实现 (模拟MMIO)
│   │
│   ├── driver/             ← 外设驱动层 (Device Drivers)
│   │   ├── adc_driver.h/c  → ADC驱动: 4通道、多精度(1/2/4/8bit)、温度补偿
│   │   └── flash_ctrl.h/c  → NOR Flash控制器: 解锁→擦除→编程→验证状态机
│   │
│   ├── core/               ← 中断管理系统 (Interrupt Management)
│   │   └── interrupt.h/c   → ISR注册、使能/禁用、主循环分发 (模拟NVIC)
│   │
│   └── main.c              → Bring-up 主程序 (BIST→Flash解锁→ADC校准→主循环)
│
├── tests/                  ← pytest 测试套件
│   ├── conftest.py         → 共享fixture: 自动编译.so, 提供chip_init
│   ├── test_hal.py         → HAL单元测试: 读写/位操作/非法地址保护
│   ├── test_adc_stress.py  → ADC压力测试: 多通道轮询/温度Corner
│   ├── test_flash_lifecycle.py → Flash寿命测试: 擦写验证/坏块检测
│   └── test_bringup.py     → 系统级Bring-up回归测试
│
├── CMakeLists.txt          → CMake构建 (生成 libcimfw.so + bringup 可执行文件)
├── Makefile                → 快速构建入口 (make → make test)
└── README.md               → 本文件
```

### 各模块详解

#### 1. HAL — 硬件抽象层 (`firmware/hal/`)

固件与硬件之间的桥梁，定义虚拟芯片寄存器的地址映射。所有上层驱动通过 `hal_reg_read()` / `hal_reg_write()` 访问寄存器，不直接操作 `g_reg_space[]`。

- **寄存器地址**: 32个32位寄存器，基址 `0x40000000`
- **安全机制**: 越界访问返回 `0xDEADBEEF` 并输出错误日志，模拟真实硬件的 Bus Fault

#### 2. ADC 驱动 (`firmware/driver/adc_driver.c`)

支持 4 通道轮询采集，状态机管理转换生命周期：
```
IDLE → CONFIG → SAMPLING → DONE → IDLE
```
- **精度模式**: 1-bit / 2-bit / 4-bit / 8-bit
- **温度补偿**: 根据环境温度动态降低噪声（补偿后噪声减少60%）
- **噪声模型**: `noise = (T - 25°C) × 3mV/°C`

#### 3. Flash 控制器 (`firmware/driver/flash_ctrl.c`)

严格模拟 NOR Flash 物理特性：
- **四步写入序列**: 解锁 → 擦除(全1) → 编程(1→0) → 验证
- **耐久度**: 每页最多 10,000 次擦写，超出后标记坏块
- **出厂坏块**: 第3页和第7页预置为坏块（BIST 扫描）

#### 4. 中断管理 (`firmware/core/interrupt.c`)

模拟 NVIC（Nested Vectored Interrupt Controller）行为：
- ISR 注册表（最多 32 个中断源）
- 使能掩码控制
- 挂起位 → 主循环轮询分发

#### 5. Bring-up 主程序 (`firmware/main.c`)

模拟芯片上电自检全流程：
1. **BIST**: 扫描 Flash 坏块表
2. **解锁**: 发送 Flash 解锁命令
3. **ADC 校准**: 采样参考电压 0.5V
4. **主循环**: 分发中断，处理 ADC 完成事件

#### 6. pytest 测试框架 (`tests/`)

通过 `ctypes.CDLL` 加载固件 `.so`，在 Python 层直接驱动 C 代码：
- 自动检测是否需要重新编译（比较源码时间戳）
- `chip_init` fixture 确保每个测试在干净的芯片状态下执行

---

## 运行步骤

### 前置条件
- **gcc** (C99 支持) 或 **CMake** 3.10+
- **Python 3.7+** + `pytest`, `numpy`, `matplotlib`（测试）

### 方式一：Makefile（推荐，需要 CMake）

```bash
# 1. 编译
make

# 2. 运行固件 Bring-up 演示
./build/bringup

# 3. 运行全部测试
make test
```

### 方式二：纯 gcc（无需 CMake）

```bash
# 1. 编译共享库
mkdir -p build
gcc -shared -fPIC -std=c99 \
    -I firmware/hal -I firmware/driver -I firmware/core \
    firmware/hal/hal_reg.c \
    firmware/driver/adc_driver.c \
    firmware/driver/flash_ctrl.c \
    firmware/core/interrupt.c \
    -o build/libcimfw.so

# 2. 编译可执行文件
gcc -std=c99 \
    -I firmware/hal -I firmware/driver -I firmware/core \
    firmware/main.c build/libcimfw.so \
    -o build/bringup

# 3. 运行
./build/bringup

# 4. 运行测试
cd tests && pytest -v
```

### 方式三：仅运行测试（自动编译）

```bash
# conftest.py 会自动检测并编译 libcimfw.so
cd tests
pytest -v
```

### 产物说明

| 文件 | 说明 |
|------|------|
| `build/libcimfw.so` | 固件共享库（被 Python ctypes 加载） |
| `build/bringup` | Bring-up 可执行文件 |
| `tests/__pycache__/` | pytest 缓存（确认测试跑过） |

### Git 提交

```bash
git add .
git commit -m "feat: add HAL + ADC/Flash driver + interrupt + pytest bringup"
git push origin main
```
