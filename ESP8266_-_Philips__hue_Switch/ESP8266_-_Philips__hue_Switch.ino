/* Project name: ESP8266 - Philips® hue Switch
   Project URI:  https://www.studiopieters.nl/esp8266-philips-hue-switch/
   Description: Switch to use with the Philips® Hue bridge
   Version: 2.3.3
   License: MIT Copyright 2019 StudioPieters®
*/

#include <ESP8266WiFi.h>
#include <SPI.h>

IPAddress ip(192, 168, 178, 1);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);

int light = 1;                                   // Number of the Hue light to be switched

const char hueHubIP[] = "192.168.178.100";       // Hue hub IP
const char hueUsername[] = "7CAFB8485183FFC588A6BAB16A2FEE4LSUOJ4CME";  // hue bridge username

const int hueHubPort = 80;


const char ssid[] = "SSID";                      // Network SSID (name)
const char pass[] = "password";                  // Network password



boolean onOffState = false;                       // To store actual on/off state of lights as reported by Hue bridge

WiFiClient client;


void getHue()
{
  if (client.connect(hueHubIP, hueHubPort))
  {
    client.print("GET /api/");
    client.print(hueUsername);
    client.print("/lights/");
    client.print(light);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(hueHubIP);
    client.println("Content-type: application/json");
    client.println("keep-alive");
    client.println();
    while (client.connected())
    {
      if (client.available())
      {
        client.findUntil("\"on\":", "\0");
        onOffState = (client.readStringUntil(',') == "true");
        break;
      }
    }
    client.stop();
  }
}




void setHue(String command)
{
  if (client.connect(hueHubIP, hueHubPort))
  {
    client.print("PUT /api/");
    client.print(hueUsername);
    client.print("/lights/");
    client.print(light);
    client.println("/state HTTP/1.1");
    client.println("keep-alive");
    client.print("Host: ");
    client.println(hueHubIP);
    client.print("Content-Length: ");
    client.println(command.length());
    client.println("Content-Type: text/plain;charset=UTF-8");
    client.println();                             // Blank line before body
    client.println(command);
    client.stop();
  }
}




void setup()
{


  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1);

  }

}



void loop()
{

  getHue();                                       // Lights 1 status


  if (onOffState == true)                         // If lights are on, send "Off" command

  {
    String command =  "{\"on\": false}";
    setHue(command);
  }

  else                                            // If lights are off, send "On" command

  {
    String command =  "{\"on\": true}";
    setHue(command);


  }




  ESP.deepSleep(0);
}
