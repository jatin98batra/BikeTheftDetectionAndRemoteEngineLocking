/* 
 * Author: Jatin Batra
 * The code is published under GPL License feel free to edit anything
 * github: @jatin98batra
 */
#include<SoftwareSerial.h>
SoftwareSerial GSM_MOD(10,11); //RX,TX
char rec[150];
int i;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  GSM_MOD.begin(9600);
  Serial.begin(9600);
  int ret;
  int count=0;
  while(1)
  {
    count++;
    Serial.print("Testing Module......Try: ");
    Serial.println(count);
    ret=checkModule();
    if(ret == 1 )
     {
      Serial.println("It works!");
      delay(15);
      break;
      }
    else
      {
      Serial.println("Device Not Ready");    
      Serial.println("Retrying... in 1 Sec..");
      delay(1000);
      continue;
      }
  }
  /*To clean the USART Buffer*/
  recData(rec); 
  cleanString(rec);

}


void loop() {

  if(GSM_MOD.available()>1)
  {
    recData(rec);
//    printData();
    checkRing(rec);
    cleanString(rec);
    delay(1000);
    
        
  }

 
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                                          ////////////////////////////////FUNCTIONS BEGINS BELOW///////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 void recData(char *lrec)
{
    if(GSM_MOD.available()>1)
      {
        bool dataLeft=true;
        i=0;
        while(dataLeft)
        {
           lrec[i]=GSM_MOD.read();
           i++;
           if(GSM_MOD.available()>0)
           dataLeft=true;
           else
           dataLeft=false;  

          
        }
      }
}

void printData()
{
  for(i=0;i<150;i++)
  {
   if(rec[i] != '\0')
    { 
//      Serial.print(i);
//      Serial.print("=");
//      Serial.print(rec[i]);  
//      Serial.print("\n");
      Serial.print(rec[i]);
     }
  else
    {
      break;
      
    }
    
  }
}


void cleanString(char* lrec)
{
  int j=0;
  for(j=0;j<sizeof(lrec);j++)
  {
    lrec[j]='\0';
    
  }
  
}

////////////////////////////////////////////////////////////Commands/////////////////////////////////////////////////////
int checkModule()
{
  int ret;
  GSM_MOD.write("AT\r\n");
  delay (100);
  if(GSM_MOD.available())
  recData(rec);
  if(strcmp(rec,"AT\r\n\r\nOK\r\n") == 0)
        ret=1;
  else
    ret=0;
  cleanString(rec);
  return ret;
}


void checkRing(char * lrec)
{
  if(strncmp("RING\r\n",lrec,6) == 0)
  {
    Serial.println("Call Detected");
    if(strncmp("+91XXXXXXXXXX",lrec+16,13)==0)
    {
      Serial.println("User Calling");
      Serial.println("Initiating Engine Locking");
        
    }
    else
    {
      Serial.println("Call from unknown source.. no action taken"); 
      
    }
    GSM_MOD.write("ATH\r\n");  
  }
}
