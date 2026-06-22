#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "SmartHome_Groupe1";
const char* password = "smarthome2026";

#define RXD2 16
#define TXD2 17

WebServer server(80);

// Variables qui stockent les dernières données reçues
String temp = "--";
String hum  = "--";
String dist = "--";
String etat = "--";

String pageHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='2'>"; // rafraichit toutes les 2s
  html += "<title>Smart Home Dashboard</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#1a1a2e;color:#fff;text-align:center;padding:20px;}";
  html += "h1{color:#1D9E75;}";
  html += ".card{background:#16213e;border-radius:10px;padding:20px;margin:12px auto;max-width:400px;}";
  html += ".val{font-size:32px;font-weight:bold;color:#1D9E75;}";
  html += ".label{font-size:14px;color:#aaa;}";
  html += ".alerte{color:#e24b4a;}";
  html += "</style></head><body>";
  html += "<h1>🏠 Smart Home</h1>";

  html += "<div class='card'><div class='label'>Temperature</div>";
  html += "<div class='val'>" + temp + " &deg;C</div></div>";

  html += "<div class='card'><div class='label'>Humidite</div>";
  html += "<div class='val'>" + hum + " %</div></div>";

  html += "<div class='card'><div class='label'>Distance</div>";
  html += "<div class='val'>" + dist + " cm</div></div>";

  html += "<div class='card'><div class='label'>Etat du systeme</div>";
  if (etat == "OK") {
    html += "<div class='val'>" + etat + "</div></div>";
  } else {
    html += "<div class='val alerte'>" + etat + "</div></div>";
  }

  html += "<p style='color:#666;font-size:12px;'>Dashboard local ESP32 - sans Internet</p>";
  html += "</body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", pageHTML());
}

void parserDonnees(String data) {
  // Format attendu : TEMP:26|HUM:71|DIST:120|ETAT:OK
  int posTemp = data.indexOf("TEMP:");
  int posHum  = data.indexOf("HUM:");
  int posDist = data.indexOf("DIST:");
  int posEtat = data.indexOf("ETAT:");

  if (posTemp >= 0 && posHum >= 0) {
    temp = data.substring(posTemp + 5, data.indexOf("|", posTemp));
    hum  = data.substring(posHum + 4, data.indexOf("|", posHum));
    dist = data.substring(posDist + 5, data.indexOf("|", posDist));
    etat = data.substring(posEtat + 5);
    etat.trim();
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Dashboard sur : ");
  Serial.println(IP);

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  // Réception des données Arduino
  if (Serial2.available()) {
    String message = Serial2.readStringUntil('\n');
    message.trim();
    if (message.length() > 0) {
      parserDonnees(message);
      Serial.println("Recu : " + message);
    }
  }

  server.handleClient();
}