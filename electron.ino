SYSTEM_MODE(SEMI_AUTOMATIC);

float MoistFilt[10];
int mesuring_done = 0;
int CellCon = 0;

void setup()
{
 Serial.begin(9600);

}

float Moisture()
{
int A1_Raw;
float tmp;
float Moist;
float Sum;
float y;

for (int i = 0; i < 10 ; i ++)
{
 A1_Raw = analogRead(A0);
 y =  A1_Raw;
 tmp = (3.3*y)/4096;
 if (tmp > 0 and tmp <= 1.1){  Moist = 10*tmp-1; }
 if (tmp > 1.1 and tmp <= 1.3){  Moist = 25*tmp-17.5; }
 if (tmp > 1.3 and tmp <= 1.82){  Moist = 48.08*tmp-47.5; }
 if (tmp > 1.82){  Moist = 26.32*tmp-7.89; }
 MoistFilt[i] = Moist;
}
 Sum = 0;
 for (int i = 0; i < 10 ; i++) {Sum = Sum + MoistFilt[i];}
 Sum = Sum / 10;
 return Sum;
}

void loop()
{
//   Serial.println("Loop calling...");
   int index = 0;
   float value = 0;
   String stringOne = "";

   EEPROM.get(0, index);

   if (mesuring_done == 0) 
   {

	   index = index + 1;

	   value = Moisture();

	   EEPROM.put(index*4, value);

	   Serial.println(String::format("%3.1f",value));   
	   Serial.println(index);
	   mesuring_done = 1;
	   EEPROM.put(0, index);   
   }
 
   if (index > 47)
   {
     if (Particle.connected() == false) 
		{
		        
			if (CellCon = 0) { Cellular.on(); CellCon = 1;}
			
			if (!Cellular.ready()) {Cellular.connect();}
		        if (Cellular.ready())  {Particle.connect();}

		}
     if (Particle.connected() == true)
     {
	     Serial.println("Cloud ready");
	     for(int i=1;i<48;i++)
		{
			EEPROM.get(i*4,value);
			stringOne = stringOne + String::format("%3.1f",value) + ",";
		}

	     stringOne = String(Time.now()) + "," + stringOne.remove(stringOne.length()-1);
	     Serial.println(stringOne);
        
	     bool sent = Particle.publish("M", stringOne);

	     if ( sent == true) 	     Serial.println("Publish good");

	     delay(2000);

	     index = 0;
	     EEPROM.put(0, index);   
	     Particle.disconnect();
	     Cellular.off();
     }
   }




//if (index < 48)  {delay(5000);	   mesuring_done = 0;}
if (index < 48)  {Cellular.off();System.sleep(SLEEP_MODE_DEEP, 75);}

}
