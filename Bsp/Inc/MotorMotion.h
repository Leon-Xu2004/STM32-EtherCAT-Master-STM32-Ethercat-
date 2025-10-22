#ifndef __MOTORMOTION_H_
#define __MOTORMOTION_H_

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "soem.h"
#include "ethercat.h"
#include "osal.h"

#define SCALE 1U  			// ����Ŵ�ϵ�������� 1000 ��ʾ���ȱ�����λС��
#define ZeroErrorPPR 	524288		// ����ƿص��ÿȦ������
#define AXIS_1_ORIGIN	3280000
#define AXIS_2_ORIGIN   170000
#define AXIS_3_ORIGIN   3250000
#define AXIS_4_ORIGIN   3115000
#define AXIS_5_ORIGIN	3305000
#define AXIS_6_ORIGIN	3330000

typedef struct {
	uint32_t S_total;   	// ��λ�ƣ���λ * SCALE��
    uint32_t S_acc;     	// ���ٶ�λ�ƣ���λ * SCALE��
    uint32_t S_dec;     	// ���ٶ�λ�ƣ���λ * SCALE��
    uint32_t S_flat;    	// ���ٶ�λ�ƣ���λ * SCALE��

    uint32_t V_max;     	// ����ٶȣ���λ * SCALE/ms��
    uint32_t Acc;       	// ���ٶȣ���λ * SCALE/ms2��
    uint32_t Dec;       	// ���ٶȣ���λ * SCALE/ms2��

    uint32_t Acc_time;      // ����ʱ�䣨��λ��ms��
    uint32_t Dec_time;      // ����ʱ�䣨��λ��ms��
    uint32_t Total_time;    // ��ʱ�䣨��λ��ms��
	
	uint8_t  run_state;   	// 1=����, 2=����, 3=����, 4=���
    uint32_t t_count;     	// �Ѿ���ȥ�� ms ������ǰ�׶εļ�ʱ����
    int32_t  start_pos;   	// �����˶���ʼ��������ֵ
	int32_t  last_pos;		// ��һ�μ�����ġ�����λ�á�
	int32_t  shadow_pos;
	int32_t	 Motor_Nowpos;	// ��ǰѭ�����ڵ��λ��
	int32_t	 Motor_Lastpos;	// �ϴ�ѭ�����ڵ��λ��
    int32_t  dir;         	// ����: +1 �� -1
    bool     busy;        	// ��������Ƿ����˶��ڽ���
	int32_t  step_base;  	// ��̬���ٶε����λ�ã�soft stop �ã�

} T_CurveMotion;

extern T_CurveMotion AxisParameter[6 + 1];


void MotorController_Init(T_CurveMotion* p, float s_total, float v_max, float acc);
int16_t MotorController_Update(T_CurveMotion *p);
void MotorToPulsePosition(uint8_t axis, int32_t position_abs, float v_max, float acc);
void Motor_SoftStop(uint8_t axis);
void ecat_loop(void);

#endif
