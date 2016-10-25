////////////////////////////////////////////////////////////////////////////////////
//
//                               PI and more
//                      ESP8266 Advanced web server
//
// https://piandmore.wordpress.com/2016/10/05/advanced-webserver-in-arduino-ide/
//
////////////////////////////////////////////////////////////////////////////////////
//
// The ESP8266WiFi library is needed
//
#include <ESP8266WiFi.h>

//
// If a requested paramter is not found, this is what is returned
//
#define PARAMNOTFOUND "XxXxXxX"

//
// Please fill in your SSID and password
//
const char* ssid = "YourSSID";
const char* password = "YourPassword";

//
// Initialize the webserver
//
WiFiServer server(80);

void setup() {
  //
  // Initialize serial output
  //
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //
  // Connect to the wifi and wait for a connection
  //
  WiFi.begin(ssid,password);
  while (WiFi.status()!=WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  //
  // Start the webserver
  //
  server.begin();
  delay(500);
  //
  // Show the IP of the server in serial monitor
  //
  Serial.println("Web server running. The IP is:");
  Serial.println(WiFi.localIP());
}

//
// Given the full request string, return just the URI
//
String uriRequested (String reqstr) {
  byte p = reqstr.indexOf("T /")+2;
  return reqstr.substring(p,reqstr.indexOf(" ",p));
}

//
// Given the full request string, return the requested page
//
String pageRequested (String reqstr) {
  String t = uriRequested(reqstr);
  if (t.indexOf("?")>=0) {
    t = t.substring(0,t.indexOf("?"));
  }
  return t;
}

//
// Given the full request string, return the value of the parameter requested
// or PARAMNOTFOUND
//
String giveUrlParam (String reqstr, String search) {
  String t = uriRequested(reqstr);
  if (t.indexOf("?")>=0) {
    t = "&"+t.substring(t.indexOf("?")+1);
    int p = t.indexOf("&"+search+"=");
    if (p==-1) {
      return PARAMNOTFOUND;
    } else {
      p = t.indexOf("=",p)+1;
      t = t.substring(p,t.indexOf("&",p));
      return t;
    }
  } else {
    return PARAMNOTFOUND;
  }
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    //
    // A web client is asking for a page
    //
    Serial.println("Page requested");
    boolean lastLineBlank = true;
    String requestString = String(100);
    while (client.connected()) {
      //
      // We need to read the request of the client
      //
      if (client.available()) {
        char c = client.read();
        if (requestString.length()<100) {
          requestString = requestString+c;
        }
        if (c=='\n' && lastLineBlank) {
          //
          // We have read the request of the client. Now send the correct page
          //
          Serial.println(requestString);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");

          //
          // pr will contain the name of the page
          //
          String pr = pageRequested(requestString);
          if (pr=="/index.php" || pr=="/") {
            client.println("<html>");
            client.println("<head></head><body>");
            client.println("<h1>You requested the main page</h1>");
            client.println("</body></html>");
          } else if (pr=="/other.php") {
            client.println("<html>");
            client.println("<head></head><body>");
            client.println("<h1>You requested the other page</h1>");
            pr = giveUrlParam(requestString,"id");
            //
            // In this page we demo that you can see if someone has put
            // the id param in the requested url and if so, what the value is
            //
            if (pr==PARAMNOTFOUND) {
              client.println("You did not want to send your id");
            } else {
              client.println("You gave your id which is <b>"+pr+"<br>");
            }
            client.println("</body></html>");
          } else {
            client.println("<html>");
            client.println("<head></head><body>");
            client.println("<h1>You requested an unknown page</h1>");
            client.println("</body></html>");
          }
          break;
        }
        if (c=='\n') {
          lastLineBlank = true;
        } else if (c!='\r') {
          lastLineBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Page send.");
  }
}
