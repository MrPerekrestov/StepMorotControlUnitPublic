#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h> 

LiquidCrystal_I2C lcd(0x27, 20, 4);

//step motor has 50 steps!!!
unsigned long Timer;
// rotation speed in milliseconds
int RotationSpeed;
int RotationSpeedRPM;
int StepNumberToChangeTheSample;
unsigned long RotationSpeedAccumulation=0;
unsigned long StepNumberAccumulation=0;
int RotationSpeedNumber=0;
int StepNumberNumber=0;
float Progress;
char bar = 255;
byte HoursLeft;
byte MinutesLeft;
byte SecondsLeft;

//sample deposition time variables
unsigned long TimeHours;
unsigned long TimeMinutes;
unsigned long TimeSeconds;
unsigned long DepositionTime;
unsigned long CurrentTime;
bool ProcessTerminated = false;
byte SampleNumber;

//set time pins
const int TimeHoursPin = 8;
const int TimeMinutesPin = 7;
const int TimeSecondsPin = 6;

//motor pins
const int in1 = 2;
const int in2 = 3;
const int in3 = 4;
const int in4 = 5;

// rotation direction pins
const int ClockWiseRotationTriggerPin = A0;
const int CounterClockWiseRotationTriggerPin = A1;

//bottom row buttons pins

const int ChangePositionClockWisePin = 12;
const int ChangePositionCounterClockWisePin = 11;
const int SampleNumberPin = 10;
const int StartPin = 9;

// pitentiometer pins
const int RotationSpeedPin = A2;
const int StepNumberToChangeTheSamplePin = A3;

void setup()
{
  //motor pins set up
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //rotation direction pins set up
  pinMode(ClockWiseRotationTriggerPin, INPUT);
  pinMode(CounterClockWiseRotationTriggerPin, INPUT);

  //potentiometer pins initialization
  pinMode(RotationSpeedPin, INPUT);
  pinMode(StepNumberToChangeTheSamplePin, INPUT);

  //set time pins initialization
  pinMode(TimeHoursPin, INPUT);
  pinMode(TimeMinutesPin, INPUT);
  pinMode(TimeSecondsPin, INPUT);
  
  //set up bottom button pins
  pinMode(ChangePositionClockWisePin, INPUT);
  pinMode(ChangePositionCounterClockWisePin, INPUT);
  pinMode(SampleNumberPin, INPUT);
  pinMode(StartPin, INPUT);
   
  //read time from EEPROM
  TimeHours   = EEPROM.read(0);
  TimeMinutes = EEPROM.read(1);
  TimeSeconds = EEPROM.read(2);
  
 //read SampleNumber from EEPROM
  SampleNumber = EEPROM.read(3);
  //set up timer
  Timer = millis();  
  
  //lcd set up
  lcd.begin();
  
  lcd.backlight();
  
  lcd.print("*********************"); 
  lcd.setCursor ( 0, 1 );
  lcd.print("Step motor");
  lcd.setCursor ( 0, 2);
  lcd.print("   control unit");
  lcd.setCursor ( 0, 3);
  lcd.print("********************");
  
  delay(2000);

  lcd.clear();
}


void loop()
{  
  
 CheckRotationTrigger();
 CheckHoursPin();  
 CheckMinutesPin();  
 CheckSecondsPin();  
 CheckChangePositionClockWise();
 CheckChangePositionCounterClockWise();
 CheckSampleNumberPin();
 CheckStartPin();
 Accumulate();
 if (millis()-Timer>100)
 {
   ShowMenu();
 }
  
}
void StepMotorPinsOff()
{
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}
void step2()
  {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    delay(RotationSpeed);
  }
  
void step4()
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    delay(RotationSpeed);
  }

void step6()
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    delay(RotationSpeed);
  }

void step8()
  {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    delay(RotationSpeed);
  }

  void CheckStartPin()
  {
    if (digitalRead(StartPin)==LOW)
    {
      delay(10);      
      if (digitalRead(StartPin)==LOW)
      {        
        DepositionTime = 1000*(TimeHours*3600+TimeMinutes*60+TimeSeconds);
        ProcessTerminated = false;
        for(int i=1; i<(SampleNumber+1); i++)
        {
           for(int k=0; k<StepNumberToChangeTheSample; k++)
           {
             step2();
             step4();
             step6();
             step8();
           }        
           StepMotorPinsOff();
           CurrentTime = millis();
           lcd.clear();
           while((millis()-CurrentTime)<DepositionTime)
           {
             lcd.setCursor(1,1);
             lcd.print("Sample: "+(String)i+" ");
             if    ((millis()-CurrentTime)<DepositionTime)  HoursLeft = ((DepositionTime - (millis()-CurrentTime))/3600000);
             if    ((millis()-CurrentTime)<DepositionTime)  MinutesLeft = ((DepositionTime -(millis()-CurrentTime)-HoursLeft*3600000)/60000);
             if    ((millis()-CurrentTime)<DepositionTime)  SecondsLeft = (DepositionTime -(millis()-CurrentTime)-HoursLeft*3600000-MinutesLeft*60000)/1000;
             lcd.setCursor(12,1);
             
             if (HoursLeft<10)
             {
              lcd.print("0"+(String)HoursLeft+":");
             }
             else 
             {
              lcd.print((String)HoursLeft+":");
             }
             
             if (MinutesLeft<10)
             {
              lcd.print("0"+(String)MinutesLeft+":");
             }
             else 
             {
              lcd.print((String)MinutesLeft+":");
             }

             if (SecondsLeft<10)
             {
              lcd.print("0"+(String)SecondsLeft);
             }
             else 
             {
              lcd.print((String)SecondsLeft);
             }
             
             Progress = millis()-CurrentTime;
             Progress = 20*(Progress/DepositionTime);
             lcd.setCursor(Progress-1,2);
             lcd.print(bar);
             if (digitalRead(StartPin)==LOW)
             {
                delay(10);
                if (digitalRead(StartPin)==LOW)
                {
                  ProcessTerminated = true;
                  break;
                }                
             }      
           }
           if (ProcessTerminated) 
           {
             break;
           }
        }
        lcd.clear();
        delay(500);
      }
    }
  }
  void CheckSampleNumberPin()
  {
     if (digitalRead(SampleNumberPin)==LOW)
     {
        delay (10);
          if (digitalRead(SampleNumberPin)==LOW)
          {
            if (SampleNumber<10)
            {
              SampleNumber++;
            }
            else
            {
              SampleNumber = 1;
            }
            EEPROM.write(3,SampleNumber);
            delay(400);
          }
     }
  }
  void Accumulate()
  {    
      RotationSpeedAccumulation+=map(analogRead(RotationSpeedPin), 0, 1023, 200, 5);
      RotationSpeedNumber ++; 
      
      StepNumberAccumulation +=map(analogRead(StepNumberToChangeTheSamplePin),0,1023, 1, 100); 
      StepNumberNumber++;   
  }
 
  void CheckRotationTrigger()
  {
     if (digitalRead(ClockWiseRotationTriggerPin)==HIGH)
     {
        delay (10);
          if (digitalRead(ClockWiseRotationTriggerPin)==HIGH)
          {
            lcd.clear();
            lcd.setCursor ( 0, 1 ); 
            lcd.print("      Rotation      ");
            lcd.setCursor ( 0, 2 );
            lcd.print("     clock wise     ");           
            
            while (digitalRead(ClockWiseRotationTriggerPin)== HIGH)
            {
              step2();
              step4();
              step6();
              step8();              
            }
          }
          
          StepMotorPinsOff();
          
          lcd.clear();          
     }   

     if (digitalRead(CounterClockWiseRotationTriggerPin)==HIGH)
     {
        delay (10);
        if (digitalRead(CounterClockWiseRotationTriggerPin)==HIGH)
        {
          lcd.clear();
          lcd.setCursor ( 0, 1 ); 
          lcd.print("      Rotation      ");
          lcd.setCursor ( 0, 2 );
          lcd.print(" counter clock wise ");    
          
          while (digitalRead(CounterClockWiseRotationTriggerPin)== HIGH)
          {
             step8();
             step6();
             step4();
             step2();                    
          }
        }
        
        StepMotorPinsOff();
        
        lcd.clear();         
       
     }     
  }

  void CheckHoursPin()
  {
    if (digitalRead(TimeHoursPin) == LOW)
    {
      delay(10);
       if (digitalRead(TimeHoursPin) == LOW)
       {
          if (TimeHours<23)
          {
            TimeHours++;
          }
          else
          {
            TimeHours = 0;
          }     
          EEPROM.write(0,TimeHours);   
       }
       delay(400);      
    }
  }

  void CheckMinutesPin()
  {
    if (digitalRead(TimeMinutesPin) == LOW)
    {
      delay(10);
       if (digitalRead(TimeMinutesPin) == LOW)
       {
          if (TimeMinutes<59)
          {
            TimeMinutes++;
          }
          else
          {
            TimeMinutes = 0;
          }     
          EEPROM.write(1,TimeMinutes);   
       }
       delay(400);      
    }
  }

  void CheckSecondsPin()
  {
    if (digitalRead(TimeSecondsPin) == LOW)
    {
      delay(10);
      
       if (digitalRead(TimeSecondsPin) == LOW)
       {
          if (TimeSeconds<59)
          {
            TimeSeconds ++ ;
          }
          else
          {
            TimeSeconds = 0;
          }     
          EEPROM.write(2,TimeSeconds);   
       }
       delay(400);      
    }
  }
  
  void CheckChangePositionClockWise()
  {
    if (digitalRead(ChangePositionClockWisePin)==LOW)
    {
      delay(10);
      if (digitalRead(ChangePositionClockWisePin)==LOW)
      {
        lcd.clear();
        lcd.setCursor(3,1);
        lcd.print("Change position");
        lcd.setCursor(5,2);
        lcd.print("Clock wise");
        for(int i=0; i<StepNumberToChangeTheSample; i++)
        {
          step2();
          step4();
          step6();
          step8();
        }
        
        StepMotorPinsOff();
        
        delay(500);
        lcd.clear();
      }
    }
  }

  void CheckChangePositionCounterClockWise()
  {
    if (digitalRead(ChangePositionCounterClockWisePin)==LOW)
    {
      delay(10);
      if (digitalRead(ChangePositionCounterClockWisePin)==LOW)
      {
        lcd.clear();
        lcd.setCursor(3,1);
        lcd.print("Change position");
        lcd.setCursor(1,2);
        lcd.print("Counter Clock wise");
        for(int i=0; i<StepNumberToChangeTheSample; i++)
        {
          step8();
          step6();
          step4();
          step2();
        }
        
        StepMotorPinsOff();
        
        delay(500);
        lcd.clear();
      }
    }
  }

  

  void ShowMenu()
  {
      // show rotation speed
      
    lcd.setCursor ( 0, 0 ); 
    lcd.print("Speed:");
        
    RotationSpeed = RotationSpeedAccumulation/RotationSpeedNumber; 
    RotationSpeedAccumulation = 0;
    RotationSpeedNumber = 0;
    RotationSpeedRPM = 300 / RotationSpeed ;

    lcd.setCursor ( 9, 0 );
     
    if (RotationSpeedRPM<10)
    {
      lcd.print((String)(RotationSpeedRPM)+" ");
    }
    else
    {
      lcd.print((String)(RotationSpeedRPM));
    }
    
    lcd.setCursor ( 13, 0 );
    lcd.print("r/m");
    
    //show number of steps of one sample change

    StepNumberToChangeTheSample = StepNumberAccumulation/StepNumberNumber;
    StepNumberAccumulation = 0;
    StepNumberNumber = 0;

     lcd.setCursor ( 0, 1 );
     lcd.print("Shift:");
     
     lcd.setCursor ( 9, 1 );
     lcd.print((String)(StepNumberToChangeTheSample)+"   ");
     lcd.setCursor ( 13, 1 );
     lcd.print("steps");
     
     //show sample number
     lcd.setCursor ( 0, 2 );
     lcd.print("Samples:");

     lcd.setCursor ( 9, 2 );
     lcd.print((String)SampleNumber+"   ");

     //show time

     lcd.setCursor ( 0, 3 );
     lcd.print("Time:");
     
     //hours
     lcd.setCursor ( 9, 3 );      
     if (TimeHours>9)
     {
        lcd.print((String)TimeHours+":");
     }
     else
     {
        lcd.print("0"+(String)TimeHours+":"); 
     }
     
     //minutes
     lcd.setCursor ( 12, 3 );      
     if (TimeMinutes>9)
     {
        lcd.print((String)TimeMinutes+":");
     }
     else
     {
        lcd.print("0"+(String)TimeMinutes+":"); 
     }
     
     //seconds
     lcd.setCursor ( 15, 3 );      
     if (TimeSeconds>9)
     {
        lcd.print((String)TimeSeconds);
     }
     else
     {
        lcd.print("0"+(String)TimeSeconds); 
     }
     
    Timer = millis();
  }
  
