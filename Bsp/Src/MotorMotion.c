#include "MotorMotion.h"

int Motor_Run;

/**
 * @brief ��ʼ�����������ٶ����߲�������������״̬
 *sssssssssssss
 * ��������������ġ���λ�ơ�����ٶȡ����ٶȡ�ת��Ϊ����������������
 * �����ٽ׶Ρ����ٽ׶Ρ����ٽ׶εĳ���ʱ��͸���λ�ƣ��ѷŴ� SCALE ������
 * ͬʱ��������ʱ״̬��ʹ�ú���������ÿ�����ڵ��� @c MotorionController_Update
 * �������ʵʱ���������
 *
 * @param[in,out] p         Ŀ��������߲����ṹ��
 * @param[in]     s_total   ��λ�ƣ���λ�����壨pulse��
 * @param[in]     v_max     ����ٶȣ���λ������/���루pulse/ms��
 * @param[in]     acc       ���ٶȣ���λ������/����2��pulse/ms2��
 *
 * @note
 * - p->S_total ��uint32_t��  ��  �洢��λ = ���� �� SCALE
 * - p->V_max   ��uint32_t��  ��  �洢��λ = (����/ms) �� SCALE
 * - p->Acc     ��uint32_t��  ��  �洢��λ = (����/ms2) �� SCALE
 * - p->Dec     ��uint32_t��  ��  �洢��λ = (����/ms2) �� SCALE���Գƣ�
 * - p->Acc_time��uint32_t��  ��  ���ٽ׶γ���ʱ�䣬��λ�����루ms��
 * - p->Dec_time��uint32_t��  ��  ���ٽ׶γ���ʱ�䣬��λ�����루ms��
 * - p->S_acc   ��uint32_t��  ��  ���ٽ׶ν����ۼ�λ�ƣ���λ������
 * - p->S_dec   ��uint32_t��  ��  ���ٽ׶ο�ʼǰ�ۼ�λ�ƣ���λ������
 * - p->S_flat  ��uint32_t��  ��  ���ٽ׶���λ�ƣ���λ������
 * - p->Total_time��uint32_t����  �����˶��ܺ�ʱ����λ�����루ms��
 * - p->run_state��uint8_t��  ��  ����ʱ״̬ 1=����, 2=����, 3=����, 4=���
 * - p->t_count ��uint32_t��  ��  ��ǰ�׶�����ʱ�䣬��λ�����루ms��
 * - p->start_pos��int32_t��  ��  �˶���ʼ����λ�ã���λ������
 * - p->dir     ��int32_t��   ��  �˶�����+1 �� -1���޵�λ
 * - p->busy    ��bool��	  ��  �˶�æ��־���޵�λ
 */
void MotorController_Init(T_CurveMotion* p,
                          float s_total,
                          float v_max,
                          float acc)
{

    // ��ʼ���˶���ز���
    p->run_state = 1;
    p->t_count   = 0;
    p->busy 	 = true;
    p->last_pos  = 0;


    // תΪ��������ֵ���� SCALE��
    p->S_total  = (uint32_t)(s_total + 0.5f);
    p->V_max    = (uint32_t)(v_max   + 0.5f);
    p->Acc      = (uint32_t)(acc     + 0.5f);
    p->Dec      = p->Acc;

    // �Ӽ���ʱ��
    p->Acc_time = (p->V_max + p->Acc / 2) / p->Acc;
    p->Dec_time = p->Acc_time;

    // �Ӽ���λ��
    uint64_t t2 = (uint64_t)p->Acc_time * p->Acc_time;
    p->S_acc 	= (uint32_t)((uint64_t)p->Acc * t2 / 2);
    p->S_dec 	= p->S_acc;

    // �ж��Ƿ�Ϊ�����ٶ�����
    if ((p->S_acc + p->S_dec) >= s_total)
    {
        // �������ٶ���������ʱ��
        uint64_t temp = ((uint64_t)p->S_total) / p->Acc;
        uint32_t t_new = 0;

        // ��ǰʱ��ƽ�� < �����ٶ���������ʱ��
        while ((uint64_t)t_new * t_new < temp) ++t_new;

        p->Acc_time = t_new;
        p->Dec_time = t_new;

        // ����ٽ׶���λ��
        p->S_acc = (uint32_t)((uint64_t)p->Acc * (uint64_t)t_new * t_new / (2 * SCALE));
        // ����ٽ׶���λ��
        p->S_dec = p->S_acc;
        // �����ٽ׶���λ��
        p->S_flat = 0;

        // ������ʱ��
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
 * @brief ����ʱ״̬���º�����ÿ�ε��ö�Ӧ���� MILLISECONDE��1 ms����
 *        ���ص�ǰ�ۼ�λ�ƣ����� �� SCALE����
 *
 * @param[in,out] p  ָ�� T_CurveMotion �ṹ�壬�����滮����������ʱ״̬
 * @return            �ۼ�λ�ƣ���λ������ �� SCALE��
 *
 * @note
 * - p->Acc_time  ��λ�����루ms��
 * - p->Dec_time  ��λ�����루ms��
 * - p->S_acc     ��λ�����壨pulse��
 * - p->S_flat    ��λ�����壨pulse��
 * - p->S_total   ��λ�����壨pulse��
 * - p->V_max     ��λ��ÿ�������壨pulse/ms���� SCALE
 * - p->Acc       ��λ��ÿ����2���壨pulse/ms2���� SCALE
 * - p->Dec       ��λ��ÿ����2���壨pulse/ms2���� SCALE
 * - state ����ʱ״̬��1=����, 2=����, 3=����, 4=���
 * - step_count   ��λ�����루ms�����׶��ڲ���ʱ��
 *
 * ÿ�ε��ñ��������൱��ʱ��ǰ�� MILLISECONDE (1 ms)��
 */
 

int16_t MotorController_Update(T_CurveMotion *p)
{
    int64_t step_pos = 0;

    if (!p->busy) return 0;


    switch (p->run_state)
    {
    case 1: // ���ٽ׶�
        step_pos = ((int64_t)p->Acc * p->t_count * p->t_count) / (2 * SCALE);
        p->t_count++;
        if (p->t_count > p->Acc_time)
        {
            p->t_count = 0;
            p->run_state = (p->S_flat > 0) ? 2 : 3;
        }
        break;

    case 2: // ���ٽ׶�
        step_pos = p->S_acc + (((int64_t)p->V_max * p->t_count) / SCALE);
        p->t_count++;
        if (p->t_count * p->V_max >= (int64_t)p->S_flat * SCALE)
        {
            p->t_count = 0;
            p->run_state = 3;
        }
        break;

    case 3: // ���ٽ׶�
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

    case 4: // ��ɽ׶�
        step_pos = p->S_total;
        p->busy = false;
        break;

    default:
        step_pos = 0;
        break;
    }

    // Ӧ�÷��򲢷���ʵ��������
    int32_t actual_step = (p->dir == -1 ? -step_pos : step_pos) - p->last_pos;
    p->last_pos += actual_step;
//    printf("���� %lld ����\r\n", step_pos);
//    printf("���� %d ����.\r\n", (actual_step));
//    printf("��ǰ %d ����\r\n", p->t_count);

    return (actual_step);
}

/**
 * @brief ��ָ�����˶�����������λ�� position_abs���ٶ� v_max�� ���ٶ� acc��
 * @param axis         Ҫ�˶������ţ�0 ~ Max_Axis-1��
 * @param position_abs Ŀ�����λ�ã���λ�����壩
 * @param v_max        ����ٶȣ�����/ms��
 * @param acc          ���ٶȣ�����/ms2��
 */


void MotorToPulsePosition(uint8_t axis, int32_t position_abs, float v_max, float acc)
{
    if (axis > Max_Axis) return;

    // ���ݹؼ�λ�ñ���
    int32_t last_Motor_Nowpos = AxisParameter[axis].Motor_Nowpos;
    int32_t last_start_pos = AxisParameter[axis].start_pos;
    int32_t last_last_pos = AxisParameter[axis].last_pos;

    // ����ṹ�壨�����ؼ�������
    memset(&AxisParameter[axis], 0, sizeof(T_CurveMotion));
    AxisParameter[axis].busy = true;
    AxisParameter[axis].Motor_Nowpos = last_Motor_Nowpos;
    AxisParameter[axis].start_pos = last_start_pos;
    AxisParameter[axis].last_pos = last_last_pos;

    // ��ȡ��ǰλ�ò�����λ��
    AxisParameter[axis].Motor_Nowpos = inputs[axis]->ActualPosition;
    int32_t delta_pulse = position_abs - AxisParameter[axis].Motor_Nowpos;
    AxisParameter[axis].dir = (delta_pulse >= 0) ? +1 : -1;
    AxisParameter[axis].start_pos = AxisParameter[axis].Motor_Nowpos;

    // ��ʼ���˶�����
    MotorController_Init(&AxisParameter[axis], fabsf((float)delta_pulse), v_max, acc);

}


/**
 * @brief ��ָ�����ִ����ͣ��������ͣ���������µļ��ٹ켣�Σ���ȫƽ��ͣ�¡�
 * 
 * @param axis Ŀ����ţ��� 1 ��ʼ��
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
        printf("[�׶�] ��ǰΪ���ٽ׶Σ�run_state = 1��\r\n");
        break;
    case 2:
        p->S_flat = p->S_acc + (((int64_t)p->V_max * t_now) / SCALE);
        printf("[�׶�] ��ǰΪ���ٽ׶Σ�run_state = 2��\r\n");
        break;
    default:
        printf("[�׶�] �Ƿ�״̬��ǿ���˳���ͣ\r\n");
        return;
    }


    // ����Ϊ��ǰ�ٶ�
    int32_t v_now = p->Motor_Nowpos - p->Motor_Lastpos;
    uint32_t abs_v = (v_now >= 0) ? v_now : -v_now;
    p->V_max = abs_v;

    p->t_count = 0;

	// �л�״̬Ϊ���ٽ׶�
    p->run_state = 3;
		
	printf("��ǰ t_count       : %lu\r\n", t_now);
    printf("��ǰ S_acc         : %u pulses\r\n", p->S_acc);
    printf("��ǰ S_flat        : %u pulses\r\n", p->S_flat);
	
}
/*
void Motor_SoftStop(uint8_t axis)
{
    T_CurveMotion* p = &AxisParameter[axis];
    if (!p->busy || p->run_state >= 3) return;

    printf("\r\n=== [Axis %d] Motor_SoftStop ������ ===\r\n", axis);

    uint32_t t_now = p->t_count;
    int64_t step_pos = 0;
    switch (p->run_state)
    {
    case 1:
        step_pos = ((int64_t)p->Acc * t_now * t_now) / (2 * SCALE);
        printf("[�׶�] ��ǰΪ���ٽ׶Σ�run_state = 1��\r\n");
        break;
    case 2:
        step_pos = p->S_acc + (((int64_t)p->V_max * t_now) / SCALE);
        printf("[�׶�] ��ǰΪ���ٽ׶Σ�run_state = 2��\r\n");
        break;
    default:
        printf("[�׶�] �Ƿ�״̬��ǿ���˳���ͣ\r\n");
        return;
    }

    if (p->dir == -1) step_pos = -step_pos;
    p->last_pos = step_pos;
    p->step_base = step_pos;  // �����ֶΣ����ٶ����

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
  * ��������: ecatѭ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
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
                    // �ȰѸ��½���浽һ������������ظ�����
                    int32_t delta = MotorController_Update(&AxisParameter[axis]);

					// ��¼��ǰλ����Ϊ�ϸ����ڵ�λ��
					AxisParameter[axis].Motor_Lastpos = AxisParameter[axis].Motor_Nowpos;
					
                    // Ŀ��λ�� = �ŷ���ǰʵ��λ�� + ����������
                    AxisParameter[axis].Motor_Nowpos += delta;

                    outputs[axis]->TargetPosition = AxisParameter[axis].Motor_Nowpos;
//                    printf("Axis %d ����ʱ������ %d.\r\n", axis, (inputs[axis]->ActualPosition - AxisParameter[axis].Motor_Lastpos));                    
                }
            }
        }
    }
}
