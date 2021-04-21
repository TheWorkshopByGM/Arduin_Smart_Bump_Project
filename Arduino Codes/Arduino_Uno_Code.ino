//Guy Maalouf - 25/4/2019
//Smart Bump Project
// This code is for the arduino Unos that will be connected to the multiplexers and the coils

//Pin Variables
int Sel1_Pin = 9;
int Sel2_Pin = 8;
int Sel3_Pin = 7;

int Output_Pin = 6;


//_______________________________________
//_______________________________________
//General Variables

int Select = 0;
int minVal = 200;
float sensitivity = 10;

int Individual_Delay = 50;
int Inter_Coils_Delay = 100;


//_______________________________________
//_______________________________________
// Interrupt Variables
#define CYCLES_PER_SIGNAL 5000

unsigned long lastSignalTime1 = 0;
unsigned long lastSignalTime2 = 0;
unsigned long lastSignalTime3 = 0;
unsigned long lastSignalTime4 = 0;
unsigned long lastSignalTime5 = 0;
unsigned long lastSignalTime6 = 0;

unsigned long signalTimeDelta1 = 0;
unsigned long signalTimeDelta2 = 0;
unsigned long signalTimeDelta3 = 0;
unsigned long signalTimeDelta4 = 0;
unsigned long signalTimeDelta5 = 0;
unsigned long signalTimeDelta6 = 0;

int Bias1 = 0;
int Bias2 = 0;
int Bias3 = 0;
int Bias4 = 0;
int Bias5 = 0;
int Bias6 = 0;

boolean firstSignal1 = true;
boolean firstSignal2 = true;
boolean firstSignal3 = true;
boolean firstSignal4 = true;
boolean firstSignal5 = true;
boolean firstSignal6 = true;

unsigned long storedTimeDelta1 = 0;
unsigned long storedTimeDelta2 = 0;
unsigned long storedTimeDelta3 = 0;
unsigned long storedTimeDelta4 = 0;
unsigned long storedTimeDelta5 = 0;
unsigned long storedTimeDelta6 = 0;

//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
// This signal is called whenever OCR1A reaches 0
// (Note: OCR1A is decremented on every external clock cycle)
SIGNAL(TIMER1_COMPA_vect)
{
  unsigned long currentTime = micros();
  if(Select == 0){
    signalTimeDelta1 =  currentTime - lastSignalTime1;
    lastSignalTime1 = currentTime;
    if (firstSignal1){
      firstSignal1 = false;
    }
    else if (storedTimeDelta1 == 0){
      storedTimeDelta1 = signalTimeDelta1;
    }
  }
  else if(Select == 1){
    signalTimeDelta2 =  currentTime - lastSignalTime2;
    lastSignalTime2 = currentTime;
    if (firstSignal2){
      firstSignal2 = false;
    }
    else if (storedTimeDelta2 == 0){
      storedTimeDelta2 = signalTimeDelta2;
    }
  }
  else if(Select == 2){
    signalTimeDelta3 =  currentTime - lastSignalTime3;
    lastSignalTime3 = currentTime;
    if (firstSignal3){
      firstSignal3 = false;
    }
    else if (storedTimeDelta3 == 0){
      storedTimeDelta3 = signalTimeDelta3;
    }
  }
  else if(Select == 3){
    signalTimeDelta4 =  currentTime - lastSignalTime4;
    lastSignalTime4 = currentTime;
    if (firstSignal4){
      firstSignal4 = false;
    }
    else if (storedTimeDelta4 == 0){
      storedTimeDelta4 = signalTimeDelta4;
    }
  }
  else if(Select == 4){
    signalTimeDelta5 =  currentTime - lastSignalTime5;
    lastSignalTime5 = currentTime;
    if (firstSignal5){
      firstSignal5 = false;
    }
    else if (storedTimeDelta5 == 0){
      storedTimeDelta5 = signalTimeDelta5;
    }
  }
  else if(Select == 5){
    signalTimeDelta6 =  currentTime - lastSignalTime6;
    lastSignalTime6 = currentTime;
    if (firstSignal6){
      firstSignal6 = false;
    }
    else if (storedTimeDelta6 == 0){
      storedTimeDelta6 = signalTimeDelta6;
    }
  }

  // Reset OCR1A
  OCR1A += CYCLES_PER_SIGNAL;
}

void setup()
{
  // Set WGM(Waveform Generation Mode) to 0 (Normal)
  TCCR1A = 0b00000000; //prescaler
  
  // Set CSS(Clock Speed Selection) to 0b111 (External clock source on T0 pin
  // (ie, pin 5 on UNO). Clock on rising edge.)
  TCCR1B = 0b00000111;

  // Enable timer compare interrupt A (ie, SIGNAL(TIMER1_COMPA_VECT))
  TIMSK1 |= (1 << OCIE1A);

  // Set OCR1A (timer A counter) to 1 to trigger interrupt on next cycle
  OCR1A = 1;

  Select = 0;
  
  pinMode(Sel1_Pin,OUTPUT);
  pinMode(Sel2_Pin,OUTPUT);
  pinMode(Sel3_Pin,OUTPUT);
  pinMode(Output_Pin,OUTPUT);
  
  digitalWrite(Sel1_Pin,LOW);
  digitalWrite(Sel2_Pin,LOW);
  digitalWrite(Sel3_Pin,LOW);
  digitalWrite(Output_Pin,HIGH);
  
  Serial.begin(9600);
  
  storedTimeDelta1 = 0;
  storedTimeDelta2 = 0;
  storedTimeDelta3 = 0;
  storedTimeDelta4 = 0;
  storedTimeDelta5 = 0;
  storedTimeDelta6 = 0;

for(int i = 0; i<10; i++){
   int storedTimeDeltaDifference1 = 0;
   int storedTimeDeltaDifference2 = 0;
   int storedTimeDeltaDifference3 = 0;
   int storedTimeDeltaDifference4 = 0;
   int storedTimeDeltaDifference5 = 0;
   int storedTimeDeltaDifference6 = 0;
   //Coil 1
   if(Select == 0){
     Select = 1;
     digitalWrite(Sel1_Pin,HIGH);
     digitalWrite(Sel2_Pin,LOW);
     digitalWrite(Sel3_Pin,LOW);
     storedTimeDeltaDifference1 = (storedTimeDelta1 - signalTimeDelta1) * sensitivity - Bias1;
     if(i == 5){
      Bias1 = storedTimeDeltaDifference1;
     }
     Serial.print("Coil 1: ");
     Serial.print(storedTimeDeltaDifference1);
     delay(Inter_Coils_Delay);
   }
   
   //Coil 2
   if(Select == 1){
     Select = 2;
     digitalWrite(Sel1_Pin,LOW);
     digitalWrite(Sel2_Pin,HIGH);
     digitalWrite(Sel3_Pin,LOW);
     storedTimeDeltaDifference2 = (storedTimeDelta2 - signalTimeDelta2) * sensitivity - Bias2;
     if(i == 5){
      Bias2 = storedTimeDeltaDifference2;
     }
     Serial.print("  -  Coil 2: ");
     Serial.print(storedTimeDeltaDifference2);
     delay(Inter_Coils_Delay);
   }
 
   //Coil 3
   if(Select == 2){
     Select = 3;
     digitalWrite(Sel1_Pin,HIGH);
     digitalWrite(Sel2_Pin,HIGH);
     digitalWrite(Sel3_Pin,LOW);
     storedTimeDeltaDifference3 = (storedTimeDelta3 - signalTimeDelta3) * sensitivity - Bias3;
     if(i == 5){
      Bias3 = storedTimeDeltaDifference3;
     }    
     Serial.print("  -  Coil 3: ");
     Serial.print(storedTimeDeltaDifference3);
     delay(Inter_Coils_Delay);
   }
   
   //Coil 4
   if(Select == 3){
     Select = 4;
     digitalWrite(Sel1_Pin,LOW);
     digitalWrite(Sel2_Pin,LOW); 
     digitalWrite(Sel3_Pin,HIGH);   
     storedTimeDeltaDifference4 = (storedTimeDelta4 - signalTimeDelta4) * sensitivity - Bias4;
     if(i == 5){
      Bias4 = storedTimeDeltaDifference4;
     } 
     Serial.print("  -  Coil 4: ");
     Serial.print(storedTimeDeltaDifference4);
     delay(Inter_Coils_Delay);
   }
   
   //Coil 5
   if(Select == 4){
     Select = 5;
     digitalWrite(Sel1_Pin,HIGH);
     digitalWrite(Sel2_Pin,LOW); 
     digitalWrite(Sel3_Pin,HIGH);   
     storedTimeDeltaDifference5 = (storedTimeDelta5 - signalTimeDelta5) * sensitivity - Bias5;
     if(i == 5){
      Bias5 = storedTimeDeltaDifference5;
     } 
     Serial.print("  -  Coil 5: ");
     Serial.print(storedTimeDeltaDifference5);
     delay(Inter_Coils_Delay);
   }

   //Coil 6
   if(Select == 5){
     Select = 0;
     digitalWrite(Sel1_Pin,LOW);
     digitalWrite(Sel2_Pin,LOW); 
     digitalWrite(Sel3_Pin,LOW);   
     storedTimeDeltaDifference6 = (storedTimeDelta6 - signalTimeDelta6) * sensitivity - Bias6;
     if(i == 5){
      Bias6 = storedTimeDeltaDifference6;
     } 
     Serial.print("  -  Coil 6: ");
     Serial.println(storedTimeDeltaDifference6);
     delay(Inter_Coils_Delay);
   }
   if(i == 9){
    Serial.println("INITILIZATION DONE");
    Serial.print("Coil 1: ");
    Serial.print(storedTimeDeltaDifference1);
    Serial.print("  -  Coil 2: ");
    Serial.print(storedTimeDeltaDifference2);
    Serial.print("  -  Coil 3: ");
    Serial.print(storedTimeDeltaDifference3);
    Serial.print("  -  Coil 4: ");
    Serial.print(storedTimeDeltaDifference4);
    Serial.print("  -  Coil 5: ");
    Serial.print(storedTimeDeltaDifference5);
    Serial.print("  -  Coil 6: ");
    Serial.println(storedTimeDeltaDifference6);
   }
  }
  
  Select = 0;
  digitalWrite(Sel1_Pin,LOW);
  digitalWrite(Sel2_Pin,LOW); 
  digitalWrite(Sel3_Pin,LOW); 
  digitalWrite(Output_Pin,LOW); 
  delay(Inter_Coils_Delay); 
}

void loop(){
   //Coil 1
   if(Select == 0){
     int storedTimeDeltaDifference1 = (storedTimeDelta1 - signalTimeDelta1) * sensitivity - Bias1;
     if(storedTimeDeltaDifference1>minVal){
      Serial.print(">>>>>>>>>>>>> Coil 1 Detected ! <<<<<<<<<<<<<<");  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      digitalWrite(Output_Pin,HIGH);
      Serial.print("Micros: ");
      Serial.println(micros());
      Select = 1;
      digitalWrite(Sel1_Pin,HIGH);
      digitalWrite(Sel2_Pin,LOW);
      digitalWrite(Sel3_Pin,LOW);
      delay(Inter_Coils_Delay);
     }
     Serial.print("Coil 1: ");                                        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     Serial.println(storedTimeDeltaDifference1);                      //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     delay(Individual_Delay);
   }
   
   //Coil 2
   if(Select == 1){
     int storedTimeDeltaDifference2 = (storedTimeDelta2 - signalTimeDelta2) * sensitivity - Bias2;
     if(storedTimeDeltaDifference2>minVal){
      Serial.print(">>>>>>>>>>>>> Coil 2 Detected ! <<<<<<<<<<<<<<");  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      digitalWrite(Output_Pin,LOW);
      Select = 2;
      digitalWrite(Sel1_Pin,LOW);
      digitalWrite(Sel2_Pin,HIGH);
      digitalWrite(Sel3_Pin,LOW);
      delay(Inter_Coils_Delay);
     }
     Serial.print("Coil 2: ");                                            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     Serial.println(storedTimeDeltaDifference2);                          //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     delay(Individual_Delay);
   }

   //Coil 3
   if(Select == 2){
     int storedTimeDeltaDifference3 = (storedTimeDelta3 - signalTimeDelta3) * sensitivity - Bias3;
     if(storedTimeDeltaDifference3>minVal){
      Serial.print(">>>>>>>>>>>>> Coil 3 Detected ! <<<<<<<<<<<<<<");  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      digitalWrite(Output_Pin,HIGH);
      Select = 3;
      digitalWrite(Sel1_Pin,HIGH);
      digitalWrite(Sel2_Pin,HIGH);
      digitalWrite(Sel3_Pin,LOW);
      delay(Inter_Coils_Delay);
     }
     Serial.print("Coil 3: ");                                            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<          
     Serial.println(storedTimeDeltaDifference3);                          //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     delay(Individual_Delay);
   }

   //Coil 4
   if(Select == 3){
     int storedTimeDeltaDifference4 = (storedTimeDelta4 - signalTimeDelta4) * sensitivity - Bias4;
     if(storedTimeDeltaDifference4>minVal){
      Serial.print(">>>>>>>>>>>>> Coil 4 Detected ! <<<<<<<<<<<<<<");  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      digitalWrite(Output_Pin,LOW);
      Select = 4;
      digitalWrite(Sel1_Pin,LOW);
      digitalWrite(Sel2_Pin,LOW);
      digitalWrite(Sel3_Pin,HIGH);
      delay(Inter_Coils_Delay);
     }
     Serial.print("Coil 4: ");                                            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     Serial.println(storedTimeDeltaDifference4);                          //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     delay(Individual_Delay);
   }

   //Coil 5
   if(Select == 4){
     int storedTimeDeltaDifference5 = (storedTimeDelta5 - signalTimeDelta5) * sensitivity - Bias5;
     if(storedTimeDeltaDifference5>minVal){
      Serial.print(">>>>>>>>>>>>> Coil 5 Detected ! <<<<<<<<<<<<<<");  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      digitalWrite(Output_Pin,HIGH);
      Select = 5;
      digitalWrite(Sel1_Pin,HIGH);
      digitalWrite(Sel2_Pin,LOW);
      digitalWrite(Sel3_Pin,HIGH);
      delay(Inter_Coils_Delay);
     }
     Serial.print("Coil 5: ");                                            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     Serial.println(storedTimeDeltaDifference5);                          //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     delay(Individual_Delay);
   }

   //Coil 6
   if(Select == 5){
     int storedTimeDeltaDifference6 = (storedTimeDelta6 - signalTimeDelta6) * sensitivity - Bias6;
     if(storedTimeDeltaDifference6>minVal){
      //Serial.print(">>>>>>>>>>>>> Coil 6 Detected ! <<<<<<<<<<<<<<");  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      digitalWrite(Output_Pin,LOW);
      Select = 0;
      digitalWrite(Sel1_Pin,LOW);
      digitalWrite(Sel2_Pin,LOW);
      digitalWrite(Sel3_Pin,LOW);
      delay(Inter_Coils_Delay);
     }
     Serial.print("Coil 6: ");                                            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     Serial.println(storedTimeDeltaDifference6);                          //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     delay(Individual_Delay);
   }
}
