#include "gpsneo.h"
//------------------------------------Define-----------------------------------//
 //---------------------------------------------------------------------------//
#define true 1
#define false 0
#define bool int
//-----------------------------------Variable----------------------------------//
 //---------------------------------------------------------------------------//
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
char ch; 
//Khai bao struct
struct nmeaMessage_t 	nmeaMessage;
struct dataGps_t 			dataGps;
struct statusGps_t		statusGps;
// Ham xoa ki tu trong chuoi
void GPS_DeleteChar(char s[], int pos){
	int n = strlen(s); 
	for(int i = pos + 1; i < n; i++)
	{
		s[i - 1] = s[i];
	}
	// Ki tu ket thuc
	s[strlen(s) - 1] = '\0';
}
/**
  * @brief  Ham tim kiem chuoi ki tu
	* @param  Char: Ki tu can tim
	*					Str: Chuoi chua ki tu can tim
	*					Time: So lan lap lai ki tu trong chuoi
	*					Len: Chieu rong cua chuoi
  *                 
  * @retval Khong
  */
int GPS_SearchChar(unsigned char Char, char *Str, unsigned char Time, int Len){
    int  i=0;
    int  j=0;
    while((j<Time)&&(i<=Len))
    {
        if(Str[i] == Char)    j++;  
        i++;  
    }
    return i;
}
/**
  * @brief  Ham xoa bo dem Rx
	* @param  Khong
  *                 
  * @retval Khong
  */
void GPS_ClearRxBuffer(void){
	for (int j=0; j<GPS_BUFFER_SIZE; j++)
	nmeaMessage.GPS_RX_Buffer[j] = 0; //clear Rx_Buffer before receiving new data
	}
void GPS_ClearData(void){
	for(int j=0; j< strlen(dataGps.Latitude); j++)
	dataGps.Latitude[j] = 0;					//clear latitude buffer
	for(int j=0; j< strlen(dataGps.Longtitude); j++)
	dataGps.Longtitude[j] = 0;				//clear longtitude buffer
	for(int j=0; j< strlen(dataGps.Speed); j++)
	dataGps.Speed[j] = 0;							//clear speed buffer
}
/**
  * @brief  Ham xoa bo dem Rx
	* @param  Khong
  *                 
  * @retval Khong
  */
unsigned char GPS_Data(char* time, char* status, char* latitude, char* S_N, char* longitude, char* E_W, char* speed, char* date){
  int i = 0;
	int k = 0;
	int Temp1, Temp2;
	Temp2 = GPS_SearchChar(',',nmeaMessage.GPS_RX_Buffer,2,GPS_BUFFER_SIZE);	
	if(nmeaMessage.GPS_RX_Buffer[Temp2] == 'V'){
		return 0;
	}
	else{
//-------------------------------------------------------------------------------------------------		
    //LATITUDE
		Temp1 = GPS_SearchChar(',',nmeaMessage.GPS_RX_Buffer,3,GPS_BUFFER_SIZE);	 
		Temp2 = GPS_SearchChar(',',nmeaMessage.GPS_RX_Buffer,4,GPS_BUFFER_SIZE);	
		k = 0;
		for(i = Temp1; i < Temp2-1; i++){
			dataGps.Latitude[k++] = nmeaMessage.GPS_RX_Buffer[i];
		}
		if(nmeaMessage.GPS_RX_Buffer[Temp2]=='N'){
			dataGps.S_N[0] = 'N';
		}
		else{
			dataGps.S_N[0] = 'S';
		}
//-------------------------------------------------------------------------------------------------				
    //LONGTITUDE
		Temp1 = GPS_SearchChar(',',nmeaMessage.GPS_RX_Buffer,5,GPS_BUFFER_SIZE);	
		Temp2 = GPS_SearchChar(',',nmeaMessage.GPS_RX_Buffer,6,GPS_BUFFER_SIZE);	
		k = 0;
		for(i = Temp1 ; i < Temp2-1; i++){
			dataGps.Longtitude[k] = nmeaMessage.GPS_RX_Buffer[i];
			k++;	
		}
		if(nmeaMessage.GPS_RX_Buffer[Temp2]=='E'){
			dataGps.E_W[0] = 'E';
		}
		else{
			dataGps.E_W[0] = 'W';
		}
//-------------------------------------------------------------------------------------------------			
		//VELOCITY
		Temp1 = GPS_SearchChar(',',nmeaMessage.GPS_RX_Buffer,7,GPS_BUFFER_SIZE);	 
		Temp2 = GPS_SearchChar(',',nmeaMessage.GPS_RX_Buffer,8,GPS_BUFFER_SIZE);
		k = 0;
		for(i = Temp1; i < Temp2-1; i++){
			dataGps.Speed[k] = nmeaMessage.GPS_RX_Buffer[i];
			k++;	
		}
		GPS_Knot2Kmh(dataGps.Speed, &dataGps.Velocity);
	return 1;	
	}
}

void GPS_RawData(void){
	while(1){
		//Khi co du lieu tu Shift register truyen vao Rx buffer thi RXNE flag set
  if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET){
		 ch = (uint8_t)((&huart1)->Instance->DR & (uint8_t)0x00FF);
	if (ch == '$'){
		nmeaMessage.GPS_Counter = 1;
		nmeaMessage.GPS_RX_Buffer[nmeaMessage.GPS_Counter-1] = '$';
	}
	if (nmeaMessage.GPS_Counter == 2){
		if (ch == 'G'){
		nmeaMessage.GPS_RX_Buffer[nmeaMessage.GPS_Counter-1] = 'G';
		}
	}
	if (nmeaMessage.GPS_Counter == 3){
		if (ch == 'P'){
		nmeaMessage.GPS_RX_Buffer[nmeaMessage.GPS_Counter-1] = 'P';
		}
	}
	if (nmeaMessage.GPS_Counter == 4){
		if (ch == 'R'){
		nmeaMessage.GPS_RX_Buffer[nmeaMessage.GPS_Counter-1] = 'R';
		nmeaMessage.GPS_Flag++;
		}
	}
	if (nmeaMessage.GPS_Counter == 5){
		if (ch == 'M'){
		nmeaMessage.GPS_RX_Buffer[nmeaMessage.GPS_Counter-1] = 'M';
		nmeaMessage.GPS_Flag++;	
		}
	}
	if (nmeaMessage.GPS_Counter == 6){
		if (ch == 'C'){
		nmeaMessage.GPS_RX_Buffer[nmeaMessage.GPS_Counter-1] = 'C';
		nmeaMessage.GPS_Flag++;
		}
	}
	if (0 < nmeaMessage.GPS_Counter && nmeaMessage.GPS_Counter < 100){
			if (nmeaMessage.GPS_Flag == 3){
				nmeaMessage.GPS_Counter_Tmp = nmeaMessage.GPS_Counter-1;
				nmeaMessage.GPS_RX_Buffer[nmeaMessage.GPS_Counter_Tmp] = ch;
				nmeaMessage.GPS_Counter++;
			}
			else {
				nmeaMessage.GPS_Counter++;
		}
	}
if (ch == '*'){
		if (nmeaMessage.GPS_Flag == 3){
			nmeaMessage.GPS_Flag = 0;
	    nmeaMessage.GPS_Counter = 0;
			printf("%s\r\n", nmeaMessage.GPS_RX_Buffer);
			statusGps.GPS_ans_stt = GPS_Data(dataGps.Time, dataGps.Status, dataGps.Latitude, dataGps.S_N, 
																				 dataGps.Longtitude, dataGps.E_W, dataGps.Speed, dataGps.Date);
			if(statusGps.GPS_ans_stt){
				GPS_ClearRxBuffer();
				printf("%s\r\n",dataGps.Latitude);
				printf("%s\r\n",dataGps.S_N);
				printf("%s\r\n",dataGps.Longtitude);
				printf("%s\r\n",dataGps.E_W);
				printf("%s\r\n",dataGps.Speed);
				printf("%f\r\n",dataGps.Velocity);
			}
			else{
				printf("Not connect yet\r\n");
			}
			break;
			}
	}		
/*-------------------------------------------------------------------*/	 
			}
		}
	}
	
void GPS_Knot2Kmh(char * knot, float * km_h){
    float f_knot;
    f_knot = atof(knot);
    *km_h = f_knot * 1.85;
}