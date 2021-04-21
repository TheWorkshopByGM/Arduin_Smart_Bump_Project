//Guy Maalouf - 25/4/2019
//Smart Bump Project
// !!IMPORTANT NOTICE!! : in order to set temperature to 0 in software comment line 486 and uncomment line 487
// if you encounter errors make sure you add the DHT.h library 

#include <Servo.h>
#include "DHT.h"
#define DHTPIN 23     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//PINS
int Left_Coils_Pin = 53;
int Right_Coils_Pin = 51;
int Rain_Sens_Pin = A0;
int Light_Sens_Pin = A1;
int Left_Servo_Pin = 2;
int Right_Servo_Pin = 3;

int R_A_Pin=24;
int R_B_Pin=22;
int R_C_Pin=50;
int R_D_Pin=46;
int R_E_Pin=44;
int R_F_Pin=26;
int R_G_Pin=48;
int R_DP_Pin=52;

int L_A_Pin=30;
int L_B_Pin=28;
int L_C_Pin=40;
int L_D_Pin=38;
int L_E_Pin=36;
int L_F_Pin=34;
int L_G_Pin=32;
int L_DP_Pin=42;

//===================================================================================================
//===================================================================================================
//=========================================== VARIABLES =============================================
//===================================================================================================
//===================================================================================================
//Reports
bool Speed_Limit_Report = false;
bool Pred_Speed_Report = true;
bool Detailed_Speed_Report = true;

//Servos
Servo Left_Servo;
int Left_Servo_Max = 120;
int Left_Servo_Min = 55;

Servo Right_Servo;
int Right_Servo_Max = 130;
int Right_Servo_Min = 70;

//Speed and Acceleration
bool Left_Prev = LOW;
long Left_Actual_Time = 0;
long Left_Previous_Time= 0;
float Left_Denominator = 0;
int Left_Coil = 0;
float Left_Speed_1 = 0; //1-2
float Left_Speed_2 = 0; //2-3
float Left_Speed_3 = 0; //3-4
float Left_Speed_4 = 0; //4-5
float Left_Acceleration_1; //Speed 1 - 2
float Left_Acceleration_2; //Speed 2 - 3
float Left_Acceleration_3; //Speed 3 - 4
float Left_Pred_Speed = 0;
float Left_Temporary_Value = 0;

bool Right_Prev = LOW;
long Right_Actual_Time = 0;
long Right_Previous_Time= 0;
float Right_Denominator = 0;
int Right_Coil = 0;
float Right_Speed_1 = 0; //1-2
float Right_Speed_2 = 0; //2-3
float Right_Speed_3 = 0; //3-4
float Right_Speed_4 = 0; //4-5
float Right_Acceleration_1; //Speed 1 - 2
float Right_Acceleration_2; //Speed 2 - 3
float Right_Acceleration_3; //Speed 3 - 4
float Right_Pred_Speed = 0;
float Right_Temporary_Value = 0;

//Speed Limit
int Speed_Limit = 0;
int Prev_Speed_Limit = 80;

//SENSORS
//Rain
int Rain_Sens_Val = 0;
bool No_Rain = false;
bool Light_Rain = false;
bool Medium_Rain = false;
bool Heavy_Rain = false;
int Light_Threshold = 975;
int Medium_Threshold = 590;
int Heavy_Threshold = 380;

//Light
int Light_Sens_Val = 0;
bool Drying_Heavy = false;
bool Drying_Medium = false;
bool Drying_Light = false;
bool Drying_First_Time = true;
long Prev_Time_Heavy = 0;
long Prev_Time_Medium = 0;
long Prev_Time_Light = 0;
long Current_Time = 0;
long Drying_Delay = 0;
long Max_Drying_Delay = 10000; // Self Set, in ms
int Max_Light_Sens_Val = 1023; //Self Set, between 0 and 1023
int Min_Light_Sens_Val = 75;//Self Set, between 0 and 1023
float Temporary_Val = 0;

//Temperature
float Temperature_Val = 1;
int Temperature_Threshold = 0;
bool Icy_Road = false;
long Last_Measure = -2000;
long Current_Measure = 0;



//===================================================================================================
//===================================================================================================
//=========================================== FUNCTIONS =============================================
//===================================================================================================
//===================================================================================================
//====================================== Quadratic Function ========================================
float quad(float a, float b, int c) //form: a^2 + bx + c = 0    <--------------- make sure to input values in correct form
{
  float determinant, x1, x2;
  if (a == 0) {return (-c/b);} //0 acceleration => time needed = distance/speed
  determinant = (pow(b,2) - (4 * a* c)); 
  if (determinant == 0) { return ((-b)/(2*a));} // if the determinant is zero, then there will be one solution
  else if (determinant > 0) //need the smallest positive time
  {
    x1 = ((-b) + sqrtf(determinant))/(2*a);
    x2 = ((-b) - sqrtf(determinant))/(2*a);
    if(x1 < 0) {return x2;}
    else if (x2 < 0) {return x1;}
    else if (x1 > x2) {return x2;}
    else {return x1;}
  }
  else //in this case we will have imaginary numbers since it would be deccelerating too fast for it to ever reach the required distance => for simplicity return t= 10000000 (=>speed = 0)
  {
    return 0.0;
  }
}

//=========================================== Left_Coils ============================================
void Left_Coils(){
  //Serial.print("Left: ");
  //Serial.println(digitalRead(Left_Coils_Pin));
  if(digitalRead(Left_Coils_Pin) != Left_Prev){
    if(Left_Coil == 0){   
      Left_Coil++;
      Left_Previous_Time = micros();
      Left_Prev = !Left_Prev ;
    }
    else if (Left_Coil == 1){
      Left_Coil++;
      Left_Actual_Time = micros();
      Left_Denominator = (Left_Actual_Time- Left_Previous_Time) / 1000000.00;
      Left_Speed_1 = 20.00 / Left_Denominator; //finds the speed in cm/s
      Left_Pred_Speed = Left_Speed_1;
      Left_Previous_Time = Left_Actual_Time; 
      Left_Prev = !Left_Prev;
      if(Detailed_Speed_Report){
        Serial.print("Left_Actual_Time: ");               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Left_Actual_Time);                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("  -  Left_Previous_Time: ");        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Previous_Time);               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("Speed 1: ");                        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Speed_1);                     //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        Serial.print("Left Pred Speed 1: ");                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
      else if(Pred_Speed_Report){
        Serial.print("Left Pred Speed 1: ");                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
    }
    else if (Left_Coil == 2){
      Left_Coil++;
      Left_Actual_Time = micros();
      Left_Denominator = (Left_Actual_Time- Left_Previous_Time) / 1000000.00;
      Left_Speed_2 = 20.00 / Left_Denominator; //finds the speed in cm/s
      Left_Acceleration_1 = (Left_Speed_2 - Left_Speed_1)/Left_Denominator;
      Left_Temporary_Value = quad(0.5*Left_Acceleration_1,Left_Speed_2,-60);
      if(Left_Temporary_Value == 0.0){
        Left_Pred_Speed = 0;
      }
      else{
        Left_Pred_Speed = Left_Temporary_Value*Left_Acceleration_1 + Left_Speed_2;
      }
      //Left_Pred_Speed = Left_Speed_2;
      Left_Previous_Time = Left_Actual_Time; 
      Left_Prev = !Left_Prev;
      if(Detailed_Speed_Report){
        Serial.print("Left_Actual_Time: ");               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Left_Actual_Time);                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("  -  Left_Previous_Time: ");        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Previous_Time);               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("Speed 2: ");                        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Left_Speed_2);                       //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        Serial.print("  -  Acceleration 1: ");            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Acceleration_1);              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  
        Serial.print("Left Pred Speed 2: ");                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
      else if(Pred_Speed_Report){
        Serial.print("Left Pred Speed 2: ");                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
    } 
    else if (Left_Coil == 3){
      Left_Coil++;
      Left_Actual_Time = micros();
      Left_Denominator = (Left_Actual_Time- Left_Previous_Time) / 1000000.00;
      Left_Speed_3 = 20.00 / Left_Denominator; //finds the speed in cm/s
      Left_Acceleration_2 = (Left_Speed_3 - Left_Speed_2)/Left_Denominator;
      Left_Temporary_Value = quad(0.5*Left_Acceleration_2,Left_Speed_3,-40);
      if(Left_Temporary_Value == 0.0){
        Left_Pred_Speed = 0;
      }
      else{
        Left_Pred_Speed = Left_Temporary_Value*Left_Acceleration_2 + Left_Speed_3;
      }
      //Left_Pred_Speed = Left_Speed_3;
      Left_Previous_Time = Left_Actual_Time; 
      Left_Prev = !Left_Prev;
      if(Detailed_Speed_Report){
        Serial.print("Left_Actual_Time: ");               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Left_Actual_Time);                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("  -  Left_Previous_Time: ");        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Previous_Time);               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("Speed 3: ");                        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Left_Speed_3);                       //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        Serial.print("  -  Acceleration 2: ");            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Acceleration_2);              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        Serial.print("Left Pred Speed 3: ");                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
      else if(Pred_Speed_Report){
        Serial.print("Left Pred Speed 3: ");                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
    }
    else if (Left_Coil == 4){
      Left_Coil++;
      Left_Actual_Time = micros();
      Left_Denominator = (Left_Actual_Time- Left_Previous_Time) / 1000000.00;
      Left_Speed_4 = 20.00 / Left_Denominator; //finds the speed in cm/s
      Left_Acceleration_3 = (Left_Speed_4 - Left_Speed_3)/Left_Denominator;
      Left_Temporary_Value = quad(0.5*Left_Acceleration_3,Left_Speed_4,-20);
      if(Left_Temporary_Value == 0.0){
        Left_Pred_Speed = 0;
      }
      else{
        Left_Pred_Speed = Left_Temporary_Value*Left_Acceleration_3 + Left_Speed_4;
      }
      //Left_Pred_Speed = Left_Speed_4;
      Left_Previous_Time = Left_Actual_Time; 
      Left_Prev = !Left_Prev;
      if(Detailed_Speed_Report){
        Serial.print("Left_Actual_Time: ");               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Left_Actual_Time);                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("  -  Left_Previous_Time: ");        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Previous_Time);               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("Speed 4: ");                        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Left_Speed_4);                       //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        Serial.print("  -  Acceleration 3: ");            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Acceleration_3);              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("Left Pred Speed 4: ");                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
      else if(Pred_Speed_Report){
        Serial.print("Left Pred Speed 4: ");                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Left_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
    }
    else if (Left_Coil == 5){
      Left_Coil = 0;  
      Left_Prev = !Left_Prev; 
      Left_Pred_Speed = 0;
    }
  }
}

//=========================================== Right_Coils ================================================
void Right_Coils(){
  //Serial.print("Right: ");
  //Serial.println(digitalRead(Right_Coils_Pin));
  if(digitalRead(Right_Coils_Pin) != Right_Prev){
    if(Right_Coil == 0){   
      Right_Coil++;
      Right_Previous_Time = micros();
      Right_Prev = !Right_Prev ;
    }
    else if (Right_Coil == 1){
      Right_Coil++;
      Right_Actual_Time = micros();
      Right_Denominator = (Right_Actual_Time- Right_Previous_Time) / 1000000.00;
      Right_Speed_1 = 20.00 / Right_Denominator; //finds the speed in cm/s
      Right_Pred_Speed = Right_Speed_1;
      Right_Previous_Time = Right_Actual_Time; 
      Right_Prev = !Right_Prev;
      if(Detailed_Speed_Report){
        Serial.print("Right_Actual_Time: ");               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Right_Actual_Time);                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("  -  Right_Previous_Time: ");        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Previous_Time);               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("Speed 1: ");                         //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Speed_1);                     //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        Serial.print("Right Pred Speed 1: ");              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
      else if(Pred_Speed_Report){
        Serial.print("Right Pred Speed 1: ");              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
    }
    else if (Right_Coil == 2){
      Right_Coil++;
      Right_Actual_Time = micros();
      Right_Denominator = (Right_Actual_Time- Right_Previous_Time) / 1000000.00;
      Right_Speed_2 = 20.00 / Right_Denominator; //finds the speed in cm/s
      Right_Acceleration_1 = (Right_Speed_2 - Right_Speed_1)/Right_Denominator;
      Right_Temporary_Value = quad(0.5*Right_Acceleration_1,Right_Speed_2,-60);
      if(Right_Temporary_Value == 0.0){
        Right_Pred_Speed = 0;
      }
      else{
        Right_Pred_Speed = Right_Temporary_Value*Right_Acceleration_1 + Right_Speed_2;
      }
      //Right_Pred_Speed = Right_Speed_2;
      Right_Previous_Time = Right_Actual_Time; 
      Right_Prev = !Right_Prev;
      if(Detailed_Speed_Report){
        Serial.print("Right_Actual_Time: ");               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Right_Actual_Time);                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("  -  Right_Previous_Time: ");        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Previous_Time);               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("Speed 2: ");                         //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Right_Speed_2);                       //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        Serial.print("  -  Acceleration 1: ");             //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Acceleration_1);              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  
        Serial.print("Right Pred Speed 2: ");              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
      else if(Pred_Speed_Report){
        Serial.print("Right Pred Speed 2: ");              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
    } 
    else if (Right_Coil == 3){
      Right_Coil++;
      Right_Actual_Time = micros();
      Right_Denominator = (Right_Actual_Time- Right_Previous_Time) / 1000000.00;
      Right_Speed_3 = 20.00 / Right_Denominator; //finds the speed in cm/s
      Right_Acceleration_2 = (Right_Speed_3 - Right_Speed_2)/Right_Denominator;
      Right_Temporary_Value = quad(0.5*Right_Acceleration_2,Right_Speed_3,-40);
      if(Right_Temporary_Value == 0.0){
        Right_Pred_Speed = 0;
      }
      else{
        Right_Pred_Speed = Right_Temporary_Value*Right_Acceleration_2 + Right_Speed_3;
      }
      //Right_Pred_Speed = Right_Speed_3;
      Right_Previous_Time = Right_Actual_Time; 
      Right_Prev = !Right_Prev;
      if(Detailed_Speed_Report){
        Serial.print("Right_Actual_Time: ");               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Right_Actual_Time);                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("  -  Right_Previous_Time: ");        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Previous_Time);               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("Speed 3: ");                         //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Right_Speed_3);                       //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        Serial.print("  -  Acceleration 2: ");             //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Acceleration_2);              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        Serial.print("Right Pred Speed 3: ");              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<         
      }
      else if(Pred_Speed_Report){
        Serial.print("Right Pred Speed 3: ");              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
    }
    else if (Right_Coil == 4){
      Right_Coil++;
      Right_Actual_Time = micros();
      Right_Denominator = (Right_Actual_Time- Right_Previous_Time) / 1000000.00;
      Right_Speed_4 = 20.00 / Right_Denominator; //finds the speed in cm/s
      Right_Acceleration_3 = (Right_Speed_4 - Right_Speed_3)/Right_Denominator;
      Right_Temporary_Value = quad(0.5*Right_Acceleration_3,Right_Speed_4,-20);
      if(Right_Temporary_Value == 0.0){
        Right_Pred_Speed = 0;
      }
      else{
        Right_Pred_Speed = Right_Temporary_Value*Right_Acceleration_3 + Right_Speed_4;
      }
      //Right_Pred_Speed = Right_Speed_4;
      Right_Previous_Time = Right_Actual_Time; 
      Right_Prev = !Right_Prev;
      if(Detailed_Speed_Report){
        Serial.print("Right_Actual_Time: ");               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Right_Actual_Time);                   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("  -  Right_Previous_Time: ");        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Previous_Time);               //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("Speed 4: ");                         //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print(Right_Speed_4);                       //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        Serial.print("  -  Acceleration 3: ");             //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Acceleration_3);              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.print("Right Pred Speed 4: ");              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }
      else if(Pred_Speed_Report){
        Serial.print("Right Pred Speed 4: ");              //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        Serial.println(Right_Pred_Speed);                  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
      }      
    }
    else if (Right_Coil == 5){
      Right_Coil = 0; 
      Right_Prev = !Right_Prev;
      Right_Pred_Speed = 0;
    }
  }
}

//========================================== Speed Limit ============================================
void Set_Speed_Limit(){

  //Rain Readings
  Rain_Sens_Val = analogRead(Rain_Sens_Pin);
  if(Speed_Limit_Report){
    Serial.print("Rain Val: ");
    Serial.println(Rain_Sens_Val);
  }
  if(Rain_Sens_Val > Light_Threshold){
      No_Rain = true;
      Light_Rain = false;
      Medium_Rain = false;
      Heavy_Rain = false;
      if(Speed_Limit_Report){
        Serial.println("Rain : No Rain");
      }
  }
  else if(Light_Threshold > Rain_Sens_Val && Rain_Sens_Val > Medium_Threshold){
      No_Rain = false;
      Light_Rain = true;
      Medium_Rain = false;
      Heavy_Rain = false;
      if(Speed_Limit_Report){
        Serial.println("Rain : Light Rain");
      }
  }
  else if(Medium_Threshold > Rain_Sens_Val && Rain_Sens_Val > Heavy_Threshold){
      No_Rain = false;      
      Light_Rain = false;
      Medium_Rain = true;
      Heavy_Rain = false;
      if(Speed_Limit_Report){
        Serial.println("Rain : Medium Rain");
      }
  }
  else if(Heavy_Threshold > Rain_Sens_Val){
      No_Rain = false;    
      Light_Rain = false;
      Medium_Rain = false;
      Heavy_Rain = true;
      if(Speed_Limit_Report){
        Serial.println("Rain : Heavy Rain");
      }
  }

  //Temperature Readings
  Current_Measure = millis();
  if((Current_Measure - Last_Measure) > 2000){
    Last_Measure = Current_Measure;
    float t = dht.readTemperature();
    Temperature_Val = dht.computeHeatIndex(t, false, false);    // Uncomment this line to read the actual temperature from the room 
    //Temperature_Val = 0;                                      // Uncomment this line to set temperature to 0 manually in software
    if(Temperature_Val <= Temperature_Threshold){
      Icy_Road = true;
    }
    else{
      Icy_Road = false;      
    }
  }
  if(Speed_Limit_Report){
    Serial.print("Temperature: ");
    Serial.println(Temperature_Val);
  }
  
  if(No_Rain){
    if(!Drying_Light || !Drying_Medium || !Drying_Heavy){
       Speed_Limit = 80;
    }
  }
  else if(Icy_Road){
    Speed_Limit = 20;
  }
  else if(Light_Rain){
     if(!Drying_Medium || !Drying_Heavy){
       Speed_Limit = 60;
    }
  }
  else if(Medium_Rain){
     if(!Drying_Heavy){
       Speed_Limit = 50;
    }
  }
  else if(Heavy_Rain){
    Speed_Limit = 40;
  }
  if(Speed_Limit_Report){
    Serial.print("Desired Speed Limit: ");
    Serial.println(Speed_Limit);
  }

  if(Prev_Speed_Limit < Speed_Limit){
    if(Prev_Speed_Limit == 40 || Prev_Speed_Limit == 20){
      Drying_Heavy = true;
      if(Speed_Limit_Report){
        Serial.println("Drying: Drying Heavy");
      }
    }
    else if(Prev_Speed_Limit == 50){
      Drying_Medium = true;
      if(Speed_Limit_Report){
        Serial.println("Drying: Drying Medium");
      }
    }
    else if(Prev_Speed_Limit == 60){
      Drying_Light = true;
      if(Speed_Limit_Report){
        Serial.println("Drying: Drying Light");
      }
    }
    
    if(Drying_First_Time){
      Drying_First_Time = false;
      if(Drying_Heavy){
        Prev_Time_Heavy = millis();
      }
      else if(Drying_Medium){
        Prev_Time_Medium = millis();
      }
      else if(Drying_Light){
        Prev_Time_Light = millis();
      }
    }

    else{
      Current_Time = millis();
      Light_Sens_Val = 1024 - analogRead(Light_Sens_Pin);
      if(Speed_Limit_Report){
        Serial.print("Light_Sens_Val: ");
        Serial.println(Light_Sens_Val);
      }
      Temporary_Val = ((float)(Light_Sens_Val-Min_Light_Sens_Val)/(float)(Max_Light_Sens_Val-Min_Light_Sens_Val));
      if(Speed_Limit_Report){
        Serial.print("Temporary_Val 1: ");
        Serial.println(Temporary_Val);
      }
      Temporary_Val = (float)Max_Drying_Delay*Temporary_Val;
      if(Speed_Limit_Report){
        Serial.print("Temporary_Val 2: ");
        Serial.println(Temporary_Val);
      }
      Drying_Delay = Max_Drying_Delay - Temporary_Val;
      if(Speed_Limit_Report){
        Serial.print("Drying Delay: ");
        Serial.println(Drying_Delay);
      }
      if(Drying_Heavy){
        if((Current_Time - Prev_Time_Heavy) > Drying_Delay){
          if(Speed_Limit_Report){
            Serial.println("Drying_Heavy --> Drying_Medium");
          }
          Drying_Heavy = false;
          Drying_Medium = true;
          Prev_Speed_Limit = 50;
          Drying_First_Time = true;
        }
      }
      else if(Drying_Medium){
        if((Current_Time - Prev_Time_Medium) > Drying_Delay){
          if(Speed_Limit_Report){
            Serial.println("Drying_Medium --> Drying_Light");
          }
          Drying_Medium = false;
          Drying_Light = true;
          Prev_Speed_Limit = 60;
          Drying_First_Time = true;
        }
      }
      else if(Drying_Light){
        if((Current_Time - Prev_Time_Light) > Drying_Delay){
          if(Speed_Limit_Report){
            Serial.println("Drying_Light --> No Drying");
          }
          Drying_Light = false;
          Prev_Speed_Limit = 80;
          Drying_First_Time = true;
        }
      }
    }
  }
  else{
    if(Speed_Limit_Report){
      Serial.println("Drying: No Drying");
    }
    Prev_Speed_Limit = Speed_Limit;
  }
  if(Drying_Heavy || Drying_Medium || Drying_Light){
    Speed_Limit = Prev_Speed_Limit;
  }
  else{
    Prev_Speed_Limit = Speed_Limit;
  }
  /*Serial.print("Speed Limit Set: ");
  Serial.println(Speed_Limit);
  Serial.println();
  Serial.println();
  Serial.println();*/
}

//========================================== Display ============================================
void Set_Display(){
  if(Speed_Limit == 80){
    digitalWrite (R_A_Pin,0);
    digitalWrite (R_B_Pin,0);
    digitalWrite (R_C_Pin,0);
    digitalWrite (R_D_Pin,0);
    digitalWrite (R_E_Pin,0);
    digitalWrite (R_F_Pin,0);
    digitalWrite (R_G_Pin,1);
    digitalWrite (R_DP_Pin,1);

    digitalWrite (L_A_Pin,0);
    digitalWrite (L_B_Pin,0);
    digitalWrite (L_C_Pin,0);
    digitalWrite (L_D_Pin,0);
    digitalWrite (L_E_Pin,0);
    digitalWrite (L_F_Pin,0);
    digitalWrite (L_G_Pin,0);
    digitalWrite (L_DP_Pin,1);
  }
  else if(Speed_Limit == 60){
    digitalWrite (R_A_Pin,0);
    digitalWrite (R_B_Pin,0);
    digitalWrite (R_C_Pin,0);
    digitalWrite (R_D_Pin,0);
    digitalWrite (R_E_Pin,0);
    digitalWrite (R_F_Pin,0);
    digitalWrite (R_G_Pin,1);
    digitalWrite (R_DP_Pin,1);

    digitalWrite (L_A_Pin,0);
    digitalWrite (L_B_Pin,1);
    digitalWrite (L_C_Pin,0);
    digitalWrite (L_D_Pin,0);
    digitalWrite (L_E_Pin,0);
    digitalWrite (L_F_Pin,0);
    digitalWrite (L_G_Pin,0);
    digitalWrite (L_DP_Pin,1);
  }
  else if(Speed_Limit == 50){
    digitalWrite (R_A_Pin,0);
    digitalWrite (R_B_Pin,0);
    digitalWrite (R_C_Pin,0);
    digitalWrite (R_D_Pin,0);
    digitalWrite (R_E_Pin,0);
    digitalWrite (R_F_Pin,0);
    digitalWrite (R_G_Pin,1);
    digitalWrite (R_DP_Pin,1);

    digitalWrite (L_A_Pin,0);
    digitalWrite (L_B_Pin,1);
    digitalWrite (L_C_Pin,0);
    digitalWrite (L_D_Pin,0);
    digitalWrite (L_E_Pin,1);
    digitalWrite (L_F_Pin,0);
    digitalWrite (L_G_Pin,0);
    digitalWrite (L_DP_Pin,1);
  }
  else if(Speed_Limit == 40){
    digitalWrite (R_A_Pin,0);
    digitalWrite (R_B_Pin,0);
    digitalWrite (R_C_Pin,0);
    digitalWrite (R_D_Pin,0);
    digitalWrite (R_E_Pin,0);
    digitalWrite (R_F_Pin,0);
    digitalWrite (R_G_Pin,1);
    digitalWrite (R_DP_Pin,1);

    digitalWrite (L_A_Pin,1);
    digitalWrite (L_B_Pin,0);
    digitalWrite (L_C_Pin,0);
    digitalWrite (L_D_Pin,1);
    digitalWrite (L_E_Pin,1);
    digitalWrite (L_F_Pin,0);
    digitalWrite (L_G_Pin,0);
    digitalWrite (L_DP_Pin,1);
  }
  else if(Speed_Limit == 20){
    digitalWrite (R_A_Pin,0);
    digitalWrite (R_B_Pin,0);
    digitalWrite (R_C_Pin,0);
    digitalWrite (R_D_Pin,0);
    digitalWrite (R_E_Pin,0);
    digitalWrite (R_F_Pin,0);
    digitalWrite (R_G_Pin,1);
    digitalWrite (R_DP_Pin,1);

    digitalWrite (L_A_Pin,0);
    digitalWrite (L_B_Pin,0);
    digitalWrite (L_C_Pin,1);
    digitalWrite (L_D_Pin,0);
    digitalWrite (L_E_Pin,0);
    digitalWrite (L_F_Pin,1);
    digitalWrite (L_G_Pin,0);
    digitalWrite (L_DP_Pin,1);
  }
}
//========================================== Bump Control ===========================================
void Control_Bumps(){
  if(Right_Pred_Speed < Speed_Limit){
    Right_Servo.write(Right_Servo_Min);
  }
  else{
    Right_Servo.write(Right_Servo_Max);
  }
  if(Left_Pred_Speed < Speed_Limit){
    Left_Servo.write(Left_Servo_Min);
  }
  else{
    Left_Servo.write(Left_Servo_Max);
  }
}

//===================================================================================================
//===================================================================================================
//========================================== VOID SETUP =============================================
//===================================================================================================
//===================================================================================================
void setup() {
  dht.begin();

  Left_Servo.attach(Left_Servo_Pin);
  Right_Servo.attach(Right_Servo_Pin);
  
  pinMode(Left_Coils_Pin, INPUT);
  pinMode(Right_Coils_Pin, INPUT);

  pinMode(L_A_Pin, OUTPUT);
  pinMode(L_B_Pin, OUTPUT);
  pinMode(L_C_Pin, OUTPUT);
  pinMode(L_D_Pin, OUTPUT);
  pinMode(L_E_Pin, OUTPUT);
  pinMode(L_F_Pin, OUTPUT);
  pinMode(L_G_Pin, OUTPUT);
  pinMode(L_DP_Pin, OUTPUT);
  
  pinMode(R_A_Pin, OUTPUT);
  pinMode(R_B_Pin, OUTPUT);
  pinMode(R_C_Pin, OUTPUT);
  pinMode(R_D_Pin, OUTPUT);
  pinMode(R_E_Pin, OUTPUT);
  pinMode(R_F_Pin, OUTPUT);
  pinMode(R_G_Pin, OUTPUT);
  pinMode(R_DP_Pin, OUTPUT);
  
  Serial.begin(9600);
  while(digitalRead(Right_Coils_Pin) == HIGH || digitalRead(Left_Coils_Pin) == HIGH){
    delay(50);
  }
  Serial.println("Ready !");
}

//===================================================================================================
//===================================================================================================
//=========================================== VOID LOOP =============================================
//===================================================================================================
//===================================================================================================
void loop() {
  Left_Coils();
  Right_Coils();
  Set_Speed_Limit();
  Set_Display();
  Control_Bumps();
}

