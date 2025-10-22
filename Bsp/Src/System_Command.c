#include "System_Command.h"

__attribute__((section(".dma_buffer"))) uint8_t uart_rx_buffer[UART_BUFFER_SIZE];


// 将字符串全部转为大写（忽略大小写）
static void to_uppercase(char* str)
{
    while (*str)
    {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

// 解析串口指令字符串（如 "M1 GOTO 1000"）
CommandType SerialCmd_ParseLine(const char* line, SerialCommand* cmd)
{
    char buf[MAX_CMD_LENGTH];
    // 将串口接收的命令 line 复制到 buf，并限制最大长度，防止溢出
    strncpy(buf, line, MAX_CMD_LENGTH - 1);
    // 确保字符串以 \0 结尾，防止溢出访问
    buf[MAX_CMD_LENGTH - 1] = '\0';

    to_uppercase(buf);  // 统一大小写

    // 定义一个指针数组 tokens，用于存储每个被分割出来的子字符串。
    char* tokens[MAX_CMD_TOKENS] = {0};
    // 用于记录已提取的 token 个数，最多提取 MAX_CMD_TOKENS 个。
    int token_count = 0;
    // 使用空格作为分词的操作
    char* token = strtok(buf, " \r\n");
    while (token && token_count < MAX_CMD_TOKENS)
    {
        tokens[token_count++] = token;
		// 继续对同一字符串进行分割
        token = strtok(NULL, " \r\n");
//		printf("tokens: %s.\r\n", tokens[token_count++]);
    }

    // 初始化
    cmd->type = CMD_INVALID;
    cmd->motor_id = -1;
    cmd->target_pos = 0;
    cmd->v_max = 0;
    cmd->acc = 0;

	// 1.控制单个电机按照给定加速度、速度运行某个位置
	// 指令案例：M1 GOTO 1000 V 10 A 10
    if (token_count >= 3 && tokens[0][0] == 'M')
    {
        cmd->motor_id = atoi(tokens[0] + 1);
		
		// 比较字符串的内容
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
	
	// 2.控制单个电机停止当前操作
	// 指令案例：M1 STOP or M1 STATUS
	// 3.控制单个电机停止当前操作
	// 指令案例：M1 STOP or M1 STATUS
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
	
	// 控制所有电机停止，指令格式，例如：ALL STOP or ALL RESET
    else if (token_count == 2 && strcmp(tokens[0], "ALL") == 0)
    {
        if (strcmp(tokens[1], "STOP") == 0)
        {
            cmd->type = CMD_ALL_STOP;
        }
    }
	
	// 指令格式，HELP
    else if (token_count == 1 && strcmp(tokens[0], "HELP") == 0)
    {
        cmd->type = CMD_HELP;
    }
	// 指令格式，INIT
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
        printf("支持指令:\r\n");
        printf("  1.控制单个电机按照给定加速度、速度运行某个位置。\r\n");
		printf("	指令案例：M1 GOTO 1000 V 10 A 10。\r\n");
        printf("  2.控制单个电机停止当前操作。\r\n");
        printf("	指令案例：M1 STOP or M1 STATUS。\r\n");
        printf("  3.控制单个电机停止当前操作。\r\n");
		printf("	指令案例：M1 STOP or M1 STATUS。\r\n");
        printf("  4.控制所有电机停止。\r\n");
        printf("	指令格式：ALL STOP。\r\n");
		printf("  5.开机。\r\n");
        printf("	指令格式：INIT。\r\n");
		printf("  6.帮助。\r\n");
        printf("	指令格式：HELP。\r\n");
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
        printf("未知指令\r\n");
        break;
    }
}

/**
 * @brief 串口数据处理函数（被空闲中断触发后调用）
 * @param data 接收到的数据指针
 * @param len  数据长度
 */
void ProcessReceivedData(uint8_t *data, uint16_t len)
{
    if (len == 0 || len > UART_BUFFER_SIZE)
        return;  // 防止非法数据处理
	
	printf("接收到的数据: ");
    for (int i = 0; i < len; i++) {
        printf("%c", data[i]);  // 按字符打印
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
        printf("指令格式错误！请输入 HELP 查看帮助。\r\n");
    }
}
