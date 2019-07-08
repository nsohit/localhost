/*
  MySQL Connector/Arduino Example : connect by wifi

  This example demonstrates how to connect to a MySQL server from an
  Arduino using an Arduino-compatible Wifi shield. Note that "compatible"
  means it must conform to the Ethernet class library or be a derivative
  thereof. See the documentation located in the /docs folder for more
  details.

  INSTRUCTIONS FOR USE

  1) Change the address of the server to the IP address of the MySQL server
  2) Change the user and password to a valid MySQL user and password
  3) Change the SSID and pass to match your WiFi network
  4) Connect a USB cable to your Arduino
  5) Select the correct board and port
  6) Compile and upload the sketch to your Arduino
  7) Once uploaded, open Serial Monitor (use 115200 speed) and observe

  If you do not see messages indicating you have a connection, refer to the
  manual for troubleshooting tips. The most common issues are the server is
  not accessible from the network or the user name and password is incorrect.

  Created by: Dr. Charles A. Bell
*/
#include <ESP8266WiFi.h>           // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

//aceletometer
#include<Wire.h>
const int MPU_addr=0x68;  // I2C address of the MPU-6050
byte AcX;

// gps
#include <TinyGPS.h>
#include <SoftwareSerial.h>
SoftwareSerial GPS(D3,D4); // configure software serial port 

// Create an instance of the TinyGPS object
TinyGPS shield;


IPAddress server_addr(103,103,175,152);  // IP of the MySQL *server* here
char user[] = "keripikp_sohit";              // MySQL user login username
char password[] = "123nursohit123";        // MySQL user login password

// Sample query
char INSERT1_SQL[] = "INSERT INTO keripikp_sqm.acelerometer(goyangan) VALUES (%d)";
char query1[100];
char INSERT2_SQL[] = "INSERT INTO keripikp_sqm.tracking (longitude,latitude,kecepatan) VALUES (%f,%f,%d)";
char query2[100];
// WiFi card example
char ssid[] = "sohit";         // your SSID
char pass[] = "bukasandi";     // your SSID Password

WiFiClient client;                 // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;


  //Serial.println(kmh, DEC);
 // Serial.println(" km/h      ");
/*  
  mph = kmh * 1.6;
  lcd.print(mph, DEC);
  lcd.print(" MPH   ");
*/

void setup()
{
 Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  Serial.begin(115200);
  GPS.begin(9600);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only

  // Begin WiFi section
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // print out info about the connection:
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());


  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);
}

void loop()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  
  AcX=Wire.read()<<8|Wire.read(); 
  //AcX / 74,6;

  
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (GPS.available())
    {
      char c = GPS.read();
      // Serial.write(c); // hilangkan koment jika mau melihat data yang dikirim dari modul GPS
      if (shield.encode(c)) // Parsing semua data
        newData = true;
          
    }
  }
   int year;
   byte month, day, hour, minute, second, hundredths, kmh, mph;
   shield.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
   kmh= shield.f_speed_kmph();
  
   

    float flat, flon;
    //unsigned long age;
   // shield.f_get_position(&flat, &flon, &age);
    flat == (TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    flon == (TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    shield.satellites() == (TinyGPS::GPS_INVALID_SATELLITES ? 0 :shield.satellites());
    shield.hdop() == (TinyGPS::GPS_INVALID_HDOP ? 0 : shield.hdop());


    
   delay(10000); //10 sec

   sprintf(query1, INSERT1_SQL, AcX);
   sprintf(query2, INSERT2_SQL,flon,flat, kmh );

 

   Serial.println("Recording data.");
   Serial.println(query1);
   Serial.println(query2);
  
  cursor->execute(query1);
  cursor->execute(query2);


}
