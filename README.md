# ⚙️ 基于 STM32 的 EtherCAT 主站运动控制系统
[![Platform](https://img.shields.io/badge/Platform-STM32H743-blue?logo=stmicroelectronics)]()
[![EtherCAT](https://img.shields.io/badge/Protocol-EtherCAT-red?logo=siemens)]()
[![Language](https://img.shields.io/badge/Language-C%2FC%2B%2B-green)]()
[![License](https://img.shields.io/badge/License-MIT-yellow)]()
[![Status](https://img.shields.io/badge/Version-v1.0.0-brightgreen)]()

> 🧠 **Author:** Leon Xu（徐领航）  
> 📅 **Date:** 2025.05  
> 📄 完整技术说明书见 [Docs/基于STM32的EtherCAT主站运动控制系统说明书.pdf](Docs/基于STM32的EtherCAT主站运动控制系统说明书.pdf)

---

## 📘 项目简介 | Overview

本项目基于 **STM32H743** 平台实现嵌入式 **EtherCAT 主站运动控制系统**。  
通过移植 **SOEM (Simple Open EtherCAT Master)** 协议栈，实现对多台伺服电机的 **周期同步位置控制（CSP）**。  
系统融合了 **高精度时钟同步（68μs ±1μs）** 与 **梯形加减速轨迹规划算法**，可实现工业级实时控制与高稳定性。

本系统可广泛应用于：
- 工业机器人多轴协调控制  
- 数控机床、运动平台系统  
- 智能制造与嵌入式运动控制设备  

---

## 🧩 系统架构 | System Architecture

+-----------------------------------------------------------+
| Application Layer |
| ├── MotorController_Init() # 三段式速度规划初始化 |
| ├── MotorController_Update() # 实时增量计算与调度 |
| └── ecat_loop() # EtherCAT 同步主循环 |
+-----------------------------------------------------------+
| Protocol Stack Layer (SOEM) |
| ├── ecat_init() # 主站初始化与从站识别 |
| ├── Zeroer_Nsetup() # 对象字典与PDO映射 |
| ├── Zeroer_CSPInit() # 从站CSP模式初始化 |
| └── ec_dcsync0() # 分布式时钟同步（68μs） |
+-----------------------------------------------------------+
| Hardware Driver Layer (HAL) |
| ├── MX_ETH_Init() # MAC + DMA 初始化 |
| ├── LAN8720_Init() # PHY 自动协商与复位 |
| ├── MX_TIM4_Init() # 1kHz周期定时器驱动任务 |
| └── FreeRTOS (可选) # 实时任务管理 |
+-----------------------------------------------------------+

---

## ⚙️ 核心特性 | Key Features

| 模块 | 功能描述 |
|------|-----------|
| 🧩 多轴同步控制 | 基于 EtherCAT CSP 模式，支持多伺服周期同步控制 |
| ⚡ 高精度周期通信 | 68μs ±1μs 通信周期，低抖动高鲁棒性 |
| 🚀 梯形加减速算法 | 平滑运动、低冲击、快速响应 |
| 🔧 模块化设计 | 底层驱动 / 协议栈 / 应用层清晰分层 |
| 📡 串口控制命令 | 通过 UART 下发 “GOTO / STOP / STATUS”等命令 |
| 🧠 实时插补 | 每周期实时生成位移目标，保持轨迹连续性 |

---

## 🧮 三段式梯形加减速算法 (Trapezoidal Velocity Profile)

该算法通过加速、匀速、减速三阶段控制伺服位移，实现平滑轨迹与稳定加减速。  

公式如下：

\[
t_a = \frac{v_{max}}{a_{max}}, \quad
S_{acc} = \frac{1}{2}a_{max}t_a^2, \quad
S_{flat} = S_{total} - 2S_{acc}
\]

\[
S(t)=
\begin{cases}
\frac{1}{2}a_{max}t^2 & 0<t<t_a\\
S_{acc}+v_{max}(t-t_a) & t_a<t<t_a+t_m\\
S_{total}-\frac{1}{2}a_{max}(t_f-t)^2 & t_a+t_m<t<t_f
\end{cases}
\]

---

## 📡 串口控制指令 | Serial Commands

| 指令 | 示例 | 功能说明 |
|------|------|----------|
| `M<n> GOTO <pos>` | `M1 GOTO 200000` | 控制 n 号伺服运动到指定位置（单位：脉冲） |
| `M<n> STOP` | `M2 STOP` | 停止指定伺服运动 |
| `M<n> STATUS` | `M1 STATUS` | 查询伺服当前状态与位置 |
| `ALL STOP` | `ALL STOP` | 停止全部电机 |
| `HELP` | `HELP` | 显示所有支持指令 |

---

## 🧱 硬件配置 | Hardware Setup

| 硬件 | 型号 / 描述 |
|------|---------------|
| MCU | STM32H743IIT6 (Cortex-M7, 400MHz) |
| PHY | LAN8720A (RMII 接口) |
| 伺服电机 | eRob70H100T-BM-18EN / eRob80H100T-BM-18EN |
| 电源 | 明纬 48V / 400W |
| 调试器 | J-Link v9 |
| 开发环境 | STM32CubeIDE / Keil MDK 5 |

---

## 🧰 项目结构 | Project Structure

STM32_EtherCAT_Master/
├── Core/
│ ├── Inc/ # 头文件（PID, Motion, Protocol, etc.）
│ └── Src/ # 源文件
├── Drivers/ # STM32 HAL 驱动层
├── Ethercat/ # SOEM 协议栈
├── Docs/ # 技术文档与说明书
│ └── 基于STM32的EtherCAT主站运动控制系统说明书.pdf
├── MDK-ARM/ # Keil 工程文件
├── Readme/ # 附加参考文档
└── README.md


---

## 🎥 演示视频 | Demo Video

[![点击观看演示视频](Docs/images/demo_cover.png)](https://www.bilibili.com/video/BVxxxxxxxxx/)

> 视频展示了 STM32 EtherCAT 主站对多伺服电机进行实时同步控制与加减速插补的效果。

---

## 🚀 快速上手 | Getting Started

### 1️⃣ 编译与下载
```bash
# 打开 STM32CubeIDE 或 Keil MDK 工程
# 选择目标芯片 STM32H743
# 编译并通过 J-Link 下载至目标板

**### 2️⃣ 串口连接
**
波特率：115200
数据位：8
停止位：1
校验位：无
使用串口助手输入指令即可控制伺服动作。
