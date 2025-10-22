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

## 🧩 系统架构 | System Architecture

```text
+-----------------------------------------------------------+
|                   Application Layer                       |
|  ├── MotorController_Init()   # 三段式速度规划初始化       |
|  ├── MotorController_Update() # 实时增量计算与调度         |
|  └── ecat_loop()              # EtherCAT 同步主循环         |
+-----------------------------------------------------------+
|                Protocol Stack Layer (SOEM)                |
|  ├── ecat_init()              # 主站初始化与从站识别        |
|  ├── Zeroer_Nsetup()          # 对象字典与PDO映射          |
|  ├── Zeroer_CSPInit()         # 从站CSP模式初始化           |
|  └── ec_dcsync0()             # 分布式时钟同步（68μs）     |
+-----------------------------------------------------------+
|                Hardware Driver Layer (HAL)                |
|  ├── MX_ETH_Init()           # MAC + DMA 初始化             |
|  ├── LAN8720_Init()          # PHY 自动协商与复位          |
|  ├── MX_TIM4_Init()          # 1kHz周期定时器驱动任务       |
|  └── FreeRTOS (可选)         # 实时任务管理                 |
+-----------------------------------------------------------+
