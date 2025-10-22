#ifndef _ECATUSER_H_
#define _ECATUSER_H_

/* 包含头文件 ----------------------------------------------------------------*/
#include "ethercat.h"
#include "osal.h"
#include "bsp_key.h"
#include "stdio.h"
#include "string.h"
#include "stm32h7xx_hal.h"
#include "math.h"
#include "Stdbool.h"
#include "bsp_led.h"
#include "MotorMotion.h"

/* 私有类型定义 --------------------------------------------------------------*/


#define MILLISECONDE 	1						// ms
#define SYNC0TIME 		MILLISECONDE*1000		// us
#define SHIFTSETTIME 	SYNC0TIME*300  			// ns
#define DEBUG 1
#define Max_Axis 6

PACKED_BEGIN
typedef struct PACKED
	{
    uint16_t ControlWord;          // 0x6040:00h (16 bit)
    int8_t   OpModeSet;            // 0x6060:00h (8 bit)
    uint32_t ProfileVelocity;      // 0x6081:00h (32 bit)
    uint32_t ProfileAcceleration;  // 0x6083:00h (32 bit)
    uint32_t ProfileDeceleration;  // 0x6084:00h (32 bit)
    int32_t  TargetPosition;       // 0x607A:00h (32 bit)
	uint8_t  Padding;
} Zeroer_RxPDO;
	PACKED_END

PACKED_BEGIN
typedef struct PACKED
	{
    uint16_t StatusWord;          	// 0x6041:00h (16 bit)
    int8_t   ModeDisplay;          	// 0x6061:00h (8 bit)
    int32_t  ActualPosition;       	// 0x6064:00h (32 bit)
    int32_t  ErrorValue;           	// 0x60F4:00h (32 bit)
	uint8_t  Padding;
} Zeroer_TxPDO;
	PACKED_END

typedef struct {
    uint8_t step;        // 当前状态步骤（0-初始化中，1-完成）
    uint32_t timeout;    // 超时计数器（单位：ms）
    bool is_enabled;     // 是否已使能
} AxisState;



extern char IOmap[4096];
extern Zeroer_RxPDO *outputs[Max_Axis + 1];
extern Zeroer_TxPDO *inputs[Max_Axis + 1];
extern int Motor_Run;


void ecat_init(void);
int write32(uint16 slave, uint16 index, uint8 subindex, int value);
int write8(uint16  slave, uint16  index, uint8  subindex, int value);
void Zeroer_CSPInit(void);
int Zeroer_Nsetup(uint16 slvcnt);
void RecoverFromFault(uint8_t axis);
#endif
