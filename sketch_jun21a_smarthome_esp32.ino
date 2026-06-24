#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "SmartHome_Groupe1";
const char* password = "smarthome2026";

#define RXD2 16
#define TXD2 17

WebServer server(80);

String temp = "--";
String hum  = "--";
String dist = "--";
String etat = "--";

String pageHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='3'>";
  html += "<title>Smart Home Dashboard</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#1a1a2e;color:#fff;text-align:center;padding:20px;}";
  html += "h1{color:#1D9E75;}";
  html += ".card{background:#16213e;border-radius:10px;padding:18px;margin:12px auto;max-width:400px;}";
  html += ".val{font-size:28px;font-weight:bold;color:#1D9E75;}";
  html += ".label{font-size:14px;color:#aaa;}";
  html += ".alerte{color:#e24b4a;}";
  html += "button{background:#1D9E75;color:#fff;border:none;padding:14px 20px;margin:6px;";
  html += "border-radius:8px;font-size:16px;width:160px;cursor:pointer;}";
  html += "button.off{background:#e24b4a;}";
  html += "button.auto{background:#2980b9;}";
  html += "</style></head><body>";
  html += "<h1>🏠 Smart Home</h1>";

  html += "<div class='card'><div class='label'>Etat du systeme</div>";
  if (etat == "OK") {
    html += "<div class='val'>" + etat + "</div></div>";
  } else {
    html += "<div class='val alerte'>" + etat + "</div></div>";
  }

  html += "<div class='card'><div class='label'>Distance</div>";
  html += "<div class='val'>" + dist + " cm</div></div>";

  html += "<div class='card'><div class='label'>Commande Eclairage</div>";
  html += "<a href='/lum_on'><button>Allumer</button></a>";
  html += "<a href='/lum_off'><button class='off'>Eteindre</button></a><br>";
  html += "<a href='/lum_auto'><button class='auto'>Mode Auto</button></a>";
  html += "</div>";

  html += "<div class='card'><div class='label'>Commande Alarme</div>";
  html += "<a href='/alarme_off'><button class='off'>Couper alarme</button></a>";
  html += "<a href='/alarme_on'><button>Reactiver</button></a>";
  html += "</div>";

  html += "<p style='color:#666;font-size:12px;'>Dashboard local ESP32 - sans Internet</p>";
  html += "</body></html>";
  return html;
}

void handleRoot()      { server.send(200, "text/html", pageHTML()); }

void handleLumOn()     { Serial2.println("LUM_ON");     server.sendHeader("Location","/"); server.send(303); }
void handleLumOff()    { Serial2.println("LUM_OFF");    server.sendHeader("Location","/"); server.send(303); }
void handleLumAuto()   { Serial2.println("LUM_AUTO");   server.sendHeader("Location","/"); server.send(303); }
void handleAlarmeOff() { Serial2.println("ALARME_OFF"); server.sendHeader("Location","/"); server.send(303); }
void handleAlarmeOn()  { Serial2.println("ALARME_ON");  server.sendHeader("Location","/"); server.send(303); }

void parserDonnees(String data) {
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

  server.on("/",           handleRoot);
  server.on("/lum_on",     handleLumOn);
  server.on("/lum_off",    handleLumOff);
  server.on("/lum_auto",   handleLumAuto);
  server.on("/alarme_off", handleAlarmeOff);
  server.on("/alarme_on",  handleAlarmeOn);
  server.begin();
}

void loop() {
  if (Serial2.available()) {
    String message = Serial2.readStringUntil('\n');
    message.trim();
    if (message.length() > 0) {
      parserDonnees(message);
    }
  }
  server.handleClient();
}