// Author : Amit kumar
#include <AWS_IOT.h>
#include <WiFi.h>
#include "Temp.h"
/***************************************************/

uint8_t temprature_sens_read();
/***********************************************************/
#define MBEDTLS_ERR_NET_UNKNOWN_HOST                      -0x0052
AWS_IOT Temp_test;   // AWS_IOT instance

char WIFI_SSID[]="er"; //wifi ssid
char WIFI_PASSWORD[]="AmitKumar";//wifi password
char HOST_ADDRESS[]="a22eq4g0x3b0m1-ats.iot.ap-south-1.amazonaws.com";//aws host address http
char CLIENT_ID[]= "Temp_test";//mqtt client
char TOPIC_NAME[]= "$aws/things/Temp_testing/shadow/update";//mqtt topic name


int status = WL_IDLE_STATUS;
int tick=0,msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];

/************************************************************/
void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
    strncpy(rcvdPayload,payLoad,payloadLen);
    rcvdPayload[payloadLen] = 0;
    msgReceived = 1;
}
/**************************************************************/
void setup() {
    
    Serial.begin(9600);
    delay(2000);

    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(WIFI_SSID);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        // wait 5 seconds for connection:
        delay(5000);
    }

    Serial.println("Connected to wifi");

    if(Temp_test.connect(HOST_ADDRESS,CLIENT_ID)== 0) // Connect to AWS using Host Address and Cliend ID
    {
        Serial.println("Connected to AWS");
        delay(1000);
           if(0==Temp_test.subscribe(TOPIC_NAME,mySubCallBackHandler))
        {
            Serial.println("Subscribe Successfull");
        }
        else
        {
            Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
            while(1);
        }
    }
    else
    {
        Serial.println("AWS connection failed, Check the HOST Address");
        while(1);
    }

    delay(2000);
}


void loop() {

//
 if(msgReceived == 1)
    {
        msgReceived = 0;
        Serial.print("Received Message:");
        Serial.println(rcvdPayload);
    }
  float temp = ((temprature_sens_read() - 32) / 1.8);
  // Check if any reads failed and exit early (to try again).
    if ( isnan(temp)) {
        Serial.println("Failed to read from Temp sensor!");
    }
    else
    {
        sprintf(payload,"  Temperature:%f'C",temp); // Create the payload for publishing
        
        if(Temp_test.publish(TOPIC_NAME,payload) == 0)   // Publish the message(Temp)
        {        
            Serial.print("Publish Message:");   
            Serial.println(payload);
        }
        else
        {
            Serial.println("Publish failed");
        }
        // publish the temp and humidity every 60 seconds.
        vTaskDelay(60000 / portTICK_RATE_MS);     
    } 
}