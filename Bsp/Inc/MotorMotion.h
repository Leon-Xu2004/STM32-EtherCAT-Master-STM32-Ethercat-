#ifndef __MOTORMOTION_H_
#define __MOTORMOTION_H_

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "soem.h"
#include "ethercat.h"
#include "osal.h"

#define SCALE 1U  			// 定点放大系数，例如 1000 表示精度保留三位小数
#define ZeroErrorPPR 	524288		// 零差云控电机每圈脉冲数
#define AXIS_1_ORIGIN	3280000
#define AXIS_2_ORIGIN   170000
#define AXIS_3_ORIGIN   3250000
#define AXIS_4_ORIGIN   3115000
#define AXIS_5_ORIGIN	3305000
#define AXIS_6_ORIGIN	3330000

typedef struct {
	uint32_t S_total;   	// 总位移（单位 * SCALE）
    uint32_t S_acc;     	// 加速段位移（单位 * SCALE）
    uint32_t S_dec;     	// 减速段位移（单位 * SCALE）
    uint32_t S_flat;    	// 匀速段位移（单位 * SCALE）

    uint32_t V_max;     	// 最大速度（单位 * SCALE/ms）
    uint32_t Acc;       	// 加速度（单位 * SCALE/ms2）
    uint32_t Dec;       	// 减速度（单位 * SCALE/ms2）

    uint32_t Acc_time;      // 加速时间（单位：ms）
    uint32_t Dec_time;      // 减速时间（单位：ms）
    uint32_t Total_time;    // 总时间（单位：ms）
	
	uint8_t  run_state;   	// 1=加速, 2=匀速, 3=减速, 4=完成
    uint32_t t_count;     	// 已经过去的 ms 数（当前阶段的计时器）
    int32_t  start_pos;   	// 本次运动起始绝对脉冲值
	int32_t  last_pos;		// 上一次计算出的“绝对位置”
	int32_t  shadow_pos;
	int32_t	 Motor_Nowpos;	// 当前循环周期电机位置
	int32_t	 Motor_Lastpos;	// 上次循环周期电机位置
    int32_t  dir;         	// 方向: +1 或 -1
    bool     busy;        	// 标记轴上是否有运动在进行
	int32_t  step_base;  	// 动态减速段的起点位置（soft stop 用）

} T_CurveMotion;

extern T_CurveMotion AxisParameter[6 + 1];


void MotorController_Init(T_CurveMotion* p, float s_total, float v_max, float acc);
int16_t MotorController_Update(T_CurveMotion *p);
void MotorToPulsePosition(uint8_t axis, int32_t position_abs, float v_max, float acc);
void Motor_SoftStop(uint8_t axis);
void ecat_loop(void);

#endif
