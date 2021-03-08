

#include <Wire.h>
//#include <LCD.h>
#include <dht.h>

dht DHT;

#define DHT11_PIN 7
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x27,2,1,0,4,5,6,7,3,POSITIVE // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3

int n = 1;

LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);
int sensorPin = A0; // select the input pin for LDR

int sensorValue = 0; // variable to store the value coming from the sensor

// Local Network Settings
byte mac[]     = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Must be unique on local network
byte ip[]      = { 172,16,22,21 };                // Must be unique on local network
byte gateway[] = { 172,16,22,1};
byte subnet[]  = { 255, 255, 255, 0 };

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "CB0054N5NLTZEB7J";    // Write API Key for a ThingSpeak Channel
const int updateInterval = 10000;        // Time interval in milliseconds to update ThingSpeak  


// Variable Setup
long lastConnectionTime = 0;
boolean lastConnected = false;
int failedCounter = 0;


// Initialize Arduino Ethernet Client
//EthernetClient client;



void setup()
{
 lcd.begin (16,2); 
 lcd.setBacklight(1);      
 
// Switch on the backlight
lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
lcd.setBacklight(HIGH);
lcd.home (); // go home

//////////ethernet
//Serial.begin(115200);
//  Ethernet.begin(mac, ip, gateway, subnet);
//  delay(1000);
//  Serial.print("ETHERNET SHIELD ip  is     : ");
//  Serial.println(Ethernet.localIP());
//   // Start Ethernet on Arduino
//  startEthernet();

 lcd.print("Suhaam-Proj");  
}

void loop()
{

  int chk = DHT.read11(DHT11_PIN);
  int c;
  int relay_pin_1 = 9;
  //int relay_pin_2 = 10;
  int relay_pin_6 = 11;
 // int relay_pin_4 = 0;
 // int relay_pin_7 = 11;

    sensorValue = analogRead(sensorPin); // read the value from the sensor
    Serial.println(sensorValue); //prints the values coming from the sensor on the screen
    
    delay(1000);

  //int readData = DHT.read22(dataPin); // Reads the data from the sensor
  float t = DHT.temperature; // Gets the values of the temperature
  //float h = DHT.humidity; // Gets the values of the humidity
  float h =sensorValue;
  
 // Backlight on/off every 3 seconds
// lcd.setCursor (0,0);        // go to start of 2nd line 
// lcd.print("Displaytest ");
// lcd.setBacklight(LOW);     // Backlight on
// delay(3000);
    
  lcd.setCursor ( 0, 0 );        // go to the next line
  lcd.print ("Temp= ");
  lcd.print(t);
  lcd.print("  ");
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Lght= ");
  lcd.print(h);
  //lcd.print((char)223);
  lcd.print(" LUX");


     
pinMode(relay_pin_1, OUTPUT);
//pinMode(relay_pin_2, OUTPUT);
pinMode(relay_pin_6, OUTPUT);

digitalWrite(relay_pin_1, HIGH);
//digitalWrite(relay_pin_2, LOW);
//digitalWrite(relay_pin_6, HIGH);

//String tem = String(t);
//String hum = String(h);

char t_buffer[10];
char h_buffer[10];

String tem = dtostrf(t,0,5,t_buffer); //Convert Temperature value to String
String hum = dtostrf(h,0,5,h_buffer); //Convert Humidity value to String
Serial.println(t);
Serial.println(h);


if (t >= 21) //temp 
{

digitalWrite(relay_pin_1,LOW);

lcd.setCursor(0,1);

lcd.print(" temp Fan is ON ");
delay(1000);

}

if (h > 1000)
{

digitalWrite(relay_pin_6,HIGH);

lcd.setCursor(0,1);
delay(1000);
//lcd.print(" bulb is ON ");


}
//else
//{
//  digitalWrite(relay_pin_6,LOW);
//}
}
