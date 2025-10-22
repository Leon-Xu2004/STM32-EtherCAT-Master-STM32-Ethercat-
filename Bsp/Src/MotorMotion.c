#include "MotorMotion.h"

int Motor_Run;

/**
 * @brief 初始化定点三段速度曲线参数并重置运行状态
 *sssssssssssss
 * 本函数将浮点表达的“总位移、最大速度、加速度”转换为定点整数，并计算
 * 出加速阶段、匀速阶段、减速阶段的持续时间和各段位移（已放大 SCALE 倍）。
 * 同时重置运行时状态，使得后续可以在每个周期调用 @c MotorionController_Update
 * 函数获得实时脉冲输出。
 *
 * @param[in,out] p         目标轴的曲线参数结构体
 * @param[in]     s_total   总位移，单位：脉冲（pulse）
 * @param[in]     v_max     最大速度，单位：脉冲/毫秒（pulse/ms）
 * @param[in]     acc       加速度，单位：脉冲/毫秒2（pulse/ms2）
 *
 * @note
 * - p->S_total （uint32_t）  ：  存储单位 = 脉冲 × SCALE
 * - p->V_max   （uint32_t）  ：  存储单位 = (脉冲/ms) × SCALE
 * - p->Acc     （uint32_t）  ：  存储单位 = (脉冲/ms2) × SCALE
 * - p->Dec     （uint32_t）  ：  存储单位 = (脉冲/ms2) × SCALE（对称）
 * - p->Acc_time（uint32_t）  ：  加速阶段持续时间，单位：毫秒（ms）
 * - p->Dec_time（uint32_t）  ：  减速阶段持续时间，单位：毫秒（ms）
 * - p->S_acc   （uint32_t）  ：  加速阶段结束累计位移，单位：脉冲
 * - p->S_dec   （uint32_t）  ：  减速阶段开始前累计位移，单位：脉冲
 * - p->S_flat  （uint32_t）  ：  匀速阶段总位移，单位：脉冲
 * - p->Total_time（uint32_t）：  整个运动总耗时，单位：毫秒（ms）
 * - p->run_state（uint8_t）  ：  运行时状态 1=加速, 2=匀速, 3=减速, 4=完成
 * - p->t_count （uint32_t）  ：  当前阶段已用时间，单位：毫秒（ms）
 * - p->start_pos（int32_t）  ：  运动起始脉冲位置，单位：脉冲
 * - p->dir     （int32_t）   ：  运动方向，+1 或 -1，无单位
 * - p->busy    （bool）	  ：  运动忙标志，无单位
 */
void MotorController_Init(T_CurveMotion* p,
                          float s_total,
                          float v_max,
                          float acc)
{

    // 初始化运动相关参数
    p->run_state = 1;
    p->t_count   = 0;
    p->busy 	 = true;
    p->last_pos  = 0;


    // 转为定点整数值（乘 SCALE）
    p->S_total  = (uint32_t)(s_total + 0.5f);
    p->V_max    = (uint32_t)(v_max   + 0.5f);
    p->Acc      = (uint32_t)(acc     + 0.5f);
    p->Dec      = p->Acc;

    // 加减速时间
    p->Acc_time = (p->V_max + p->Acc / 2) / p->Acc;
    p->Dec_time = p->Acc_time;

    // 加减速位移
    uint64_t t2 = (uint64_t)p->Acc_time * p->Acc_time;
    p->S_acc 	= (uint32_t)((uint64_t)p->Acc * t2 / 2);
    p->S_dec 	= p->S_acc;

    // 判断是否为三角速度曲线
    if ((p->S_acc + p->S_dec) >= s_total)
    {
        // 求三角速度曲线运行时间
        uint64_t temp = ((uint64_t)p->S_total) / p->Acc;
        uint32_t t_new = 0;

        // 当前时间平方 < 三角速度曲线运行时间
        while ((uint64_t)t_new * t_new < temp) ++t_new;

        p->Acc_time = t_new;
        p->Dec_time = t_new;

        // 求加速阶段总位移
        p->S_acc = (uint32_t)((uint64_t)p->Acc * (uint64_t)t_new * t_new / (2 * SCALE));
        // 求减速阶段总位移
        p->S_dec = p->S_acc;
        // 求匀速阶段总位移
        p->S_flat = 0;

        // 求运行时间
        p->Total_time = 2 * t_new;
    }
    else
    {
        p->S_flat = p->S_total - p->S_acc - p->S_dec;
        p->Total_time = p->Acc_time +
                        (p->S_flat / p->V_max) +
                        p->Dec_time;
    }
//    printf("S_total: %u \r\n", p->S_total);
//    printf("V_max:   %u \r\n", p->V_max);
//    printf("Acc:     %u \r\n", p->Acc);
//    printf("Acc_time: %u ticks\r\n", p->Acc_time);
//    printf("Dec_time: %u ticks\r\n", p->Dec_time);
//    printf("S_acc:   %u pulses\r\n", p->S_acc);
//    printf("S_dec:   %u pulses\r\n", p->S_dec);
//    printf("S_flat:  %u pulses\r\n", p->S_flat);
//    printf("Total_time: %u ticks\r\n", p->Total_time);
}

/**
 * @brief 运行时状态更新函数，每次调用对应经过 MILLISECONDE（1 ms），
 *        返回当前累计位移（脉冲 × SCALE）。
 *
 * @param[in,out] p  指向 T_CurveMotion 结构体，包含规划参数和运行时状态
 * @return            累计位移（单位：脉冲 × SCALE）
 *
 * @note
 * - p->Acc_time  单位：毫秒（ms）
 * - p->Dec_time  单位：毫秒（ms）
 * - p->S_acc     单位：脉冲（pulse）
 * - p->S_flat    单位：脉冲（pulse）
 * - p->S_total   单位：脉冲（pulse）
 * - p->V_max     单位：每毫秒脉冲（pulse/ms）× SCALE
 * - p->Acc       单位：每毫秒2脉冲（pulse/ms2）× SCALE
 * - p->Dec       单位：每毫秒2脉冲（pulse/ms2）× SCALE
 * - state 运行时状态：1=加速, 2=匀速, 3=减速, 4=完成
 * - step_count   单位：毫秒（ms），阶段内部计时器
 *
 * 每次调用本函数，相当于时间前进 MILLISECONDE (1 ms)。
 */
 

int16_t MotorController_Update(T_CurveMotion *p)
{
    int64_t step_pos = 0;

    if (!p->busy) return 0;


    switch (p->run_state)
    {
    case 1: // 加速阶段
        step_pos = ((int64_t)p->Acc * p->t_count * p->t_count) / (2 * SCALE);
        p->t_count++;
        if (p->t_count > p->Acc_time)
        {
            p->t_count = 0;
            p->run_state = (p->S_flat > 0) ? 2 : 3;
        }
        break;

    case 2: // 匀速阶段
        step_pos = p->S_acc + (((int64_t)p->V_max * p->t_count) / SCALE);
        p->t_count++;
        if (p->t_count * p->V_max >= (int64_t)p->S_flat * SCALE)
        {
            p->t_count = 0;
            p->run_state = 3;
        }
        break;

    case 3: // 减速阶段
    {
        int64_t t = p->t_count;
        int64_t s_dec = ((int64_t)p->V_max * t - (int64_t)p->Dec * t * t / 2) / SCALE;
        step_pos = p->S_acc + p->S_flat + s_dec;
		
        p->t_count++;
        if (p->t_count >= p->Dec_time)
        {
			p->S_total = step_pos;
            p->run_state = 4;
        }
        break;
    }

    case 4: // 完成阶段
        step_pos = p->S_total;
        p->busy = false;
        break;

    default:
        step_pos = 0;
        break;
    }

    // 应用方向并返回实际脉冲数
    int32_t actual_step = (p->dir == -1 ? -step_pos : step_pos) - p->last_pos;
    p->last_pos += actual_step;
//    printf("发送 %lld 脉冲\r\n", step_pos);
//    printf("发送 %d 脉冲.\r\n", (actual_step));
//    printf("当前 %d 步数\r\n", p->t_count);

    return (actual_step);
}

/**
 * @brief 让指定轴运动到绝对脉冲位置 position_abs，速度 v_max， 加速度 acc。
 * @param axis         要运动的轴编号（0 ~ Max_Axis-1）
 * @param position_abs 目标绝对位置（单位：脉冲）
 * @param v_max        最大速度（脉冲/ms）
 * @param acc          加速度（脉冲/ms2）
 */


void MotorToPulsePosition(uint8_t axis, int32_t position_abs, float v_max, float acc)
{
    if (axis > Max_Axis) return;

    // 备份关键位置变量
    int32_t last_Motor_Nowpos = AxisParameter[axis].Motor_Nowpos;
    int32_t last_start_pos = AxisParameter[axis].start_pos;
    int32_t last_last_pos = AxisParameter[axis].last_pos;

    // 清零结构体（保留关键变量）
    memset(&AxisParameter[axis], 0, sizeof(T_CurveMotion));
    AxisParameter[axis].busy = true;
    AxisParameter[axis].Motor_Nowpos = last_Motor_Nowpos;
    AxisParameter[axis].start_pos = last_start_pos;
    AxisParameter[axis].last_pos = last_last_pos;

    // 获取当前位置并计算位移
    AxisParameter[axis].Motor_Nowpos = inputs[axis]->ActualPosition;
    int32_t delta_pulse = position_abs - AxisParameter[axis].Motor_Nowpos;
    AxisParameter[axis].dir = (delta_pulse >= 0) ? +1 : -1;
    AxisParameter[axis].start_pos = AxisParameter[axis].Motor_Nowpos;

    // 初始化运动曲线
    MotorController_Init(&AxisParameter[axis], fabsf((float)delta_pulse), v_max, acc);

}


/**
 * @brief 对指定电机执行软停（非立即停），进入新的减速轨迹段，安全平滑停下。
 * 
 * @param axis 目标轴号（从 1 开始）
 */
void Motor_SoftStop(uint8_t axis)
{
    T_CurveMotion* p = &AxisParameter[axis];
    if (!p->busy || p->run_state >= 3) return;

    uint32_t t_now = p->t_count;

    switch (p->run_state)
    {
    case 1:
        p->S_acc = 1/2 * p->Acc *t_now * t_now;
		p->S_flat = 0;
        printf("[阶段] 当前为加速阶段（run_state = 1）\r\n");
        break;
    case 2:
        p->S_flat = p->S_acc + (((int64_t)p->V_max * t_now) / SCALE);
        printf("[阶段] 当前为匀速阶段（run_state = 2）\r\n");
        break;
    default:
        printf("[阶段] 非法状态，强制退出软停\r\n");
        return;
    }


    // 设置为当前速度
    int32_t v_now = p->Motor_Nowpos - p->Motor_Lastpos;
    uint32_t abs_v = (v_now >= 0) ? v_now : -v_now;
    p->V_max = abs_v;

    p->t_count = 0;

	// 切换状态为减速阶段
    p->run_state = 3;
		
	printf("当前 t_count       : %lu\r\n", t_now);
    printf("当前 S_acc         : %u pulses\r\n", p->S_acc);
    printf("当前 S_flat        : %u pulses\r\n", p->S_flat);
	
}
/*
void Motor_SoftStop(uint8_t axis)
{
    T_CurveMotion* p = &AxisParameter[axis];
    if (!p->busy || p->run_state >= 3) return;

    printf("\r\n=== [Axis %d] Motor_SoftStop 被调用 ===\r\n", axis);

    uint32_t t_now = p->t_count;
    int64_t step_pos = 0;
    switch (p->run_state)
    {
    case 1:
        step_pos = ((int64_t)p->Acc * t_now * t_now) / (2 * SCALE);
        printf("[阶段] 当前为加速阶段（run_state = 1）\r\n");
        break;
    case 2:
        step_pos = p->S_acc + (((int64_t)p->V_max * t_now) / SCALE);
        printf("[阶段] 当前为匀速阶段（run_state = 2）\r\n");
        break;
    default:
        printf("[阶段] 非法状态，强制退出软停\r\n");
        return;
    }

    if (p->dir == -1) step_pos = -step_pos;
    p->last_pos = step_pos;
    p->step_base = step_pos;  // 新增字段：减速段起点

    int32_t v_now = p->Motor_Nowpos - p->Motor_Lastpos;
    uint32_t abs_v = (v_now >= 0) ? v_now : -v_now;
    p->V_max = abs_v;

    p->Dec_time = (p->V_max + p->Dec / 2) / p->Dec;
    if (p->Dec_time == 0) p->Dec_time = 1;

	p->last_pos = p->S_acc + p->S_flat;
    p->S_total = ((int64_t)p->V_max * p->Dec_time) / 2;

    p->run_state = 3;
    p->t_count = 0;

}
*/





/**
  * 函数功能: ecat循环
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */

void ecat_loop(void)
{

    if (Motor_Run > 0)
    {
        ec_send_processdata();
        ec_receive_processdata(EC_TIMEOUTRET);


        if (Motor_Run > 1)
        {
            for(int axis=1; axis<=ec_slavecount; axis++)
            {
                if (AxisParameter[axis].busy == TRUE)
                {
                    // 先把更新结果存到一个变量里，避免重复调用
                    int32_t delta = MotorController_Update(&AxisParameter[axis]);

					// 记录当前位置作为上个周期的位置
					AxisParameter[axis].Motor_Lastpos = AxisParameter[axis].Motor_Nowpos;
					
                    // 目标位置 = 伺服当前实际位置 + 本周期增量
                    AxisParameter[axis].Motor_Nowpos += delta;

                    outputs[axis]->TargetPosition = AxisParameter[axis].Motor_Nowpos;
//                    printf("Axis %d 周期时间运行 %d.\r\n", axis, (inputs[axis]->ActualPosition - AxisParameter[axis].Motor_Lastpos));                    
                }
            }
        }
    }
}
