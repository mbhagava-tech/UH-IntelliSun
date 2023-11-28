#include <WiFiNINA.h>
#include <Firebase_Arduino_WiFiNINA.h>
#include "DHT.h"

#include <HX711.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

/*
Pinout for Analog Sensors on Arduino:
A0
A1 - Voltage Sensor 1
A2 - Voltage Sensor 2
A3
A4 - Current Sensor 1
A5 - Current Sensor 2

Pinout for Digital Sensors on Arduino:
D2 - Temperature Sensor
D3 - HX711 CLK Output
D4 - HX711 DAT Output
*/

// Include Relevant Variables Here

// Variables needed to connect to WiFi:
char ssid[] = "JerseyDrive16006";    // your network SSID (name), replacing the "XXXXXXXXX"
char pass[] = "towEl-10-snarE";      // your network password, replacing the "XXXXXXXXX"
int keyIndex = 0;                    // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;         //connection status
WiFiServer server(70);               //server socket
WiFiClient client = server.available();

// Variables needed to connect to Firebase and Send Data:
#define DATABASE_URL "uh-intellisun-default-rtdb.firebaseio.com"
#define DATABASE_SECRET "sp3JojdNeEkpUW5slFflMXESRBwp4IGwsChXj24Y"

FirebaseData firebaseData;
String filepath;
String jsonStr;

// Variables for Temp/Humidity Sensor

#define DHTPIN  2       // Digital Pin 2
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// Variables for Load Cell Sensor

const int HX711_DOUT = 3; // Digital Pin 3
const int HX711_SCK  = 4; // Digital Pin 4

HX711 LoadCell;

// Calibration values (change these according to your setup)
const long known_weight = 87770;  // in grams
const long known_weight_reading = 12504998;  // replace with your actual reading

// Variables to Store Sensor Pin Locations
const int Voltage_Sensor_1 = A1;
const int Voltage_Sensor_2 = A2;
const int Current_Sensor_1 = A4;
const int Current_Sensor_2 = A5;

// Variables needed for Voltage Sensors
float Voltage_1      = 0.0;
float Voltage_2      = 0.0;

// Variables needed for Current Sensors
float Current_1      = 0.0;
float Current_2      = 0.0;

// Variable to Calculate the Power-Flow Into the Battery
float Power_In        = 0.0;

// Variable to Calculate the Power-Flow Out of the Battery
float Power_Out        = 0.0;

// Variable needed for Temperature Sensor
float Temperature;

int increment, last_increment, iter_count;
unsigned long old_time, new_time;
bool old_seat, curr_seat, dataSent;

void setup() {
  Serial.begin(9600);
  // Establish Connection to WiFi
  while (!Serial);
  
  enable_WiFi();
  connect_WiFi();
  server.begin();
  printWifiStatus();

  // Establish Connection to Firebase
  Firebase.begin(DATABASE_URL, DATABASE_SECRET, ssid, pass);
  Firebase.reconnectWiFi(true);

  // Set Up First Time Increment Value
  increment = 0;

  // Initialize Sensors
  dht.begin();
  LoadCell.begin(HX711_DOUT, HX711_SCK);
  calibrate();

  Serial.println();
  old_seat = gatherLoadCellData();
  last_increment = -1;
  iter_count = 0;
  old_time = millis();
}

void loop() {
  // Check if Load Cells Detect a Weight Greater than 40 lbs.
  curr_seat = gatherLoadCellData();

  if(curr_seat != old_seat)
  {
    // If so, update seat availability to Firebase immediately, and start the loop again
    if(Firebase.setBool(firebaseData, filepath + "/Seat_Availability", curr_seat))
    {
      Serial.println(firebaseData.dataPath() + " = " + curr_seat);
    }
    else
    {
      Serial.println("ERROR: " + firebaseData.errorReason());
    }

    old_seat = curr_seat;
    return;
  }
  else
  {
    // Otherwise, update old_seat
    old_seat = curr_seat;
  }

  new_time = millis();
  increment = ((unsigned long)(new_time - old_time)) / 60000UL;

  Serial.print("old_time: "); Serial.print(old_time); Serial.print(" | new_time: "); Serial.print(new_time); Serial.print(" | ");
  Serial.print("Increment: "); Serial.print(increment); Serial.print(" | Last_Increment: "); Serial.print(last_increment); Serial.print(" | Iteration: "); Serial.println(iter_count);

  // If the load cells haven't changed, wait until 5 minutes have passed
  if(increment > last_increment)
  {
    // Once 5 minutes have passed:
    // Gather Sensor Data
    Voltage_1 = gatherVoltageSensorData(1);
    Voltage_2 = gatherVoltageSensorData(2);
    Current_1 = gatherCurrentSensorData(1);
    Current_2 = gatherCurrentSensorData(2);
    Temperature = dht.readTemperature(true);

    // Send Sensor Data to Firebase
    dataSent = sendDataToFirebase(Voltage_1, Voltage_2, Current_1, Current_2, Temperature);
    if(dataSent)
    {
      Serial.println("All sensor data sent successfully!");
    }
    else
    {
      Serial.println("ERROR: Sensor data failed to send!");
    }
    last_increment = increment;
    iter_count++;
  }
}

// Functions Called in loop()

// WiFi Functions
void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void printWEB() {

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
           
            //create the buttons
            client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED off<br><br>");
            
            int randomReading = analogRead(A1);
            client.print("Random reading from analog pin: ");
            client.print(randomReading);
           
            
            

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

// Firebase Function
bool sendDataToFirebase(float Voltage_1, float Voltage_2, float Current_1, float Current_2, float Temperature)
{
  int count = 0;

  // Send data to Firebase with Specific Paths
  if(Firebase.setFloat(firebaseData, filepath + "/Voltage_Sensors/1", Voltage_1))
  {
    Serial.println(firebaseData.dataPath() + " = " + Voltage_1);
    count++;
  }
  if(Firebase.setFloat(firebaseData, filepath + "/Voltage_Sensors/2", Voltage_2))
  {
    Serial.println(firebaseData.dataPath() + " = " + Voltage_2);
    count++;
  }
  if(Firebase.setFloat(firebaseData, filepath + "/Current_Sensors/1", Current_1))
  {
    Serial.println(firebaseData.dataPath() + " = " + Current_1);
    count++;
  }
  if(Firebase.setFloat(firebaseData, filepath + "/Current_Sensors/2", Current_2))
  {
    Serial.println(firebaseData.dataPath() + " = " + Current_2);
    count++;
  }
  if(Firebase.setFloat(firebaseData, filepath + "/Temperature_Sensor", Temperature))
  {
    Serial.println(firebaseData.dataPath() + " = " + Temperature);
    count++;
  }

  if(count == 5)
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Load Cell Functions
void calibrate()
{
  Serial.println("Calibrating...");
  LoadCell.set_scale();
  LoadCell.tare();
  long zero_factor = LoadCell.read_average();  // Get the average reading with no load
  LoadCell.set_scale(known_weight_reading / known_weight);
  Serial.println("Calibration complete!");
}

bool gatherLoadCellData()
{
  bool seatAvailable = false;
  
  float weight = LoadCell.get_units(10);   // Average over 10 readings for stability
  weight = (weight / (-100)) * 2.20462;    // Convert reading to kg, then to lbs
  
  // The load cells shouldn't be returning a negative number; therefore, any reading
  // less than 0 should be set to 0.
  if (weight < 0)
  {
    weight = 0;
  }

  if (weight <= 40)
  {
    seatAvailable = true;
  }

  return seatAvailable;
}

// Voltage & Current Sensor Functions
float gatherVoltageSensorData(int sensorNumber)
{
  int Volt_Sensor_Pin, Read_Voltage;
  float Voltage, volt_ratio = 0.0048875855;

  if (sensorNumber == 1)
  {
    Volt_Sensor_Pin = Voltage_Sensor_1;
  }
  else if (sensorNumber == 2)
  {
    Volt_Sensor_Pin = Voltage_Sensor_2;
  }

  Read_Voltage = analogRead(Volt_Sensor_Pin);

  Voltage = Read_Voltage * volt_ratio;

  if(abs(Voltage) > 12.0)
  {
    Voltage = -30.0;
  }

  return Voltage;
}

float gatherCurrentSensorData(int sensorNumber)
{
  int Amp_Sensor_Pin, Read_Current;
  float Current, amp_ratio = (5.0/1023.0);

  if (sensorNumber == 1)
  {
    Amp_Sensor_Pin = Current_Sensor_1;
  }
  else if (sensorNumber == 2)
  {
    Amp_Sensor_Pin = Current_Sensor_2;
  }

  Read_Current = analogRead(Amp_Sensor_Pin);

  Current = ((Read_Current * amp_ratio) - 2.5) / 0.066;

  if (abs(Current) > 24.0)
  {
    Current = -30.0;
  }

  return Current;
}




