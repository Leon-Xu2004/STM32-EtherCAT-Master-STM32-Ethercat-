#ifndef __SYSTEM_COMMAND_H
#define __SYSTEM_COMMAND_H


#include "stm32h7xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "MotorMotion.h"
#include "bsp_led.h"
#include "ethernetif.h"
#include "bsp_GeneralTIM.h"
#include "ethercat.h"
#include "soem.h"


#define UART_BUFFER_SIZE 256	// 一次接收最大量
#define MAX_CMD_LENGTH 64       // 每条指令最大长度（字符数）
#define MAX_CMD_TOKENS 16       // 每条指令最多分几个词（如 "M1 GOTO 123456" 是3个）

extern __attribute__((aligned(32))) uint8_t uart_rx_buffer[UART_BUFFER_SIZE];

// 支持的指令类型
typedef enum {
    CMD_INVALID = 0,     // 无效指令
    CMD_M_GOTO,          // M<n> GOTO <pos>
    CMD_M_STOP,          // M<n> STOP
    CMD_M_STATUS,        // M<n> STATUS
    CMD_ALL_STOP,        // ALL STOP
    CMD_ALL_RESET,       // ALL RESET
    CMD_HELP,            // HELP
	CMD_INIT			 // INIT
} CommandType;

// 指令结构体，存放解析结果
typedef struct {
    CommandType type;    	// 指令类型
    int motor_id;        	// 电机编号（如 M1 就是 1）
    int target_pos;      	// 目标位置（只有 GOTO 指令用）
	int v_max;          	// 最大速度
    int acc;            	// 加速度
} SerialCommand;

void ProcessReceivedData(uint8_t *data, uint16_t len);


#endif
