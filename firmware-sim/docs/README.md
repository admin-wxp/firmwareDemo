# norflash-cim-fw-sim

模拟 NOR Flash CIM 芯片的固件开发与自动化测试框架。

## 项目结构

firmware/          # C 语言固件层（寄存器配置 + ADC 驱动）
tests/             # Python 自动化测试（Corner Case + 报告生成）
docs/              # 测试报告输出

## 核心功能

1. **寄存器位运算配置**：通过 MMIO 模拟配置 ADC 精度（1/2/4/8 bit）、使能温度补偿
2. **ADC 转换模拟**：加入温度相关噪声（模拟电荷泄漏），支持温度补偿算法
3. **自动化测试**：批量跑 Corner Case（温度 × 精度 × 输入电压），生成可视化报告

## 快速运行

```bash
# 1. 编译固件
cd firmware
make

# 2. 运行固件主程序
./fw_sim

# 3. 运行自动化测试（需要先编译共享库）
cd ../tests
pip install -r requirements.txt
python test_adc_corner.py





# 1. 进入你的代码目录（VS Code 已打开该文件夹时，终端默认就在此目录）
cd /path/to/your/project   # 如果终端不在项目根目录，先切换
# 2. 初始化 Git
git init
# 3. 添加所有文件到暂存区
git add .
# 4. 提交到本地仓库
git commit -m "Initial commit"
# 5. 关联 GitHub 远程仓库（把 <URL> 换成你的仓库地址）
git remote add origin https://github.com/用户名/仓库名.git
# 6. 推送到远程 main 分支（如果是 master 分支，把 main 换成 master）
git branch -M main
git config --global http.sslVerify false
git push -u origin main
