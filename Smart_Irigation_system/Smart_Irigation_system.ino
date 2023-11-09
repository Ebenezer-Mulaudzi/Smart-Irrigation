#define BLYNK_TEMPLATE_ID "TMP ID"
#define BLYNK_TEMPLATE_NAME "TMP NAME"

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFiClient.h>
#include <WiFi.h>

// Define the pins for the DHT11 sensor
#define DHTPIN 14 // Use any digital pin
const int LDRPIN = 26; // Use any analog pin
#define RLED 27
#define GLED 25
#define WPUMP 2
const int soilMoisturePin = 12;

bool relayState  = false;


BlynkTimer timer;

// Enter your Auth token
char auth[] = "AUTH TOKEN";

//Enter your WIFI SSID and password
char ssid[] = "SSID";
char pass[] = "PASSWRD";


// Initialize DHT sensor
DHT dht(DHTPIN, DHT11);

// Define LCD parameters
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the I2C address and LCD dimensions

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  //Blynk.begin(auth, ssid, pass);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 8080);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();


  pinMode(LDRPIN, INPUT);
  pinMode(GLED, OUTPUT);
  pinMode(RLED, OUTPUT);
  pinMode(WPUMP, OUTPUT);
  //pinMode(soilMoisturePin, INPUT);
  //digitalWrite(WPUMP, LOW);

  lcd.setCursor(0, 0);
  lcd.print("  Initializing  ");
  for (int a = 5; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(11, 1);
  lcd.print("W:OFF");

  //Call the function
  timer.setInterval(100L, soilMoisture);
  timer.setInterval(100L, DHT11sensor);
  timer.setInterval(500L, LightIntensity);
}

void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);

}

void LightIntensity()
{
  int lightIntensity = analogRead(LDRPIN);
  lightIntensity = map(lightIntensity, 0, 4095, 0, 100);
  lightIntensity = (lightIntensity - 100) * -1;
  Serial.println(lightIntensity);
  Blynk.virtualWrite(V2, lightIntensity);

}



void soilMoisture() {
  int value = analogRead(soilMoisturePin);
  value = map(value, 0, 4095, 0, 100);
  value = (value - 100) * -1;
  Blynk.virtualWrite(V3, value);
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(value);
  lcd.print(" ");
}

//Get the button value
BLYNK_WRITE(V7) 
{
  bool mode = param.asInt();
  

  if (mode == 1) 
  {
    digitalWrite(RLED, LOW);
    digitalWrite(GLED, HIGH);
    Blynk.virtualWrite(V5, 1);
    Blynk.virtualWrite(V6, 0);

    int value = analogRead(soilMoisturePin);
    value = map(value, 0, 4095, 0, 100);
    value = (value - 100) * -1;

    if(value <= 46)
    {
      digitalWrite(WPUMP, LOW); 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SOIL DRY");
      lcd.setCursor(4, 1);
      lcd.print("WATERING");
      delay(3000);
      lcd.clear();
    }
    else if(value >= 90)
    {
      digitalWrite(WPUMP, HIGH); 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SOIL WET");
      lcd.setCursor(4, 1);
      lcd.print("STOP WATERING");
      delay(3000);
      lcd.clear();
    }


  } 
  else 
  {
   
    digitalWrite(RLED, HIGH);
    digitalWrite(GLED, LOW);
    Blynk.virtualWrite(V5, 0);
    Blynk.virtualWrite(V6, 1);

    
    
  }
}

BLYNK_WRITE(V8)
{
  bool state = param.asInt();
  if(state == 1)
  {
    digitalWrite(WPUMP, LOW); 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IRRIGATION SYSTEM");
    lcd.setCursor(4, 1);
    lcd.print("WATERING");
    delay(3000);
    lcd.clear();

  }
  else
  {
    digitalWrite(WPUMP, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IRRIGATION SYSTEM");
    lcd.setCursor(2, 1);
    lcd.print("! WATERING");
    delay(3000);
    lcd.clear();
  }
}

void loop() {
 
  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer
}
