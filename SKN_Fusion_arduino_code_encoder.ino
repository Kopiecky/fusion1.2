#include <Chrono.h>
//pins 2,3 error handling of motor driver
int E1 = 5;     //M1 Speed Control
int E2 = 6;     //M2 Speed Control
int M1 = 4;     //M1 Direction Control
int M2 = 7;     //M1 Direction Control
int enRA=8; //output A of right motor encoder
int enLA=9; //output A of left motor encoder
int enRB=10; //output B of right motor encoder
int enLB=11; //output B of left motor encoder
int counter=0; //error_handling
int val_x; //serial_incoming_data
int base_speed;//base_speed
int left_motor_pwm; //pwm handling
int right_motor_pwm;
int left_motor_direction;
int right_motor_direction;
bool LMD,RMD;//direction handling
int straight_direction_flag=0;
Chrono timer;
int counterRA=0,counterRB=0,counterLA=0,counterLB=0;//rotation counter (encoder)
void stop(void)                    //Stop
{
  digitalWrite(E1,0); 
  digitalWrite(M1,LOW);    
  digitalWrite(E2,0);   
  digitalWrite(M2,LOW);    
}   


void current_sense()                  // current sense and diagnosis
{
  int val1=digitalRead(2);
  int val2=digitalRead(3);
  if(val1==HIGH || val2==HIGH){
    counter++;
    if(counter==3){
      counter=0;
      Serial.println("Motror Driver Warning");
    }  
  } 
}

void setup(void) 
{ 
  int i;
  for(i=4;i<=7;i++)
    pinMode(i, OUTPUT);  
  Serial.begin(19200);      //Set Baud Rate
  Serial.setTimeout(10);
  Serial.println("Run keyboard control");
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW); 
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(enRA,INPUT);
  pinMode(enRB,INPUT);
  pinMode(enLA,INPUT);
  pinMode(enLB, INPUT);
 timer.restart();
} 

void loop(void) 
{
  
  //read DATA via Serial
  if (Serial.available()>4)
  {
  val_x=Serial.readString().toInt();  
  Serial.println(val_x);
  //decrypt DATA from int number
  base_speed=(val_x/10000);
  if(base_speed!=1&&base_speed!=2)
  {
    Serial.println ("Bad base speed");
    base_speed=0;
  }
  right_motor_pwm=(val_x/1000 - base_speed*10);
  left_motor_pwm=(val_x/100 - base_speed*100 - right_motor_pwm*10);
  right_motor_direction=val_x/10 - base_speed*1000 - right_motor_pwm*100 - left_motor_pwm*10;
  left_motor_direction=val_x - base_speed*10000 - right_motor_pwm*1000 - left_motor_pwm*100 - right_motor_direction*10; 
  right_motor_pwm=right_motor_pwm*base_speed;
  left_motor_pwm=left_motor_pwm*base_speed
  left_motor_pwm=map(left_motor_pwm,0,18,0,255);
  right_motor_pwm=map(right_motor_pwm,0,18,0,255);
  if(left_motor_pwm==right_motor_pwm)
  {
    straight_direction_flag=1;
    timer.restart();
  }

  if(enRA==HIGH){
    counterRA++;
  }
  if(enRB==HIGH){
    counterRB++;
  }
  if(enLA==HIGH){
  counterLA++;
  }
  if(enLB==HIGH){
    counterLB++;
  }
  if (timer.hasPassed(100)){
    timer.restart();
    if(straight_direction_flag==1){
      if(counterRA<counterLA)
      {
        if(right_motor_pwm>=250){
        left_motor_pwm=left_motor_pwm-5;
        }else
        {
          right_motor_pwm=right_motor_pwm + 5;
        }
      }
      if(counterRA>counterLA)
      {
         if(left_motor_pwm>=250){
        right_motor_pwm=right_motor_pwm-5;
        }else
        {
         left_motor_pwm=left_motor_pwm-5;
        }
      }
      counterRA=0;
      counterLA=0;
      counterRB=0;
      counterLB=0;
    }
    //doprojektować kontrolę działania enkodera i kontrolę kierunku jazdy
  }
   //manual int to bool conversion; 
   if(left_motor_direction==1)
   LMD=LOW;
   if(right_motor_direction==1)
   RMD=LOW;
   if(left_motor_direction==2)
   LMD=HIGH;
   if(right_motor_direction==2)
   RMD=HIGH;
 //applying new motor parameters
 digitalWrite (M1,LMD);
 digitalWrite (M2,RMD);
 analogWrite(E1,left_motor_pwm);
 analogWrite(E2,right_motor_pwm);


}
}
