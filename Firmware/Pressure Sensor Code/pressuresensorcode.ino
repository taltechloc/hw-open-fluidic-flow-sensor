const int APin3 = 25;// Analog pin
const int APin4 = 26;// Analog pin

int ADCValue3;// do not change
int ADCValue4;// do not change
float voltage3 =0;// do not change
float voltage4 =0;// do not change
//const int threshol = 1.00;
float Pressure1 = 0;
float Pressure2 = 0;
float Pressure1Pa = 0;
float Pressure2Pa = 0;
float DeltaP = 0;
float velocity2=0;

const int numReadings = 50;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
float total = 0;                  // the running total
float average = 0.0;                // the average

HardwareSerial Sender(1);   // Define a Serial port instance called 'Sender' using serial port 1

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
}


void loop() {
  // read the input on analog pin potPin:
  ADCValue3 = analogRead(APin3);
  voltage3 = (3.5/4095.0) * ADCValue3;
  ADCValue4 = analogRead(APin4);
  voltage4 = (3.48/4095.0) * ADCValue4;

  Pressure1 = ((voltage3-0.33)*2/2.64) - 1; //psi
  Pressure1Pa = (Pressure1 * 6594.0);
  Pressure2 = ((voltage4-0.33)*2/2.64) - 1; //psi
  Pressure2Pa = (Pressure2 * 6594.0);
   if(Pressure1Pa >= Pressure2Pa){
    DeltaP = Pressure1Pa - Pressure2Pa;
    }
  
  Serial.print("ADCValue3:");
  Serial.print(ADCValue3);
   
  Serial.print(" Voltage3:");
  Serial.print(voltage3);
  Serial.println("V"); 
  
  Serial.print("ADCValue4:");
  Serial.print(ADCValue4);
   
  Serial.print(" Voltage4:");
  Serial.print(voltage4);
  Serial.println("V");
  Serial.print(" Pressure1 Psi:");   
  Serial.println(Pressure1);
  Serial.print(" Pressure2 Psi:"); 
  Serial.println(Pressure2);
  Serial.print(" Pressure1 Pa:"); 
  Serial.println(Pressure1Pa);
  Serial.print(" Pressure2 Pa:"); 
  Serial.println(Pressure2Pa);
  Serial.print(" DeltaP Pa:"); 
  Serial.println(DeltaP);
  Serial.print(" Velocity:"); 
  Serial.println(velocity2);
  delay(100); // delay in between reads for stability
  
}
