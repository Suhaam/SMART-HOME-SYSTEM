int pir=7;
int buzz=6;
int LED=12;

void setup() 
{
 pinMode(pir INPUT);
 pinMode(buzz, OUTPUT);
 pinMode(LED,INPUT);
}

void loop() 
{
  int sensorvalue=digitalRead(pir);
  if(sensorvalue==1);
  {
    digitalWrite(buzz,HIGH);
  }
  else digitalWrite(buzz,LOW);

}
