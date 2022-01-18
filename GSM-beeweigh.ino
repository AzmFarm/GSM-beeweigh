/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-sim800l-publish-data-to-cloud/
  http://t963139h.beget.tech/esp-chart.php
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Your GPRS credentials (leave empty, if not needed)
const char apn[]      = "internet.beeline.ru"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "beeline"; // GPRS User
const char gprsPass[] = "beeline"; // GPRS Password

const char beehiveID[] = "1"; // Идентификатор улья

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = ""; 

// Server details
// The server variable can be just a domain name or it can have a subdomain. It depends on the service you are using
const char server[] = "server.com";                // domain name: example.com, unikumuchaly.ru, etc
const char resource[] = "/post-data.php";         // resource path, for example: /Beehives/post-data.php
const int  port = 80;                             // server port number 443

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-data.php also needs to have the same key 
String apiKeyValue = "apiKeyValue";

// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

// BME280 pins
//#define I2C_SDA_2            18
//#define I2C_SCL_2            19

// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

// Define the serial console for debug prints, if needed
//#define DUMP_AT_COMMANDS

#include <Wire.h>
#include <TinyGsmClient.h>
//#include <SSLClient.h>
//#include <ArduinoHttpClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

//#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>

#include <HX711.h>
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 19;
HX711 scale;                  

// I2C for SIM800 (to keep it running when powered from battery)
TwoWire I2CPower = TwoWire(0);

// I2C for BME280 sensor
//TwoWire I2CBME = TwoWire(1);
//Adafruit_BME280 bme; 


float calibration_factor = -0.755; // калибровка датчика -1.547
float units;
float ounces;

// TinyGSM Client for Internet connection
TinyGsmClient client(modem);
//TinyGsmClientSecure client(modem);
//HttpClient          http(client, server, port);

#define uS_TO_S_FACTOR 1000000UL   /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3600        /* Time ESP32 will go to sleep (in seconds) 3600 seconds = 1 hour */

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

bool setPowerBoostKeepOn(int en){
  I2CPower.beginTransmission(IP5306_ADDR);
  I2CPower.write(IP5306_REG_SYS_CTL0);
  if (en) {
    I2CPower.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  } else {
    I2CPower.write(0x35); // 0x37 is default reg value
  }
  return I2CPower.endTransmission() == 0;
}

void setup() {
  // Set serial monitor debugging window baud rate to 115200
  SerialMon.begin(115200);

   // wakes up the chip HX711 after power down mode
   scale.power_up();
   delay(1000);

   scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
   scale.set_scale();
   // scale.tare();         // сбрасываем датчик на 0
   scale.set_scale(calibration_factor); // применяем калибровку
   
   Serial.print("Reading HX711: ");
   for(int i=0; i<10; i++) units =+ scale.get_units(), 10; // выполняем замеры 10 раз
   units / 10;                                             // делим значения на 10
   ounces = units * 0.035274;                              // переводим значения в граммы

   Serial.print(ounces);
   Serial.print(" grams");
   Serial.println();

  // Start I2C communication
  I2CPower.begin(I2C_SDA, I2C_SCL, 400000);
 // I2CBME.begin(I2C_SDA_2, I2C_SCL_2, 400000);

  // Keep power when running from battery
  bool isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // use modem.init() if you don't need the complete restart

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
  }
  
  // You might need to change the BME280 I2C address, in our case it's 0x76
//  if (!bme.begin(0x76, &I2CBME)) {
//    Serial.println("Could not find a valid BME280 sensor, check wiring!");
//    while (1);
//  }

  // Configure the wake up source as timer wake up  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop() {
  SerialMon.print("Connecting to APN: ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail APN");
  }
  else {
    SerialMon.println(" OK");
    
    SerialMon.print("Connecting to ");
    SerialMon.println(server);
    if (!client.connect(server, port)) {
      SerialMon.println(" fail Connecting to server");
    }
    else {
      SerialMon.println(" OK");

       // Making an HTTP POST request
      SerialMon.println("Performing HTTP POST request...");

      // Prepare your HTTP POST request data (Weight in grams)
      String httpRequestData = "api_key=" + apiKeyValue + "&value=" + String(ounces) + "&beehiveID=" + String(beehiveID);
          
      // You can comment the httpRequestData variable above
      // then, use the httpRequestData variable below (for testing purposes without the sensor)
      // String httpRequestData = "api_key=1234567890123&value=2004.75&beehiveID=1";

      client.print(String("POST ") + resource + " HTTP/1.1\r\n");
      client.print(String("Host: ") + server + "\r\n");
      client.println("Connection: keep-alive");
      client.println("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.106 Safari/537.36");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Accept: */*");
      client.println("Accept-Encoding: gzip, deflate");
      client.println("Accept-Language: ru-RU,ru;q=0.9,en;q=0.8,en-US;q=0.7,zh;q=0.6");
      client.println("Connection: close");
      client.print("Content-Length: ");
      client.println(httpRequestData.length());
      client.println();
      client.println(httpRequestData);
                       
      unsigned long timeout = millis();
      while (client.connected() && millis() - timeout < 10000L) {
        // Print available data (HTTP response from server)
        while (client.available()) {
          char c = client.read();
          SerialMon.print(c);
          timeout = millis();
        }
      }
      SerialMon.println();
    
      // Close client and disconnect
      client.stop();
      SerialMon.println(F("Server disconnected"));
      modem.gprsDisconnect();
      SerialMon.println(F("GPRS disconnected"));
    }
  }
  // puts the chip HX711 into power down mode
  scale.power_down();
  delay(1000);
  
  // Put ESP32 into deep sleep mode (with timer wake up)
  esp_deep_sleep_start();
}
