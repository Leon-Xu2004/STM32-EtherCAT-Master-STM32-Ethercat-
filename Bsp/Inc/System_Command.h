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


#define UART_BUFFER_SIZE 256	// һ�ν��������
#define MAX_CMD_LENGTH 64       // ÿ��ָ����󳤶ȣ��ַ�����
#define MAX_CMD_TOKENS 16       // ÿ��ָ�����ּ����ʣ��� "M1 GOTO 123456" ��3����

extern __attribute__((aligned(32))) uint8_t uart_rx_buffer[UART_BUFFER_SIZE];

// ֧�ֵ�ָ������
typedef enum {
    CMD_INVALID = 0,     // ��Чָ��
    CMD_M_GOTO,          // M<n> GOTO <pos>
    CMD_M_STOP,          // M<n> STOP
    CMD_M_STATUS,        // M<n> STATUS
    CMD_ALL_STOP,        // ALL STOP
    CMD_ALL_RESET,       // ALL RESET
    CMD_HELP,            // HELP
	CMD_INIT			 // INIT
} CommandType;

// ָ��ṹ�壬��Ž������
typedef struct {
    CommandType type;    	// ָ������
    int motor_id;        	// �����ţ��� M1 ���� 1��
    int target_pos;      	// Ŀ��λ�ã�ֻ�� GOTO ָ���ã�
	int v_max;          	// ����ٶ�
    int acc;            	// ���ٶ�
} SerialCommand;

void ProcessReceivedData(uint8_t *data, uint16_t len);


#endif
