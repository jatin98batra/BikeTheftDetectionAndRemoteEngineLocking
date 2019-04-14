/* 
 * Author: Jatin Batra
 * The code is published under GPL License feel free to edit anything
 * github: @jatin98batra
 */
#include<SoftwareSerial.h>
SoftwareSerial GSM_MOD(10,11); //RX,TX
char rec[150];
int i;
void cleanString();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  GSM_MOD.begin(9600);
  Serial.begin(9600);
}


void loop() {
 int ret;
 ret=checkModule();
 if(ret == 1 )
   Serial.println("It works!");
 else
   Serial.println("Device Not Ready");    
 delay(1000);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                                          ////////////////////////////////FUNCTIONS BEGINS BELOW///////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 void recData()
{
  bool dataLeft=true;
  i=0;
  while(dataLeft)
  {
    
      rec[i]=GSM_MOD.read();
      i++;
      if(GSM_MOD.available())
      dataLeft=true;
      else
      dataLeft=false;  
    
  }
 
}


void cleanString()
{
  int j=0;
  for(j=0;j<sizeof(rec);j++)
  {
    rec[j]='\0';
    
  }
  
}





////////////////////////////////////////////////////////////Commands/////////////////////////////////////////////////////
int checkModule()
{
  int ret;
  GSM_MOD.write("AT\r\n");
  delay (100);
  if(GSM_MOD.available())
  recData();
  if(strcmp(rec,"AT\r\n\r\nOK\r\n") == 0)
        ret=1;
  else
    ret=0;
  cleanString();
  return ret;
}
