#include <ESP8266WiFi.h>

#define PARAMNOTFOUND "XxXxXxX"

const char* ssid = "YourSSID";
const char* password = "YourPassword";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  while (WiFi.status()!=WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.begin();
  delay(500);
  Serial.println("Web server running. The IP is:");
  Serial.println(WiFi.localIP());
}

String uriRequested (String reqstr) {
  byte p = reqstr.indexOf("T /")+2;
  return reqstr.substring(p,reqstr.indexOf(" ",p));
}

String pageRequested (String reqstr) {
  String t = uriRequested(reqstr);
  if (t.indexOf("?")>=0) {
    t = t.substring(0,t.indexOf("?"));
  }
  return t;
}

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
    Serial.println("Page requested");
    boolean lastLineBlank = true;
    String requestString = String(100);
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (requestString.length()<100) {
          requestString = requestString+c;
        }
        if (c=='\n' && lastLineBlank) {
          Serial.println(requestString);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");

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

