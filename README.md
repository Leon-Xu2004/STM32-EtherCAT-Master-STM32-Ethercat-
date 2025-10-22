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

