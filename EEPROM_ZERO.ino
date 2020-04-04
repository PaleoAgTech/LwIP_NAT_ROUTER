#include <EEPROM.h>
void writeString(char add,String data);

void setup() { 
  delay(1000);
  Serial.begin(9600);
  EEPROM.begin(512); 
  writeString(1, "");  
  writeString(50, ""); 
  writeString(100, ""); 
  writeString(150, "");  
}
void loop() 
{ 
}
void writeString(char add,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');  
  EEPROM.commit();
}
