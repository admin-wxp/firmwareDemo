"""
@file conftest.py
@brief pytest 共享 fixture 和固件加载
@description 所有测试共享同一个固件 so 库。提供 chip_init fixture，
             确保每个测试用例开始时芯片处于已知初始状态。
"""
import ctypes
import os
import subprocess
import pytest

# 项目根目录
ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SO_PATH = os.path.join(ROOT_DIR, "build", "libcimfw.so")

def _build_firmware():
    """编译固件为共享库（如果还没编译）"""
    build_dir = os.path.join(ROOT_DIR, "build")
    os.makedirs(build_dir, exist_ok=True)

    # 使用 gcc 直接编译（不依赖 CMake，简化测试环境）
    srcs = [
        os.path.join(ROOT_DIR, "firmware", "hal", "hal_reg.c"),
        os.path.join(ROOT_DIR, "firmware", "driver", "adc_driver.c"),
        os.path.join(ROOT_DIR, "firmware", "driver", "flash_ctrl.c"),
        os.path.join(ROOT_DIR, "firmware", "core", "interrupt.c"),
    ]

    cmd = ["gcc", "-shared", "-fPIC", "-O2", "-std=c99",
           "-I" + os.path.join(ROOT_DIR, "firmware", "hal"),
           "-I" + os.path.join(ROOT_DIR, "firmware", "driver"),
           "-I" + os.path.join(ROOT_DIR, "firmware", "core"),
           "-o", SO_PATH] + srcs
    
    # 如果 so 已存在且比源码新，跳过编译
    if os.path.exists(SO_PATH) and all(os.path.getmtime(SO_PATH) > os.path.getmtime(s) for s in srcs):
        return
    
    subprocess.run(cmd, check=True)
    print(f"[Build] Compiled {SO_PATH}")

@pytest.fixture(scope="session", autouse=True)
def firmware_so():
    """Session 级别 fixture：确保固件已编译"""
    _build_firmware()
    lib = ctypes.CDLL(SO_PATH)
    
    # 声明函数签名（关键：否则 ctypes 默认用 int，可能截断 float 指针）
    lib.adc_init.argtypes = []
    lib.flash_init.argtypes = []
    lib.hal_reg_read.argtypes = [ctypes.c_uint32]
    lib.hal_reg_read.restype = ctypes.c_uint32
    lib.hal_reg_write.argtypes = [ctypes.c_uint32, ctypes.c_uint32]
    lib.hal_reg_had_error.argtypes = []
    lib.hal_reg_had_error.restype = ctypes.c_int
    lib.hal_reg_clear_error.argtypes = []

    return lib

@pytest.fixture
def chip_init(firmware_so):
    """Function 级别 fixture：每个测试前初始化芯片"""
    firmware_so.adc_init()
    firmware_so.flash_init()
    # 清零寄存器空间（通过写 0 实现）
    for offset in range(0, 128, 4):
        firmware_so.hal_reg_write(offset, 0)
    return firmware_so