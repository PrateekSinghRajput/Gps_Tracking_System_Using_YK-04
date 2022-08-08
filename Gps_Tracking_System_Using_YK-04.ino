//Prateek
//www.prateeks.in

#include <SoftwareSerial.h>
#include <SimpleTimer.h>
#include <stdlib.h>
#include <TinyGPS++.h>
static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 9600;
int  m = 9740;
int y = 71;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial SIM800(7, 8);

int theft_Sensor = 4;
String textForSMS;

// Starts in Automatic Mode
int Yk_D0 = 12;
int Yk_D0_Flag = 0;
// Stops the automatic Mode
int Yk_D1 = 11;
// Anti-theft Mode
int Yk_D2 = 10;
int Yk_D2_Flag = 0;
// Normal mode
int Yk_D3 = 9;

double longitude;
double latitude;
char buff[10];

String mylong = "";
String mylati = "";

SimpleTimer timer;

int secs = 0;
int minutes = 0;
int mflag = 0;
void setup()
{
  SIM800.begin(19200);
  Serial.begin(9600);
  ss.begin(GPSBaud);
  delay(5000);
  Serial.println(" logging time completed!");
  randomSeed(analogRead(0));
  pinMode(Yk_D0, INPUT);
  digitalWrite(Yk_D0, LOW);
  pinMode(Yk_D1, INPUT);
  pinMode(Yk_D2, INPUT);
  pinMode(Yk_D3, INPUT);

  pinMode(theft_Sensor, INPUT_PULLUP);


  timer.setInterval(1000L, Send_GPS_Location);
}



void sendSMS(String message)
{
  SIM800.print("AT+CMGF=1\r");
  delay(100);
  SIM800.println("AT + CMGS = \"+919975617490\"");
  delay(100);
  SIM800.println(message);
  delay(100);
  SIM800.println((char)26);
  delay(100);
  SIM800.println();
  delay(5000);

}

void loop()
{

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }

  if (digitalRead(Yk_D0) == HIGH) // activates the automatic mode
  {
    Yk_D0_Flag = 1;

  }
  if (digitalRead(Yk_D1) == HIGH) // stops the automatic mode
  {
    Yk_D0_Flag = 0;
    secs = 0;
    minutes = 0;
  }
  if (Yk_D0_Flag == 1)
  {
    timer.run(); // activate the timer
    displayInfo();
    if (mflag == 1)
    {
      displayInfo();
      latitude = gps.location.lat(), 6 ;
      longitude = gps.location.lng(), 6 ;
      // for latitude
      mylati = dtostrf(latitude, 3, 6, buff);
      mylong = dtostrf(longitude, 3, 6, buff);

      //textForSMS = textForSMS + "Latitude:" + "," + mylati + "," + "Longitude:  "+ mylong + ",";
      textForSMS = textForSMS + "http://www.google.com/maps/place/" + mylati + "," + mylong ;
      sendSMS(textForSMS);
      textForSMS = "";
      Serial.println(textForSMS);
      Serial.println("message sent.");
      delay(3000);
      mflag = 0;
    }
  }

  // for the anti-theft

  if (digitalRead(Yk_D2) == HIGH)
  {
    Yk_D2_Flag = 1;
  }

  if (digitalRead(Yk_D3) == HIGH )
  {
    Yk_D2_Flag = 0;
  }

  if ( (Yk_D2_Flag == 1) && ( digitalRead(theft_Sensor) == LOW) )
  {

    timer.run(); // activate the timer
    displayInfo();
    if (mflag == 1)
    {
      displayInfo();
      latitude = gps.location.lat(), 6 ;
      longitude = gps.location.lng(), 6 ;
      // for latitude
      mylati = dtostrf(latitude, 3, 6, buff);
      mylong = dtostrf(longitude, 3, 6, buff);

      //textForSMS = textForSMS + "Latitude:" + "," + mylati + "," + "Longitude:  "+ mylong + ",";
      textForSMS = textForSMS + "http://www.google.com/maps/place/" + mylati + "," + mylong ;
      sendSMS(textForSMS);
      textForSMS = "";
      Serial.println(textForSMS);
      Serial.println("message sent.");
      delay(3000);
      mflag = 0;
    }

  }

}


void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    Serial.print(" ");
    Serial.print(F("Speed:"));
    Serial.print(gps.speed.kmph());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void Send_GPS_Location()
{

  secs = secs + 1;
  Serial.println("Secs:");
  Serial.println(secs);
  if ( secs >= 59 )
  {
    minutes = minutes + 1;
    secs = 0;
    Serial.println("Minutes:");
    Serial.println(minutes);
    if (minutes >= 2)
    {
      minutes = 0;
      secs = 0;
      mflag = 1;
    }

  }

}
