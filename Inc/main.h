#ifndef __MAIN_H__
#define __MAIN_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/

/* ���Ͷ��� ------------------------------------------------------------------*/

/* �궨�� --------------------------------------------------------------------*/

#define USE_DHCP       /* �Ƿ�ʹ��DHCP */

/* ��̬IP��ַ���� */
#define IP_ADDR0   (uint8_t) 192
#define IP_ADDR1   (uint8_t) 168
#define IP_ADDR2   (uint8_t) 1
#define IP_ADDR3   (uint8_t) 28
   
/* ��̬�������� */
#define NETMASK_ADDR0   (uint8_t) 255
#define NETMASK_ADDR1   (uint8_t) 255
#define NETMASK_ADDR2   (uint8_t) 255
#define NETMASK_ADDR3   (uint8_t) 0

/* ��̬Ĭ������ */
#define GW_ADDR0   (uint8_t) 192
#define GW_ADDR1   (uint8_t) 168
#define GW_ADDR2   (uint8_t) 1
#define GW_ADDR3   (uint8_t) 1

#define TCP_SERVER_RX_BUFSIZE	200	 //����tcp server���������ݳ���
#define TCP_SERVER_PORT			1234	 //����tcp server�Ķ˿�

/* ��չ���� ------------------------------------------------------------------*/

/* �������� ------------------------------------------------------------------*/

#endif /* __MAIN_H__ */

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

