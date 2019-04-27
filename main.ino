/* 
 * Author: Jatin Batra
 * The code is published under GPL License feel free to edit anything
 * github: @jatin98batra
 */

/*
 * 1.If theft isn't detected and
 * a) the user calls: Engine will be locked if it was unlocked or vice versa
 * 
 * 2.If theft is detected and message is sent to user and
 * a) user didn't called: the service willl be activated and engine will be locked if it was unlocked before
 * b) user did called: the service will not be activated the engine state will be left untouched .
 * 
 * 3.Optional(Temprature Sensor)
 * a)If temp rose above a certain level this will automatically start sending cautionary messages
 */


#include<SoftwareSerial.h>
#define engineLocked 0
#define engineUnlocked 1
#define theftDetected 1
#define theftUndetected 0
#define serviceAlertActive 1
#define serviceAlertDeactive 0  
SoftwareSerial GSM_MOD(10,11); //RX,TX
char rec[150];
//////States///////
int engineState=engineUnlocked; //Inital state of the engine is being unlocked
int theftState=theftUndetected;  //Intial state of the vehicle being attcked is 
int serviceAlertState=serviceAlertDeactive;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(2,INPUT_PULLUP);
  GSM_MOD.begin(9600);
  Serial.begin(9600);
  int ret;
  int count=0;
  checkModule();
  /*To clean the USART Buffer*/
  recData(rec); 
  setMode(); 
  checkModule();
  cleanString(rec);
}

void loop() {
  if(GSM_MOD.available()>1)
  {
     recData(rec);
     if(checkRing(rec)==1)
     {
        sendState(); 
     }
  }
  if(digitalRead(2) == LOW)
  {
      theftState=theftDetected;
      serviceAlertState=serviceAlertActive;
      checkModule();
      sendFirstAlert();
      while(serviceAlertState == serviceAlertActive)
    {   serviceAlert();
        checkRing(rec); 
    }
    
  }
  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                                          ////////////////////////////////FUNCTIONS BEGINS BELOW///////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 void recData(char *lrec)
{ int i=0;
    if(GSM_MOD.available()>0)
      {
        bool dataLeft=true;
        i=0;
        while(dataLeft)
        {
           lrec[i]=GSM_MOD.read();
           delay(2);
           i++;
           if(GSM_MOD.available()>0)
           dataLeft=true;
           else
           dataLeft=false;           
        }
      }
}

void printData()
{ int i;
  for(i=0;i<40;i++)
  {
//      
      Serial.print(i);
      Serial.print("=");
      Serial.print(rec[i]);  
      Serial.print("\n");
//        Serial.print(rec[i]);
     
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
void checkModule()
{
  int count=0;
  while(1)
  {
  count++;  
  Serial.print("Checking Module... Try: ");
  Serial.println(count);
  GSM_MOD.write("AT\r\n");
  delay(1000);
  if(GSM_MOD.available())
  recData(rec);
  //  printData();
  if(strncmp(rec,"AT\r\n\r\nOK\r\n",10) == 0)
    {Serial.println("It works");Serial.println("---------------------------------------");break;}    
  else
    {Serial.println("Retrying.... 1 Second Later...");delay(1000);continue;}
    
  }
}


int checkRing(char * lrec)
{
   int returnValue=0;
  //printData();
  if((strncmp("\r\nRING\r\n",lrec,8) == 0)) 
  {
   
    /*For The stable Read*/
    delay(2000);
    cleanString(rec);
    recData(rec);
    Serial.println("Call Detected");
    if(strncmp("+917988151747",lrec+10,13)==0)
    {
      Serial.println("User Calling");
      returnValue=1;
      if(engineState == engineUnlocked)
      {
           if(theftState == theftUndetected)
           {
              Serial.println("Initiating Engine Locking");
              Serial.println("---------------------------------------");
              engineState=engineLocked;
           }
           else //user movement detected as intruder activity
           {
               Serial.println("Cancelling the services...Keeping the engine as it is");
               Serial.println("---------------------------------------");
               engineState=engineState;
               theftState=theftUndetected; //Resetting
               serviceAlertState=serviceAlertDeactive;
           }
      }
      else //locked engine state
      {
           if(theftState == theftUndetected)
           {
              Serial.println("Initiating Engine Unlocking");
              Serial.println("---------------------------------------");
              engineState=engineUnlocked;
           }
           else //user movement detected as intruder activity
           {
               Serial.println("Cancelling the services...Keeping the engine as it is");
               Serial.println("---------------------------------------");
               engineState=engineState;
               theftState=theftUndetected; //Resetting
               serviceAlertState=serviceAlertDeactive;
           } 
            
      } 
      
    }
    else
    {
      Serial.println("Call from unknown source.. no action taken"); 
      Serial.println("---------------------------------------");
      returnValue=0;
    }
    delay(100);
    GSM_MOD.write("ATH\r\n");  
    delay(1000);
    recData(rec);
    cleanString(rec); ///cleaning
    checkModule();
  }
  return returnValue;
}

void setMode()
{
  while(1)
  {
    Serial.println("Setting Device in text mode");
    GSM_MOD.write("AT+CMGF=1\r\n");
    delay(1000);
    recData(rec);
    if(strcmp(rec,"AT+CMGF=1\r\n\r\nOK\r\n") == 0)  
        {Serial.println("Set"); 
        Serial.println("---------------------------------------"); break;}
    else
        {Serial.println("Not Set..... Retrying"); delay(1000);}
   }
  
}



void sendFirstAlert()
{
  while(1)
    {
      GSM_MOD.write("AT+CMGS=\"+917988151747\"\r\n");
      delay(1000) ;
      recData(rec);
//      printData();
      if(strncmp(rec,"AT+CMGS=\"+917988151747\"\r\n\r\n>",28) == 0)
      {Serial.println("SendingData...");Serial.println("---------------------------------------"); delay(1000); break;}
      else
      {Serial.println("Failed...Retrying");delay(1000);continue;}
    }
    GSM_MOD.write("Someone entered your car.Was it you? If yes then call on the registered number otherwise *theftService* will be activated");
    delay(2000);
    GSM_MOD.write(0x1a);
    delay(3000);
    recData(rec);//cleaning
    cleanString(rec);
    checkModule();
  }



  
void serviceAlert()
{
    while(1)
    {
      GSM_MOD.write("AT+CMGS=\"+917988151747\"\r\n");
      delay(1000) ;
      recData(rec);
      //printData();
      if(strncmp(rec,"AT+CMGS=\"+917988151747\"\r\n\r\n>",28) == 0)
      {Serial.println("SendingData...");Serial.println("---------------------------------------"); delay(1000); break;}
      else
      {Serial.println("Failed...Retrying");delay(1000);continue;}
    }
    GSM_MOD.write("CAR's Engine:");
    delay(1000);
    engineState?GSM_MOD.write("Unlocked\n"):GSM_MOD.write("Locked\n");
    delay(1000);
    GSM_MOD.write("CAR's Position:41(deg)24'12.2\"N-21(deg)23'166.2\"E\n");
    delay(1000);
    GSM_MOD.write("Call to stop this service!");
    delay(1000);
    GSM_MOD.write(0x1a);
    delay(5000);
    checkModule();
  }






void sendState()
{
    while(1)
    {
      GSM_MOD.write("AT+CMGS=\"+917988151747\"\r\n");
      delay(1000) ;
      recData(rec);
//      printData();
      if(strncmp(rec,"AT+CMGS=\"+917988151747\"\r\n\r\n>",28) == 0)
      {Serial.println("SendingData...");Serial.println("---------------------------------------"); delay(1000); break;}
      else
      {Serial.println("Failed...Retrying");delay(1000);continue;}
    }
    GSM_MOD.write("CAR's Engine:");
    delay(1000);
    engineState?GSM_MOD.write("Unlocked\n"):GSM_MOD.write("Locked\n");
    delay(1000);
    GSM_MOD.write(0x1a);
    delay(3000);
    recData(rec);
    cleanString(rec);
    checkModule();
  }
