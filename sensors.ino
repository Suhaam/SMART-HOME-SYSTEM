#include <EEPROM.h>
#include <EasyButton.h>
#include <LiquidCrystal.h>
#include <dht.h>

// board led
const int board_led = 13;

// eeprom value
byte eeprom_value;

// timer for sensor and delay
long last_read_from_sensor = 0;
const long sensor_read_delay = 10000;

// sensor pin
#define DHT_PIN A5

// sensor setup
dht DHT;

// maximum and minimum
const int min_value = 0;
const int max_value = 50;

// first value
const int first_value_up_pin = 10;
const int first_value_down_pin = 11;

int first_value = 13;
const int eeprom_address_first_value = 0;

// second value
const int second_value_up_pin = 12;
const int second_value_down_pin = 13;

int second_value = 22;
const int eeprom_address_second_value = 1;

// lcd setup
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// outputs
const int first_output_pin = 14;
const int second_output_pin = 15;

// buttons setup
EasyButton button_first_up(first_value_up_pin);
EasyButton button_first_down(first_value_down_pin);
EasyButton button_second_up(second_value_up_pin);
EasyButton button_second_down(second_value_down_pin);

// temperature setup
double current_temperature;
double current_humidity;

// configurable lcd messages setup
const char *message[] = {
    "Loading...", // loading screen
    "Max", // max output speed
    "Min", // min output speed
    "Off", // off message
    "T", // temperature
    "H" // humidity
};

void setup()
{ 
  // write boot message on lcd
  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  lcd.print(message[0]);
  
  // turn off board led
  pinMode(board_led, OUTPUT);
  digitalWrite(board_led, LOW);
  
  // set the two outputs as high
  pinMode(first_output_pin, OUTPUT);
  digitalWrite(first_output_pin, HIGH);
  pinMode(second_output_pin, OUTPUT);
  digitalWrite(second_output_pin, HIGH);
  
  // initialise serial
  Serial.begin(9600);
  
  // load first value from eeprom
  eeprom_value = EEPROM.read(eeprom_address_first_value);
  if(eeprom_value != 255)
  {
    first_value = eeprom_value;
  }

  // load second value from eeprom
  eeprom_value = EEPROM.read(eeprom_address_second_value);
  if(eeprom_value != 255)
  {
    second_value = eeprom_value;
  }
  
  // print two temperature intervals on lcd
  printIntervals(first_value, second_value);
}

void loop()
{ 
  // read buttons
  button_first_up.update();
  button_first_down.update();
  button_second_up.update();
  button_second_down.update();

  // increase value if it is high, if it is less than the maximum value, and the first value is less than the second one
  if(button_first_up.IsPushed() && first_value < max_value && first_value < (second_value - 1))
  {
    // increase first_value
    first_value++;      
    // write on eeprom
    EEPROM.write(eeprom_address_first_value, first_value);
    // update two temperature intervals on lcd
    printIntervals(first_value, second_value);
  }
  
  // decrease value if it is high and if the value is more than the minimum value
  if(button_first_down.IsPushed() && first_value > min_value)
  {
    // decrease first_value
    first_value--;   
    // write on eeprom
    EEPROM.write(eeprom_address_first_value, first_value);
    // update two temperature intervals on lcd
    printIntervals(first_value, second_value);
  }

  // increase value if it is high and if it is less than the maximum value
  if(button_second_up.IsPushed() == HIGH && second_value < max_value)
  {
    // increase second_value
    second_value++;
    // write on eeprom
    EEPROM.write(eeprom_address_second_value, second_value);
    // update two temperature intervals on lcd
    printIntervals(first_value, second_value);
  }

  // decrease value if it is high, if the value is more than the minimum value and if the first value is less than the second one
  if(button_second_down.IsPushed() && second_value > min_value && first_value < (second_value - 1))
  {
    // decrease second_value
    second_value--;
    // write on eeprom
    EEPROM.write(eeprom_address_second_value, second_value);
    // update two temperature intervals on lcd
    printIntervals(first_value, second_value);
  }
  
  // sampling of temperature and humidity after delay
  if((millis() - last_read_from_sensor) > sensor_read_delay)
  {
    // read the temperature
    int chk = DHT.read22(DHT_PIN);

    switch (chk)
    {
      case DHTLIB_OK:  
        Serial.print(F("OK, "));

        current_temperature = DHT.temperature;
        current_humidity = DHT.humidity;

        Serial.print(F("current temperature: "));
        Serial.println(current_temperature);
        
        Serial.print(F("Current humidity: "));
        Serial.println(current_humidity);

        // print temperature values on lcd
        printTemperature(current_temperature, current_humidity);
        
        // turn outputs on/off according to logic
        if(current_temperature <= first_value)
        {
          // temperature less/equal than first_value    
          Serial.println(F("Less than first value"));

          // print output speed on lcd
          printStatus(message[1]);
          
          // full speed
          digitalWrite(first_output_pin, HIGH);
          digitalWrite(second_output_pin, LOW);
        }
        else if(current_temperature > first_value && current_temperature < second_value)
        {
          // temperature between two values     
          Serial.println(F("Between values"));

          // print speed on lcd
          printStatus(message[2]);
          
          // half speed
          digitalWrite(first_output_pin, LOW);
          digitalWrite(second_output_pin, HIGH);
        }
        else
        {
          // temperature higher/equal second_value      
          Serial.println(F("More than second value"));

          // print speed on lcd
          printStatus(message[3]);
          
          // off
          digitalWrite(first_output_pin, HIGH);
          digitalWrite(second_output_pin, HIGH);
        }
    
        break;
      case DHTLIB_ERROR_CHECKSUM: 
        Serial.println(F("Checksum error.")); 
        break;
      case DHTLIB_ERROR_TIMEOUT: 
        Serial.println(F("Time out error.")); 
        break;
      default: 
        Serial.println(F("Unknown error.")); 
        break;
    }
    
    // set the last read as now...
    last_read_from_sensor = millis(); 
  }
}

void printIntervals(int t1, int t2) {
  lcd.setCursor(8, 0);
  lcd.print(F(" "));
  // add additional spaces if we have less digits than expected on either temperatures
  if(t1 < 10) lcd.print(F(" "));
  if(t2 < 10) lcd.print(F(" "));
  lcd.print(t1);
  lcd.print((char)223);
  lcd.print(F("-"));
  lcd.print(t2);
  lcd.print((char)223);
}

void printTemperature(double temperature, double humidity) {
  lcd.setCursor(0, 0);
  lcd.print(message[4]);
  // add initial temperature space only for temperatures with less than 2 negative digits
  if(temperature > -10) lcd.print(F(" "));
  lcd.print(temperature, 1);
  lcd.print((char)223);
  // if only one digit, add final space
  if(temperature > -10 && temperature < 10) lcd.print(F(" "));
  lcd.setCursor(0, 1);
  lcd.print(message[5]);
  lcd.print(F(" "));
  lcd.print(humidity, 1);
  lcd.print(F("%"));
  // if only one digit, add final space
  if(humidity > -10 && humidity < 10) lcd.print(F(" "));
}

void printStatus(String status_string) {
  // print output status on lcd
  lcd.setCursor(7, 1);
  // print leading spaces
  int spaces = 16 - 7 - status_string.length();
  if(spaces > 0) {
    for(int i = 0; i < spaces; i++) {
      lcd.print(F(" "));
    }
  }
  lcd.print(status_string);
}
