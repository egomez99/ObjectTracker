#include <PubNub.h>

#include <SPI.h>

//import all the necessary files for GPRS connectivity
#include <LGPRS.h>
//#include <LGPRSClient.h>

#include <LGPS.h>
#include "GPSWaypoint.h"
#include "GPS_functions.h"

#define ledPin 13

GPSWaypoint* gpsPosition;

/* Change these to match your own config TELCEL
#define GPRS_APN     "internet.itelcel.com"
#define GPRS_USER    "webgprs"
#define GPRS_PASS    "webgprs2003"
*/

// Change these to match your own config IUSACELL
#define GPRS_APN     "web.iusacellgsm.mx"
#define GPRS_USER    "Iusacellgsm"
#define GPRS_PASS    "iusacellgsm"
//

/* Change these to match your own config Movistar
#define GPRS_APN     "internet.movistar.mx"
#define GPRS_USER    "movistar"
#define GPRS_PASS    "movistar"
*/

//define the required keys for using PubNub (Bike-Map)
//char pubkey[] = "pub-c-38fc720f-66b9-47a5-972c-d20cba7cf6b1";
//char subkey[] = "sub-c-716f42e4-69d9-11e5-87d4-0619f8945a4f";
//char channel[] = "bike-map";

//define the required keys for using PubNub (bus-Map)
//char pubkey[] = "pub-c-1310ca2b-9e6f-468a-8cc3-9dc77e641711";
//char subkey[] = "sub-c-6baffaa4-785e-11e5-8495-02ee2ddab7fe";
//char channel[] = "bus-map";
//https://www.pubnub.com/console/?sub=sub-c-6baffaa4-785e-11e5-8495-02ee2ddab7fe&pub=pub-c-1310ca2b-9e6f-468a-8cc3-9dc77e641711&sec=sec-c-MTEwMWI1ZDEtOGM2MS00ZWYwLWJlMTUtNWRlODNkMmQ3ZjBh&origin=pubsub.pubnub.com&ssl=false

//define the required keys for using PubNub (test-skype)
char pubkey[] = "pub-c-60f5eb70-ef1c-4d78-be5b-d5741bbf77e3";
char subkey[] = "sub-c-97f94090-79a1-11e5-8d3c-0619f8945a4f";
char channel[] = "test-skype";

void setup()
{
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);
    Serial.println("Serial setup");

    //Connect to the GRPS network in order to send/receive PubNub messages
    Serial.println("Attach to GPRS network with correct APN settings from your mobile network provider");
    //example here is with mobile provider EE in the UK
    //attachGPRS(const char *apn, const char *username, const char *password);
  
     //while (!LGPRS.attachGPRS("mobile.o2.co.uk", "o2web", "password"))
    while (!LGPRS.attachGPRS(GPRS_APN, GPRS_USER, GPRS_PASS)) 
    {
    Serial.println(" Attaching LGPRS wait please ");
    delay(1000);
    }

    PubNub.begin(pubkey, subkey);
    Serial.println("PubNub setup");

    Serial.println("LGPRS PowerOn");
    LGPS.powerOn();
}

 
void flash(bool success)
{
     
    /* Flash LED three times. */
    for (int i = 0; i < 3; i++) {
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
    }
}


void loop()
{
    Serial.println("Getting GPS Data");
    //Aquire GPS position
    char GPS_formatted[] = "GPS fixed";
    gpsPosition = new GPSWaypoint();
    gpsSentenceInfoStruct gpsDataStruct;
    getGPSData(gpsDataStruct, GPS_formatted, gpsPosition);
    Serial.println(" GPS Data aquired");


    char* buffer_latitude = new char[30];
    sprintf(buffer_latitude, "%2.6f", gpsPosition->latitude);

    char* buffer_longitude = new char[30];
    sprintf(buffer_longitude, "%2.6f", gpsPosition->longitude);

    String upload_GPS = "[{\"latlng\":[" + String(buffer_latitude) + "," + String(buffer_longitude)+ "], \"Smart-Cities\":\"LinkitOne on Bus\"}]";
    
    const char* upload_char = upload_GPS.c_str();
     
    //Once Position is Aquired, upload it to PubNub
    LGPRSClient *client;

    
    Serial.println("publishing a message");
    client = PubNub.publish(channel, upload_char, 60);
    if (!client) {
        Serial.println("publishing error");
        delay(1000);
        return;
    }
    while (client->connected()) {
        while (client->connected() && !client->available()); // wait
        char c = client->read();
        Serial.print(c);
    }
    client->stop();
    Serial.println();
    flash(true);


    delay(5000);
}
