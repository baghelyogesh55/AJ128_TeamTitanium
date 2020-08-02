#include <NTPClient.h>
#include <WiFiUdp.h>

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ESP8266HTTPClient.h>
#include <HX711.h>
#include <WiFiClient.h>

//firebase credentials
const char FIREBASE_HOST [] ="nodemcu-1212e.firebaseio.com";
const char FIREBASE_AUTH [] ="3DIEuRkABn2lZuFDaUSg9wwk87yQPZ2LjBb7q902";
#define WIFI_SSID "AirtelWifi"
#define WIFI_PASSWORD "Ankush31@Pragati17"
const long utcOffsetInSeconds = 19800;
float weight;
HX711 scale;
//LDR Pin Connected at A0 Pin
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

float calibration_factor = 220; // this calibration factor is adjusted according to my load cell
float units;
float ounces;
float grams;
float average;
double sum=0 ;
int count=0; // The variable resistor value will be stored in sdata.

void setup()
{
    // Debug console
    Serial.begin(9600);

    scale.begin(D1,D2);
    Serial.println("HX711 calibration sketch");
    Serial.println("Remove all weight from scale");
    Serial.println("After readings begin, place known weight on scale");
    Serial.println("Press + or a to increase calibration factor");
    Serial.println("Press - or z to decrease calibration factor");

    scale.set_scale();
    scale.tare();  //Reset the scale to 0

    long zero_factor = scale.read_average(); //Get a baseline reading
    Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
    Serial.println(zero_factor);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".Cant Connect");
        delay(500);
    }
    Serial.println();
    Serial.print("connected: ");
    Serial.println(WiFi.localIP());

    timeClient.begin();
    Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
}

void loop()
{timeClient.update();
    scale.set_scale(calibration_factor);      //Adjusted to this calibration factor

    Serial.print("Reading: ");
  // ==========================================
  //          ADDED PART OF AVERAGE
  // ==========================================
  sum=0.0;
    for(int i=0;i<10;i++){
    units = scale.get_units(), 10;
    if (units < 0)
    {
        units = 0.00;
    }
    sum+=units ;
    delay(1000);
    }
    grams=sum/10 ;
// ==========================================
// ==========================================

  // ==========================================
  //          Serial Printing
  // ==========================================
  //  Serial.print(average);
    //Serial.print(ounces);
    Serial.print(" grams ");
    Serial.print(grams);
    Serial.print(" calibration_factor: ");
    Serial.print(calibration_factor);
    //Serial.println();
    Serial.print("  ");
//    Serial.print(timeClient.getHours());
//    Serial.print(":");
//    Serial.print(timeClient.getMinutes());
//    Serial.print(":");
//    Serial.println(timeClient.getSeconds());
    Serial.println(timeClient.getFormattedTime());

  // ==========================================
  // ==========================================

    String fireTemp = String(grams) +"@"+  String(timeClient.getFormattedTime()); // The string which will pushed to firebase

  // ==========================================
  //          Callibaration Adjust
  // ==========================================
    if(Serial.available())
    {
        char temp = Serial.read();
        if(temp == '+' || temp == 'a')
            calibration_factor += 2;
        else if(temp == '-' || temp == 'z')
            calibration_factor -= 2;
    }

    // ==========================================
  //          Data Uploading to Firebase
  // ==========================================
//    Firebase.pushFloat("Weight",grams);
Firebase.pushString("Weight",fireTemp);
    // handle error
    if (Firebase.failed())
    {
        Serial.print("setting /number failed:");
        Serial.println(Firebase.error());
        return;
    }
    delay(2000);
    
}
