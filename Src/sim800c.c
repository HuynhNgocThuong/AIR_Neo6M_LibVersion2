#include "..\Inc\sim800c.h"
/*Check character in string*/
bool indexOf(char* String, char* Character){
	char *p = strstr(String, Character);
	if(p == NULL){
		return false;
	}
	else{
		return true;
	}
}
/* Sim Send*/
void SIM_Send(char* cmd){
	int i = strlen(cmd);
	HAL_UART_Transmit(&huart2,(uint8_t *) cmd, i, 1000);
}
/* SIM Send AT Command*/
void SIM_sendCommand(char* cmd) {
  SIM_Send(cmd);
  SIM_Send("\r\n");
}
/* Sim Send Reply Command*/
char* SIM_replyCommand(int delay){
		HAL_UART_Receive(&huart2, (uint8_t *)dataGPRS.reply, 100, 1000);
		printf("%s\n",dataGPRS.reply);
		HAL_Delay(delay);
	return dataGPRS.reply;
}
/* Delete Array*/
void SIM_DeleteArray(){
/* _____________________________SIM_GPRS_________________________*/
	int length = strlen(dataGPRS.reply);
	for(int i = 0; i<length; i++){
		dataGPRS.reply[i] = 0;
	}
}
/* _____________________________SIM_GPRS_________________________*/
bool SIM_connectGPRS(){
	
	SIM_sendCommand("AT+SAPBR=1,1");
	if(indexOf(SIM_replyCommand(1000),"ERROR")) return false;
	SIM_DeleteArray();
	
	SIM_sendCommand("AT+SAPBR=2,1");
	if(!(indexOf(SIM_replyCommand(1000),"1,1"))) return false;
	SIM_DeleteArray();

	SIM_sendCommand("AT+HTTPINIT");
	if(indexOf(SIM_replyCommand(1000),"ERROR")) return false;
	SIM_DeleteArray();

	SIM_sendCommand("AT+HTTPPARA=CID,1");
	if(indexOf(SIM_replyCommand(1000),"ERROR")) return false;
	SIM_DeleteArray();
	return true;
}

void SIM_disconnectGPRS(){
	SIM_sendCommand("AT+HTTPTERM");
	SIM_replyCommand(500);
	SIM_DeleteArray();
	SIM_sendCommand("AT+SAPBR=0,1");
	SIM_replyCommand(500);
	SIM_DeleteArray();
}

void SIM_readserverResponse(){
	SIM_sendCommand("AT+HTTPREAD");
	SIM_replyCommand(1000);
}


void SIM_getContent(char* _Parameter){
		/*______________GET_______________*/
	SIM_Send("AT+HTTPPARA=URL,http://www.vmig2016.16mb.com/get_location.php?raw_var=");
	SIM_sendCommand(_Parameter);
	SIM_replyCommand(500);
	SIM_DeleteArray();
	
	SIM_sendCommand("AT+HTTPACTION=0");
	SIM_replyCommand(5000);
	SIM_DeleteArray();
	
	SIM_sendCommand("AT+HTTPREAD");
	SIM_replyCommand(500);
	
	printf("-----------------RETRY (before HTTPACTION):  ");
	char* r = retry == true ? "true-----" : "false-----";
	printf("%s\n",r);
	
		 
	if(indexOf(dataGPRS.reply, "httpaction") || indexOf(dataGPRS.reply, "error")){
		printf("Retry later!\n");
    retry = true;
	};
}
bool SIM_getValue(){
	printf("-------------------- -Start g----------------------\n");
  char check_connect = 0;
  while (!SIM_connectGPRS()) { 
     check_connect++;
     SIM_begin();
      if(check_connect==3){ 
		//	SIM_sendSMS(PhoneNumber, "SYNTAX ERROR! \n ID: \""+ (String)measuring_point_id+"\"\n Sim800 connect GPRS fail !! \n check your module:\n ");
			return false;
			}
			
//	String date_of_value = (String)yearr+"-"+(String)mon +"-" +(String)dayy ;
//  Serial.println(date_of_value);
//  String hour_of_value = (String)hh1+":"+(String)mm1 +":" +(String)ss1;;
//  Serial.println(hour_of_value);
//  SIM_getContent( "action=index&measuring_point_id=" + (String)measuring_point_id + "&date="+ date_of_value + "&time="+ hour_of_value+ "&value1=" + (String)temp1 + "&value2=" + (String)temp2 + "&value3=" + (String)temp3 + "&account="+ account + "&battery="+(String)battery);
//  SIM_readserverResponse();
  SIM_disconnectGPRS();
  SIM_smsFormat();
  printf("-------------------- -Get finished---------------------- -");
return true;
			
}
	
}
/* _____________________________SIM_SMS_________________________*/
void SIM_deleteMessage() {
  SIM_sendCommand("AT+CMGD=1,4"); //delete all message on SIM
  SIM_replyCommand(100);
	SIM_DeleteArray();
}
void SIM_smsFormat() {
  SIM_sendCommand("AT+CSCS=\"GSM\""); //Chon che do GSM
  SIM_replyCommand(100);
	SIM_DeleteArray();
  SIM_sendCommand("AT+CMGF=1");
  SIM_replyCommand(100);
	SIM_DeleteArray();
  SIM_sendCommand("ATE1");//debug respone
  SIM_replyCommand(100);
  SIM_DeleteArray();
}

void SIM_sendSMS(char* targetNumber, char* message){
	
	printf("|%s|\n|%s|", targetNumber, message);
//	SIM_Send("AT+CMGS=\"");
//	SIM_Send(targetNumber);
//	SIM_Send("\"\r\n");
	SIM_sendCommand("AT+CMGS=\"01216911759\"");
//	SIM_Send("AT+CMGS=\"");
//	SIM_Send(targetNumber);
//	SIM_Send("\"\r\n");
	HAL_Delay(500);
	SIM_sendCommand(message);
	HAL_Delay(300);
	HAL_UART_Transmit(&huart2,(uint8_t *)&SendSMS, 1, 1000);
	HAL_Delay(300);
	HAL_UART_Transmit(&huart2,(uint8_t *)&SendSMS, 1, 1000);
	
}
/*______________________________SIM_ACCOUNT____________________*/
void SIM_checkAccount(){
	printf("Check Account\n");
	do{
			SIM_sendCommand("AT+CUSD=1,\"*101#\"");

		  if(indexOf(SIM_replyCommand(300),"ERROR")) {
      printf("-------------------------> Reset Module SIM-----");
      //SIM_sendCommand("AT+CFUN=1,1"); //Reset luon;
      SIM_begin();
    }
	}while(!(indexOf(SIM_replyCommand(300),"+CUSD")));
	SIM_replyCommand(300);
	SIM_DeleteArray();
	SIM_sendCommand("AT+CUSD=0");
	
//	account = getUSSDMessage(data);
//  SIM_sendCommand("AT+CUSD=0");
//  SIM_buffer_flush();
//  delay(100);
}

/* _____________________________SIM_BEGIN________________________ */
void SIM_begin(){
	HAL_Delay(1000);
	do{
		SIM_sendCommand("AT+CREG?");	
	}while(!(indexOf(SIM_replyCommand(500),"0,1")));
	SIM_DeleteArray();
	//Turn off USSD
	SIM_sendCommand("AT+CUSD=0");
	SIM_replyCommand(100);
	SIM_DeleteArray();
	
	SIM_deleteMessage();
	
	SIM_sendCommand("AT&F0");
	SIM_replyCommand(100);
	SIM_DeleteArray();
	
	SIM_sendCommand("AT+CMEE=2");
	SIM_replyCommand(100);
	SIM_DeleteArray();
	
	SIM_sendCommand("AT+SAPBR=3,1,CONTYPE,GPRS");
	SIM_replyCommand(300);
	SIM_DeleteArray();
	
	SIM_sendCommand("AT+SAPBR=3,1,APN,internet");
	SIM_replyCommand(300);
	SIM_DeleteArray();
	
	SIM_smsFormat();
}

