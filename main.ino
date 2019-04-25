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
SoftwareSerial GSM_MOD(10,11); //RX,TX
char rec[150];
int i;
//////States///////
int engineState=engineUnlocked; //Inital state of the engine is being unlocked
int theftState=theftUndetected;  //Intial state of the vehicle being attcked is 
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
      Serial.println("---------------------------------------");
      delay(15);
      break;
      }
    else
      {
      Serial.println("Device Not Ready");    
      Serial.println("Retrying... in 1 Sec..");
      Serial.println("---------------------------------------");
      delay(1000);
      continue;
      }
  }
  /*To clean the USART Buffer*/
  recData(rec); 
  cleanString(rec);
theftState=theftDetected;
}

void loop() {

  if(GSM_MOD.available()>1)
  {
    recData(rec);
    checkRing(rec);
    cleanString(rec);
    delay(1000);
        
  }

 
}

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
    if(strncmp("+917988151747",lrec+16,13)==0)
    {
      Serial.println("User Calling");
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
           } 
            
      } 
      
    }
    else
    {
      Serial.println("Call from unknown source.. no action taken"); 
      Serial.println("---------------------------------------");
      
    }
    GSM_MOD.write("ATH\r\n");  
  }
}
