#include "System_Command.h"

__attribute__((section(".dma_buffer"))) uint8_t uart_rx_buffer[UART_BUFFER_SIZE];


// ���ַ���ȫ��תΪ��д�����Դ�Сд��
static void to_uppercase(char* str)
{
    while (*str)
    {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

// ��������ָ���ַ������� "M1 GOTO 1000"��
CommandType SerialCmd_ParseLine(const char* line, SerialCommand* cmd)
{
    char buf[MAX_CMD_LENGTH];
    // �����ڽ��յ����� line ���Ƶ� buf����������󳤶ȣ���ֹ���
    strncpy(buf, line, MAX_CMD_LENGTH - 1);
    // ȷ���ַ����� \0 ��β����ֹ�������
    buf[MAX_CMD_LENGTH - 1] = '\0';

    to_uppercase(buf);  // ͳһ��Сд

    // ����һ��ָ������ tokens�����ڴ洢ÿ�����ָ���������ַ�����
    char* tokens[MAX_CMD_TOKENS] = {0};
    // ���ڼ�¼����ȡ�� token �����������ȡ MAX_CMD_TOKENS ����
    int token_count = 0;
    // ʹ�ÿո���Ϊ�ִʵĲ���
    char* token = strtok(buf, " \r\n");
    while (token && token_count < MAX_CMD_TOKENS)
    {
        tokens[token_count++] = token;
		// ������ͬһ�ַ������зָ�
        token = strtok(NULL, " \r\n");
//		printf("tokens: %s.\r\n", tokens[token_count++]);
    }

    // ��ʼ��
    cmd->type = CMD_INVALID;
    cmd->motor_id = -1;
    cmd->target_pos = 0;
    cmd->v_max = 0;
    cmd->acc = 0;

	// 1.���Ƶ���������ո������ٶȡ��ٶ�����ĳ��λ��
	// ָ�����M1 GOTO 1000 V 10 A 10
    if (token_count >= 3 && tokens[0][0] == 'M')
    {
        cmd->motor_id = atoi(tokens[0] + 1);
		
		// �Ƚ��ַ���������
        if (strcmp(tokens[1], "GOTO") == 0)
        {
            cmd->type = CMD_M_GOTO;
            cmd->target_pos = atoi(tokens[2]);
            for (int i = 3; i + 1 < token_count; i++)
            {
                if (strcmp(tokens[i], "V") == 0)
                {
                    cmd->v_max = atoi(tokens[i + 1]);
                }
                else if (strcmp(tokens[i], "A") == 0)
                {
                    cmd->acc = atoi(tokens[i + 1]);
                }
            }
        }
    }
	
	// 2.���Ƶ������ֹͣ��ǰ����
	// ָ�����M1 STOP or M1 STATUS
	// 3.���Ƶ������ֹͣ��ǰ����
	// ָ�����M1 STOP or M1 STATUS
    else if (token_count == 2 && tokens[0][0] == 'M')
    {
        cmd->motor_id = atoi(tokens[0] + 1);
        if (strcmp(tokens[1], "STOP") == 0)
        {
            cmd->type = CMD_M_STOP;
        }
        else if (strcmp(tokens[1], "STATUS") == 0)
        {
            cmd->type = CMD_M_STATUS;
        }
    }
	
	// �������е��ֹͣ��ָ���ʽ�����磺ALL STOP or ALL RESET
    else if (token_count == 2 && strcmp(tokens[0], "ALL") == 0)
    {
        if (strcmp(tokens[1], "STOP") == 0)
        {
            cmd->type = CMD_ALL_STOP;
        }
    }
	
	// ָ���ʽ��HELP
    else if (token_count == 1 && strcmp(tokens[0], "HELP") == 0)
    {
        cmd->type = CMD_HELP;
    }
	// ָ���ʽ��INIT
	else if (token_count == 1 && strcmp(tokens[0], "INIT") == 0)
    {
        cmd->type = CMD_INIT;
    }
    return cmd->type;
}

void SerialCmd_Execute(const SerialCommand* cmd)
{
    switch (cmd->type)
    {
    case CMD_M_GOTO:
        LED2_TOGGLE();
		MotorToPulsePosition(cmd->motor_id, cmd->target_pos, cmd->v_max, cmd->acc);
		// Motor_Goto(cmd->motor_id, cmd->target_pos, cmd->v_max, cmd->acc);
        break;
    case CMD_M_STOP:
        LED3_TOGGLE();
		Motor_SoftStop(cmd->motor_id);
		// Motor_Stop(cmd->motor_id);
        break;
    case CMD_M_STATUS:
		// Motor_Status(cmd->motor_id);
        break;
    case CMD_ALL_STOP:
		for (uint8_t axis = 1; axis <= Max_Axis; axis++)
		{
			Motor_SoftStop(axis);
			HAL_Delay(100);
		}
		// Motor_AllStop();
        break;
    case CMD_HELP:
        printf("֧��ָ��:\r\n");
        printf("  1.���Ƶ���������ո������ٶȡ��ٶ�����ĳ��λ�á�\r\n");
		printf("	ָ�����M1 GOTO 1000 V 10 A 10��\r\n");
        printf("  2.���Ƶ������ֹͣ��ǰ������\r\n");
        printf("	ָ�����M1 STOP or M1 STATUS��\r\n");
        printf("  3.���Ƶ������ֹͣ��ǰ������\r\n");
		printf("	ָ�����M1 STOP or M1 STATUS��\r\n");
        printf("  4.�������е��ֹͣ��\r\n");
        printf("	ָ���ʽ��ALL STOP��\r\n");
		printf("  5.������\r\n");
        printf("	ָ���ʽ��INIT��\r\n");
		printf("  6.������\r\n");
        printf("	ָ���ʽ��HELP��\r\n");
        break;
	case CMD_INIT:
		LED1_TOGGLE();
		MX_ETH_Init();
		MX_TIM4_Init();
		MX_TIM3_Init();
		MX_TIM2_Init();
		PHY_Init();
		__enable_irq();
		ecat_init();
		HAL_TIM_Base_Start_IT(&htim4);
		Zeroer_CSPInit();
        break;
    default:
        printf("δָ֪��\r\n");
        break;
    }
}

/**
 * @brief �������ݴ��������������жϴ�������ã�
 * @param data ���յ�������ָ��
 * @param len  ���ݳ���
 */
void ProcessReceivedData(uint8_t *data, uint16_t len)
{
    if (len == 0 || len > UART_BUFFER_SIZE)
        return;  // ��ֹ�Ƿ����ݴ���
	
	printf("���յ�������: ");
    for (int i = 0; i < len; i++) {
        printf("%c", data[i]);  // ���ַ���ӡ
    }
	printf("\r\n ");
    SerialCommand cmd;
	memset(&cmd, 0, sizeof(SerialCommand));
    if (SerialCmd_ParseLine((const char*)data, &cmd) != CMD_INVALID)
    {
        SerialCmd_Execute(&cmd);
    }
    else
    {
        printf("ָ���ʽ���������� HELP �鿴������\r\n");
    }
}
