/*
	 HUYNH NGOC THUONG
	 DA NANG UNIVERSITY OF SCIENCE AND TECHNOLOGY
	 Last modified: 1/05/2018 - Version 2
	 **Describe: 
	 +SDcard by module SD and STM32F103C8T6
	 +In this project i had used: 
	 +Thirt party: FreeRTOS
	 +Thirt party: FATFS
	 +Comunicate protocol: UART1 - Rx for receive data from GPS Neo-Ulbox
												 UART2 - Tx for transfer data from MCU to PC for observation
 *Facebook: ngocthuong0208 - huynhngocthuong0208@gmail.com - 01216911759
*/
#ifndef _GPSNEO_H_
#define _GPSNEO_H_
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//--------------------------------------Define
#define GPS_BUFFER_SIZE 200
#define GPS_VTG_SIZE 50
//----------------------------
/*_______________________________GPS_Struct_____________________________________*/
struct nmeaMessage_t{
	//Raw Data GPS
	char GPS_RX_byte[2];
	char GPS_Transfer_cplt;
	char GPS_RX_Buffer[GPS_BUFFER_SIZE];
	char GPS_VTG_Buffer[GPS_VTG_SIZE];
	uint8_t GPS_Counter;
	uint8_t GPS_Counter_Tmp;
	uint8_t GPS_Flag;
	
	uint8_t GPS_SCounter;
	uint8_t GPS_SCounter_Tmp;
	uint8_t GPS_SFlag;
};
struct dataGps_t{
		//Data GPS
	char Time[20];
	char Status[2];
	char Latitude[9];
	char S_N[2];
	char Longtitude[10];
	char E_W[2];
	char Speed[20];
	char Dir[20];
	char Date[20];
};
struct statusGps_t{
	unsigned char GPS_ans_stt;
	unsigned char GPS_send_error;
	unsigned char GPS_receive_error;
};
struct point_t {
  double X;       // kinh do
  double Y;       // vi do
};

/*________________________________GPS_Prototype_________________________________*/
int Search_Char(unsigned char Char, char *Str, unsigned char Time, int Len);
unsigned char GPS_DeviceInfo(char* time, char* status, char* latitude, char* S_N, 
														 char* longitude, char* E_W, char* speed, char* dir, char* date);
void CLEAR_GPS_RX_Buffer(void);
void Processing_$GPGRMC(void);
void Processing_$GPGVTG(void);
void Delete_Char(char s[], int pos);
void GPS_USART_RX_ISR(void);
#endif /* _PMS7003_H_ */