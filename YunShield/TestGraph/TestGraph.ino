// include all Libraries needed:
#include <Process.h>  //Process lib use to call Linux Commands in Yun Shield 
#include <Console.h>  //Console lib, used to show debug info in Arduino IDE 
#include "passwords.h"       // contains my passwords, see below 

/*
   NOTE: passwords.h is not included with this repo because it contains my passwords.
  You need to create it for your own version of this application.  To do so, make
  a new tab in Arduino, call it passwords.h, and include the following variables and constants:

  #define APIKEY        "foo"                  // replace your pachube api key here
  #define FEEDID         “0000”                    // replace your feed ID
  #define USERAGENT     "my-project"           // user agent is the project name
  */


// set up net client info:
const unsigned long postingInterval = 60000;   //delay between updates to xively.com
unsigned long lastRequest = 0;       // when you last made a request
String dataString = "";

void setup() {
  // start console:
  Bridge.begin();
  Console.begin();

  while (!Console);    // wait for Network Serial to open
  Console.println("Xively client");
  // Do a first update immediately
  updateData();
  sendData();
  lastRequest = millis();
}

void loop() {
  // get a timestamp so you can calculate reading and sending intervals:
  long now = millis();

  // if the sending interval has passed since your
  // last connection, then connect again and send data:
  if (now - lastRequest >= postingInterval) {
    updateData();
    sendData();
    lastRequest = now;
  }
}

void updateData() {
  // convert the readings to a String to send it:
  dataString = "Temperature,";
  dataString += random(10) + 20;
  // add pressure:
  dataString += "\nPressure,";
  dataString += random(5) + 100;
}

// this method makes a HTTP connection to the server:
void sendData() {
  // form the string for the API header parameter:
  String apiString = 'X-ApiKey: ';
  apiString += APIKEY;

  // form the string for the URL parameter:
  String url = 'https://api.xively.com/v2/feeds/';
  url += FEEDID;
  url += '.csv';

  // Send the HTTP PUT request, form the linux command and use Process Class to send this command to Yun Shield

  // Is better to declare the Process here, so when the
  // sendData function finishes the resources are immediately
  // released. Declaring it global works too, BTW.
  Process xively;
  Console.print("\n\nSending data... ");
  xively.begin("curl");
  xively.addParameter("-k");
  xively.addParameter("--request");
  xively.addParameter("PUT");
  xively.addParameter("--data");
  xively.addParameter(dataString);
  xively.addParameter("--header");
  xively.addParameter(apiString);
  xively.addParameter(url);
  xively.run();
  Console.println("done!");

  // If there's incoming data from the net connection,
  // send it out the Console:
  while (xively.available() > 0) {
    char c = xively.read();
    Console.write(c);
  }

}

