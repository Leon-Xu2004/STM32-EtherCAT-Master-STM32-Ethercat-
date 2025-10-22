#include "soem.h"

AxisState axis_state[Max_Axis + 1];

char IOmap[4096];
Zeroer_RxPDO *outputs[Max_Axis + 1];
Zeroer_TxPDO *inputs[Max_Axis + 1];
int expectedWKC;

/**
  * 函数功能: 写入8位SDO数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明:
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
  * 函数功能: 写入16位SDO数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明:
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
  * 函数功能: 写入32位SDO数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明:
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
  * 函数功能: Zeroer初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明:
  */
int Zeroer_Nsetup(uint16 slvcnt)
{
    printf("[Zeroer_Nsetup]当前从站地址为：%d.\r\n",slvcnt);

    // ==================== RxPDO 配置 ====================
    write8	(slvcnt, 0x1C12, 00, 0);				     	// 清0x1C12数据
    write8	(slvcnt, 0x1600, 00, 0);				      	// 清0x1600数据
    write32	(slvcnt, 0x1600, 01, 0x60400010);				// 增加0x1600子索引 	Controlword 		控制字
    write32	(slvcnt, 0x1600, 02, 0x60600008);				// 增加0x1600子索引 	OpModeSet 			运行模式
    write32	(slvcnt, 0x1600, 03, 0x60810020);				// 增加0x1600子索引 	ProfileVelocity 	轮廓速度
    write32	(slvcnt, 0x1600, 04, 0x60830020);				// 增加0x1600子索引 	Profileacceleration 轮廓加速度
    write32	(slvcnt, 0x1600, 05, 0x60840020);				// 增加0x1600子索引 	Profiledeceleration 减速度
    write32	(slvcnt, 0x1600, 06, 0x607A0020);				// 增加0x1600子索引 	TargetPosition 		目标位置
    write32	(slvcnt, 0x1600, 07, 0x00000008);				// 增加填充对象			（8 bit，例如0x0000:01h）
    write8	(slvcnt, 0x1600, 00, 7);				    	// 设置子索引数量
    write16	(slvcnt, 0x1C12, 01, 0x1600);			  		// 设置RxPDO映射
    write8	(slvcnt, 0x1C12, 00, 01);

    // ==================== TxPDO 配置 ====================
    write8	(slvcnt, 0x1C13, 00, 00);				      	// 清0x1C13数据
    write8	(slvcnt, 0x1A00, 00, 00);				     	// 清0x1A00数据
    write32	(slvcnt, 0x1A00, 01, 0x60410010);				// 增加0x1A00子索引 	Statusword 		状态字
    write32	(slvcnt, 0x1A00, 02, 0x60610008);				// 增加0x1A00子索引 	Modedisplay 	状态显示
    write32	(slvcnt, 0x1A00, 03, 0x60640020);				// 增加0x1A00子索引		Actualposition 	实际位置
    write32	(slvcnt, 0x1A00, 04, 0x60F40020);				// 增加0x1A00子索引		Errorvalue		错误显示
    write32	(slvcnt, 0x1A00, 05, 0x00000008);				// 增加填充对象			（8 bit，例如0x0000:01h）
    write8	(slvcnt, 0x1A00, 00, 05);				      	// 设置子索引数量
    write16	(slvcnt, 0x1C13, 01, 0x1A00);			  		// 设置TxPDO映射
    write8	(slvcnt, 0x1C13, 00, 01);
    return 0;
}

/**
  * 函数功能: ecat初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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
                    // 零差云控伺服识别（根据实际厂商ID修改）
                    if((ec_slave[slc].eep_man == 0x5a65726f) && (ec_slave[slc].eep_id == 0x29252))
                    {
                        printf("在位置%d发现设备：%s\r\n", slc, ec_slave[slc].name);
                        ec_slave[slc].PO2SOconfig = &Zeroer_Nsetup; // 绑定配置函数
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
  * 函数功能: CSP初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */

void Zeroer_CSPInit()
{
    if (Motor_Run <= 0) return;

    // 新增标志位：检查所有轴是否完成初始化
    int all_axes_initialized = 1;

    // 逐个初始化从站（从站1→从站2→...）
    for (int num = 1; num <= ec_slavecount; num++)
    {
        while (axis_state[num].step < 4)    // 每个从站独立完成0-3步骤
        {
            switch (axis_state[num].step)
            {
            case 0:
                outputs[num]->ControlWord = 0x80;
                outputs[num]->OpModeSet = 0x08;
                printf("从站%d case 0.\r\n", num);
                HAL_Delay(200);
                axis_state[num].step = 1;
                break;
            case 1:
                outputs[num]->ControlWord = 0x06;
                outputs[num]->TargetPosition = inputs[num]->ActualPosition;  // 使用自身位置
                printf("从站%d case 1.\r\n", num);
                HAL_Delay(200);
                axis_state[num].step = 2;
                break;
            case 2:
                outputs[num]->ControlWord = 0x07;
                outputs[num]->TargetPosition = inputs[num]->ActualPosition;
                printf("从站%d case 2.\r\n", num);
                HAL_Delay(200);
                axis_state[num].step = 3;
                break;
            case 3:
                outputs[num]->ControlWord = 0x0f;
                outputs[num]->TargetPosition = inputs[num]->ActualPosition;
                printf("从站%d case 3.\r\n", num);
                printf("从站%d状态字：0x%04X.\r\n", num,inputs[num]->StatusWord);
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
        printf("从站%d初始化完成.\r\n", num);
    }

    /* 检查所有轴状态 */
    for (int num = 1; num <= ec_slavecount; num++)
    {
        if (axis_state[num].step != 4)
        {
            all_axes_initialized = 0;
            break;
        }
    }

    // 所有轴完成后更新状态
    if (all_axes_initialized)
    {
        Motor_Run = 2;
        printf("所有轴初始化完成，进入运行模式！\r\n");
    }
}

void RecoverFromFault(uint8_t axis)
{
    // 先判断从站是否在 OP 状态
    if (ec_slave[axis].state != EC_STATE_OPERATIONAL)
    {
        printf("[Axis %d] ? 不在 OP 状态，跳过故障恢复。\r\n", axis);
        return;
    }

    uint16_t status = inputs[axis]->StatusWord;

    // 检查是否为 Fault 状态（bit3）
    if ((status & 0x08) == 0)
    {
        printf("[Axis %d] ? 没有故障（StatusWord = 0x%04X），无需复位。\r\n", axis, status);
        return;
    }

    printf("[Axis %d] ?? 检测到故障，开始复位...\r\n", axis);

    // Step 0: 故障复位（ControlWord = 0x0080）
    outputs[axis]->ControlWord = 0x0080;
    HAL_Delay(200);

    // 进入标准状态机（等价于 Zeroer_CSPInit）
    axis_state[axis].step = 0;

    while (axis_state[axis].step < 4)
    {
        switch (axis_state[axis].step)
        {
        case 0:
            outputs[axis]->ControlWord = 0x80;
            outputs[axis]->OpModeSet = 0x08;
            printf("从站%d case 0.\r\n", axis);
            HAL_Delay(200);
            axis_state[axis].step = 1;
            break;

        case 1:
            outputs[axis]->ControlWord = 0x06;
            outputs[axis]->TargetPosition = inputs[axis]->ActualPosition;
            printf("从站%d case 1.\r\n", axis);
            HAL_Delay(200);
            axis_state[axis].step = 2;
            break;

        case 2:
            outputs[axis]->ControlWord = 0x07;
            outputs[axis]->TargetPosition = inputs[axis]->ActualPosition;
            printf("从站%d case 2.\r\n", axis);
            HAL_Delay(200);
            axis_state[axis].step = 3;
            break;

        case 3:
            outputs[axis]->ControlWord = 0x0F;
            outputs[axis]->TargetPosition = inputs[axis]->ActualPosition;
            printf("从站%d case 3.\r\n", axis);
            printf("从站%d状态字：0x%04X.\r\n", axis, inputs[axis]->StatusWord);
            HAL_Delay(200);

            if ((inputs[axis]->StatusWord & 0x08) != 0)
            {
                // 复位失败 → 再次尝试复位
                printf("从站%d复位失败，重新尝试...\r\n", axis);
                outputs[axis]->ControlWord = 0x80;
                axis_state[axis].step = 0;
            }
            else
            {
                axis_state[axis].step = 4;
                printf("从站%d已成功重新上电运行。\r\n", axis);
            }
            break;

        default:
            axis_state[axis].step = 0;
            break;
        }
    }
}
