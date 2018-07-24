#ifndef _SIM800C_H_
#define _SIM800C_H_

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
/* Define --------------------------------------------------------------------*/
#define true 1
#define false 0
#define bool int
bool retry = false;
char* PhoneNumber = "01216911759";

/*__________________________________________________-*/
bool indexOf(char* String, char* Character);
void SIM_Send(char* cmd);
void SIM_sendCommand(char* cmd);
char* SIM_replyCommand(int delay);
void SIM_DeleteArray();
void SIM_begin();
//GPRS_Function
bool SIM_connectGPRS();
void SIM_disconnectGPRS();
void SIM_readserverResponse();
bool SIM_getValue();
void SIM_getContent();
//SMS_Function
void SIM_deleteMessage();
void SIM_sendSMS();
void SIM_smsFormat();
void SIM_checkAccount();

struct dataGPRS_t{
	char reply[100];
};
char SendSMS = 26;

struct dataGPRS_t dataGPRS;
#endif /* _NEO6M_H_ */
