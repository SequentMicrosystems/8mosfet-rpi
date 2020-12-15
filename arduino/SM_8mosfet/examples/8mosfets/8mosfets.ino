/*
 * 8mosfets
 * 
 * A 8-Mosfets card controling example
 * This example control 2 cards.
 * 
 */
#include <SM_8mosfet.h>

SM_8mosfet mos1 = SM_8mosfet();
SM_8mosfet mos2 = SM_8mosfet();

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   Serial.print("8-Mosfets card 1 init");
  if(0 == mos1.begin(0)) // set the stack level to 0
  {
    Serial.println(" Done");
  }
  else
  {
   Serial.println(" Fail!");
  }
   Serial.print("8-Mosfets card 2 init");
   if(0 == mos2.begin(1)) // set the stack level to 1
   {
    Serial.println(" Done");
  }
  else
  {
   Serial.println(" Fail!");
  }

}

void loop() {
  int i;
  // put your main code here, to run repeatedly:
  for(i = 1; i < 9; i++) //turn mosfets ON, one by one 
  {
    mos1.set(i, 1);
    mos2.set(i, 1);
    delay(250);
  }
  //Serial.println(mos.getAll());
  delay (1000);
  for(i = 1; i < 9; i++)//turn mosfets OFF, one by one 
  {
    mos1.set(i, 0);
    mos2.set(i, 0);
    delay(250);
  }
  //Serial.println(mos.getAll());
  delay (1000);
}
