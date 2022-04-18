#define BLYNK_TEMPLATE_ID "TMPL14NjLJvS"
#define BLYNK_DEVICE_NAME "Lights"
#define BLYNK_AUTH_TOKEN "iz1I54CtR6hbO8-qGRi1Vj5SpoOHQcfg"
#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
const int ldrPin = A0;
const int Sensora = 7;
const int sensorB = 8;
const int led = 12;
const int led2 = 4;
int threshold = 300;
unsigned int count = 0;
boolean sensorATriggered = false;
unsigned long sensorATime;
byte currStateA;
byte prevStateA = HIGH;
boolean sensorBTriggered = false;
unsigned long sensorBTime;
byte currStateB;
byte prevStateB = HIGH;
int vis = 0;
boolean blinkled1 = false;
boolean blinkled2 = false;
boolean useraccess = false;

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "TP-Link_1A60";
char pass[] = "50892549";

// Hardware Serial on Mega, Leonardo, Micro...
//#define EspSerial Serial1

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX

#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);
BlynkTimer timer;

void myTimerEvent()
{
    Blynk.virtualWrite(V5, millis() / 1000);
}

void setup()
{
    Serial.begin(9600);
    pinMode(Sensora, INPUT);
    pinMode(sensorB, INPUT);
    pinMode(led, OUTPUT);
    pinMode(led2, OUTPUT);

    // Set ESP8266 baud rate
    EspSerial.begin(ESP8266_BAUD);
    delay(10);

    Blynk.begin(auth, wifi, ssid, pass);
    timer.setInterval(1000L, myTimerEvent);
    Blynk.virtualWrite(V0, 1);
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 1);
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V3, count);
}

BLYNK_WRITE(V0) 
{
    if (param.asInt() == 1)
    {
        blinkled1 = true;
    }
    else
    {
        blinkled1 = false;
    }
}

BLYNK_WRITE(V1) 
{
    if (param.asInt() == 1)
    {
        blinkled2 = true;
    }
    else
    {
        blinkled2 = false;
    }
}

BLYNK_WRITE(V2) 
{
    if (param.asInt() == 1)
    {
        useraccess = true;
    }
    else
    {
        useraccess = false;
        Serial.println(String("No. of visitors : ") + count);
    }
}

BLYNK_CONNECTED()
{
    Blynk.syncVirtual(V0);
    Blynk.syncVirtual(V1); 
}

void loop()
{
    currStateA = digitalRead(Sensora);

    if (vis == 0)
    {
        Serial.print("No Visitors = 0");
        Serial.println(" ");
        vis++;
    }

    if (currStateA != prevStateA)
    {
        if (currStateA == LOW)
        {
            sensorATriggered = true;

            sensorATime = millis();
        }
    }
    currStateB = digitalRead(sensorB);
    if (currStateB != prevStateB)
    {
        if (currStateB == LOW)
        {
            sensorBTriggered = true;
            sensorBTime = millis();
        }
    }
    if (sensorBTriggered && sensorATriggered)

    {

        if (sensorATime > sensorBTime)

        {
            count++;
            Serial.println(String("No. of visitors : ") + count);
            Blynk.virtualWrite(V3, count);
            delay(500);
        }

        if (sensorBTime > sensorATime)

        {
            if (count > 0)
            {
                count--;
                Serial.println(String("No. of visitors : ") + count);
                Blynk.virtualWrite(V3, count);
                delay(500);
            }
        }

        sensorATriggered = false;

        sensorBTriggered = false;
    }

    if (useraccess == true)
    {
        if (blinkled1 == true)
        {
            digitalWrite(led, HIGH);  
            delay(50);    
        }
        else
        {
            digitalWrite(led, LOW);
            delay(200);
        }

        if (blinkled2 == true)
        {
            digitalWrite(led2, HIGH);
            delay(50);
        }
        else
        {
            digitalWrite(led2, LOW);
            delay(200);
        }
    }
    if (useraccess == false)
    {
        if (count <= 0)
        {
            digitalWrite(led, LOW);
            digitalWrite(led2, LOW);
            Blynk.virtualWrite(V0, 0);
            Blynk.virtualWrite(V1, 0);
            blinkled1 = false;
            blinkled2 = false;
        }
        else
        {
            int data = analogRead(ldrPin);
            if (data <= threshold)
            {
                digitalWrite(led, HIGH);
                delay(50);
                Blynk.virtualWrite(V0, 1);
                blinkled1 = true;
                if (count >= 3)
                {
                    digitalWrite(led2, HIGH);
                    delay(50);
                    Blynk.virtualWrite(V1, 1);
                    blinkled2 = true;
                }
                else
                {
                    digitalWrite(led2, LOW);
                    Blynk.virtualWrite(V1, 0);
                    blinkled2 = false;
                    delay(200);
                }
            }
            else
            {
                digitalWrite(led, LOW);
                digitalWrite(led2, LOW);
                Blynk.virtualWrite(V0, 0);
                Blynk.virtualWrite(V1, 0);
                blinkled1 = false;
                blinkled2 = false;
                delay(200);            }
        }
    }
    Blynk.run();
    timer.run(); // Initiates BlynkTimer
}
