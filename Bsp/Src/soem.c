#include "soem.h"

AxisState axis_state[Max_Axis + 1];

char IOmap[4096];
Zeroer_RxPDO *outputs[Max_Axis + 1];
Zeroer_TxPDO *inputs[Max_Axis + 1];
int expectedWKC;

/**
  * ��������: д��8λSDO����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��:
  */
int write8(uint16  slave, uint16  index, uint8  subindex, int value)
{
    uint8 temp = value;

    int rtn = ec_SDOwrite(slave, index, subindex, FALSE, sizeof(temp), &temp, EC_TIMEOUTRXM );

    if (rtn == 0)
    {
//        printf("SDOwrite to %#x failed !!! \r\n", index);
    }
    else if (DEBUG)
    {
//        printf("SDOwrite to slave%d  index:%#x value:%x Successed !!! \r\n", slave, index, temp);
    }
    return rtn;
}

/**
  * ��������: д��16λSDO����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��:
  */
int write16(uint16  slave, uint16  index, uint8  subindex, int value)
{
    uint16 temp = value;

    int rtn = ec_SDOwrite(slave, index, subindex, FALSE, sizeof(temp), &temp, EC_TIMEOUTRXM * 20);

    if (rtn == 0)
    {
//        printf("SDOwrite to %#x failed !!! \r\n", index);
    }
    else if (DEBUG)
    {
//        printf("SDOwrite to slave%d  index:%#x value:%x Successed !!! \r\n", slave, index, temp);
    }
    return rtn;
}

/**
  * ��������: д��32λSDO����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��:
  */
int write32(uint16 slave, uint16 index, uint8 subindex, int value)
{
    uint32 temp = value;

    int rtn = ec_SDOwrite(slave, index, subindex, FALSE, sizeof(temp), &temp, EC_TIMEOUTRXM * 20);
    if (rtn == 0)
    {
//        printf("SDOwrite to %#x failed !!! \r\n", index);
    }
    else if (DEBUG)
    {
//        printf("SDOwrite to slave%d  index:%#x value:%x Successed !!! \r\n", slave, index, temp);
    }
    return rtn;
}

/**
  * ��������: Zeroer��ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��:
  */
int Zeroer_Nsetup(uint16 slvcnt)
{
    printf("[Zeroer_Nsetup]��ǰ��վ��ַΪ��%d.\r\n",slvcnt);

    // ==================== RxPDO ���� ====================
    write8	(slvcnt, 0x1C12, 00, 0);				     	// ��0x1C12����
    write8	(slvcnt, 0x1600, 00, 0);				      	// ��0x1600����
    write32	(slvcnt, 0x1600, 01, 0x60400010);				// ����0x1600������ 	Controlword 		������
    write32	(slvcnt, 0x1600, 02, 0x60600008);				// ����0x1600������ 	OpModeSet 			����ģʽ
    write32	(slvcnt, 0x1600, 03, 0x60810020);				// ����0x1600������ 	ProfileVelocity 	�����ٶ�
    write32	(slvcnt, 0x1600, 04, 0x60830020);				// ����0x1600������ 	Profileacceleration �������ٶ�
    write32	(slvcnt, 0x1600, 05, 0x60840020);				// ����0x1600������ 	Profiledeceleration ���ٶ�
    write32	(slvcnt, 0x1600, 06, 0x607A0020);				// ����0x1600������ 	TargetPosition 		Ŀ��λ��
    write32	(slvcnt, 0x1600, 07, 0x00000008);				// ����������			��8 bit������0x0000:01h��
    write8	(slvcnt, 0x1600, 00, 7);				    	// ��������������
    write16	(slvcnt, 0x1C12, 01, 0x1600);			  		// ����RxPDOӳ��
    write8	(slvcnt, 0x1C12, 00, 01);

    // ==================== TxPDO ���� ====================
    write8	(slvcnt, 0x1C13, 00, 00);				      	// ��0x1C13����
    write8	(slvcnt, 0x1A00, 00, 00);				     	// ��0x1A00����
    write32	(slvcnt, 0x1A00, 01, 0x60410010);				// ����0x1A00������ 	Statusword 		״̬��
    write32	(slvcnt, 0x1A00, 02, 0x60610008);				// ����0x1A00������ 	Modedisplay 	״̬��ʾ
    write32	(slvcnt, 0x1A00, 03, 0x60640020);				// ����0x1A00������		Actualposition 	ʵ��λ��
    write32	(slvcnt, 0x1A00, 04, 0x60F40020);				// ����0x1A00������		Errorvalue		������ʾ
    write32	(slvcnt, 0x1A00, 05, 0x00000008);				// ����������			��8 bit������0x0000:01h��
    write8	(slvcnt, 0x1A00, 00, 05);				      	// ��������������
    write16	(slvcnt, 0x1C13, 01, 0x1A00);			  		// ����TxPDOӳ��
    write8	(slvcnt, 0x1C13, 00, 01);
    return 0;
}

/**
  * ��������: ecat��ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void ecat_init(void)
{
    int slc;
    /* initialise SOEM, bind socket to ifname */
    if (ec_init("ServoDrive"))
    {
        printf("ec_init succeeded.\r\n");
        if ( ec_config_init(TRUE) > 0 )
        {

            if ( ec_slavecount >= 1 )
            {
                for(slc = 1; slc <= ec_slavecount; slc++)
                {
                    // ����ƿ��ŷ�ʶ�𣨸���ʵ�ʳ���ID�޸ģ�
                    if((ec_slave[slc].eep_man == 0x5a65726f) && (ec_slave[slc].eep_id == 0x29252))
                    {
                        printf("��λ��%d�����豸��%s\r\n", slc, ec_slave[slc].name);
                        ec_slave[slc].PO2SOconfig = &Zeroer_Nsetup; // �����ú���
                    }
                }
            }
            ec_configdc();
            for(slc = 1; slc <= ec_slavecount; slc++)
            {
                ec_dcsync0(slc, TRUE, SYNC0TIME*1000, SHIFTSETTIME); // SYNC0 on slave 1
            }

            ec_config_map(&IOmap);
            printf("Slaves mapped, state to SAFE_OP.\n \r");
            /* wait for all slaves to reach SAFE_OP state */
            ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE);
            /* read indevidual slave state and store in ec_slave[] */
            ec_readstate();
            for(slc = 1; slc <= ec_slavecount; slc++)
            {
                printf("Slave %d State=0x%04x\r\n",slc, ec_slave[slc].state);
            }
            ec_config_map(&IOmap);
            printf("segments : %d : %d %d %d %d\n",ec_group[0].nsegments,ec_group[0].IOsegment[0],ec_group[0].IOsegment[1],ec_group[0].IOsegment[2],ec_group[0].IOsegment[3]);
            ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE);
            printf("Request operational state for all slaves\n");
            expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
            printf("Calculated workcounter %d\n", expectedWKC);
            /* send one valid process data to make outputs in slaves happy*/
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            ec_writestate(0);
            HAL_Delay(100);

            /* wait for all slaves to reach OP state */
            do
            {
                ec_slave[0].state = EC_STATE_OPERATIONAL;
                ec_slave[1].state = EC_STATE_OPERATIONAL;
                ec_slave[2].state = EC_STATE_OPERATIONAL;
                ec_slave[3].state = EC_STATE_OPERATIONAL;
                ec_slave[4].state = EC_STATE_OPERATIONAL;
                ec_slave[5].state = EC_STATE_OPERATIONAL;
                ec_slave[6].state = EC_STATE_OPERATIONAL;
                ec_writestate(0);
                ec_writestate(1);
                ec_writestate(2);
                ec_writestate(3);
                ec_writestate(4);
                ec_writestate(5);
                ec_writestate(6);
            }
            while(	  (ec_slave[0].state != EC_STATE_OPERATIONAL)
                      ||(ec_slave[1].state != EC_STATE_OPERATIONAL)
                      ||(ec_slave[2].state != EC_STATE_OPERATIONAL)
                      ||(ec_slave[3].state != EC_STATE_OPERATIONAL)
                      ||(ec_slave[4].state != EC_STATE_OPERATIONAL)
                      ||(ec_slave[5].state != EC_STATE_OPERATIONAL)
                      ||(ec_slave[6].state != EC_STATE_OPERATIONAL));
            if (ec_slave[0].state == EC_STATE_OPERATIONAL )
            {
                for (slc = 1; slc <= ec_slavecount; slc++)
                {
                    outputs[slc] = (Zeroer_RxPDO *)ec_slave[slc].outputs;
                    inputs[slc]  = (Zeroer_TxPDO *)ec_slave[slc].inputs;
                }

                Motor_Run = 1;
                printf("all slaves reached operational state.\r\n");
            }
            else
            {
                printf("E/BOX not found in slave configuration.\r\n");
            }
        }
        else
        {
            printf("No slaves found!\r\n");
        }
    }
    else
    {
        printf("No socket connection Excecute as root\r\n");
    }

}

/**
  * ��������: CSP��ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */

void Zeroer_CSPInit()
{
    if (Motor_Run <= 0) return;

    // ������־λ������������Ƿ���ɳ�ʼ��
    int all_axes_initialized = 1;

    // �����ʼ����վ����վ1����վ2��...��
    for (int num = 1; num <= ec_slavecount; num++)
    {
        while (axis_state[num].step < 4)    // ÿ����վ�������0-3����
        {
            switch (axis_state[num].step)
            {
            case 0:
                outputs[num]->ControlWord = 0x80;
                outputs[num]->OpModeSet = 0x08;
                printf("��վ%d case 0.\r\n", num);
                HAL_Delay(200);
                axis_state[num].step = 1;
                break;
            case 1:
                outputs[num]->ControlWord = 0x06;
                outputs[num]->TargetPosition = inputs[num]->ActualPosition;  // ʹ������λ��
                printf("��վ%d case 1.\r\n", num);
                HAL_Delay(200);
                axis_state[num].step = 2;
                break;
            case 2:
                outputs[num]->ControlWord = 0x07;
                outputs[num]->TargetPosition = inputs[num]->ActualPosition;
                printf("��վ%d case 2.\r\n", num);
                HAL_Delay(200);
                axis_state[num].step = 3;
                break;
            case 3:
                outputs[num]->ControlWord = 0x0f;
                outputs[num]->TargetPosition = inputs[num]->ActualPosition;
                printf("��վ%d case 3.\r\n", num);
                printf("��վ%d״̬�֣�0x%04X.\r\n", num,inputs[num]->StatusWord);
                HAL_Delay(200);
                if((inputs[num]->StatusWord & 0x08) != 0)
                {
                    outputs[num]->ControlWord = 0x80;
                    axis_state[num].step = 0;
                }
                else
                {
                    axis_state[num].step = 4;
                }
                break;
            default:
                axis_state[num].step = 0;
                break;
            }
        }
        printf("��վ%d��ʼ�����.\r\n", num);
    }

    /* ���������״̬ */
    for (int num = 1; num <= ec_slavecount; num++)
    {
        if (axis_state[num].step != 4)
        {
            all_axes_initialized = 0;
            break;
        }
    }

    // ��������ɺ����״̬
    if (all_axes_initialized)
    {
        Motor_Run = 2;
        printf("�������ʼ����ɣ���������ģʽ��\r\n");
    }
}

void RecoverFromFault(uint8_t axis)
{
    // ���жϴ�վ�Ƿ��� OP ״̬
    if (ec_slave[axis].state != EC_STATE_OPERATIONAL)
    {
        printf("[Axis %d] ? ���� OP ״̬���������ϻָ���\r\n", axis);
        return;
    }

    uint16_t status = inputs[axis]->StatusWord;

    // ����Ƿ�Ϊ Fault ״̬��bit3��
    if ((status & 0x08) == 0)
    {
        printf("[Axis %d] ? û�й��ϣ�StatusWord = 0x%04X�������踴λ��\r\n", axis, status);
        return;
    }

    printf("[Axis %d] ?? ��⵽���ϣ���ʼ��λ...\r\n", axis);

    // Step 0: ���ϸ�λ��ControlWord = 0x0080��
    outputs[axis]->ControlWord = 0x0080;
    HAL_Delay(200);

    // �����׼״̬�����ȼ��� Zeroer_CSPInit��
    axis_state[axis].step = 0;

    while (axis_state[axis].step < 4)
    {
        switch (axis_state[axis].step)
        {
        case 0:
            outputs[axis]->ControlWord = 0x80;
            outputs[axis]->OpModeSet = 0x08;
            printf("��վ%d case 0.\r\n", axis);
            HAL_Delay(200);
            axis_state[axis].step = 1;
            break;

        case 1:
            outputs[axis]->ControlWord = 0x06;
            outputs[axis]->TargetPosition = inputs[axis]->ActualPosition;
            printf("��վ%d case 1.\r\n", axis);
            HAL_Delay(200);
            axis_state[axis].step = 2;
            break;

        case 2:
            outputs[axis]->ControlWord = 0x07;
            outputs[axis]->TargetPosition = inputs[axis]->ActualPosition;
            printf("��վ%d case 2.\r\n", axis);
            HAL_Delay(200);
            axis_state[axis].step = 3;
            break;

        case 3:
            outputs[axis]->ControlWord = 0x0F;
            outputs[axis]->TargetPosition = inputs[axis]->ActualPosition;
            printf("��վ%d case 3.\r\n", axis);
            printf("��վ%d״̬�֣�0x%04X.\r\n", axis, inputs[axis]->StatusWord);
            HAL_Delay(200);

            if ((inputs[axis]->StatusWord & 0x08) != 0)
            {
                // ��λʧ�� �� �ٴγ��Ը�λ
                printf("��վ%d��λʧ�ܣ����³���...\r\n", axis);
                outputs[axis]->ControlWord = 0x80;
                axis_state[axis].step = 0;
            }
            else
            {
                axis_state[axis].step = 4;
                printf("��վ%d�ѳɹ������ϵ����С�\r\n", axis);
            }
            break;

        default:
            axis_state[axis].step = 0;
            break;
        }
    }
}
