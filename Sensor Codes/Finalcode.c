#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>
#include <PZEM004Tv30.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

const char *ssid = "JAGDISH 9498";
const char *password = "123456789";

WebServer server(80);
DHT dht(26, DHT11);
PZEM004Tv30 pzem(&Serial1, 16, 17);

LiquidCrystal_I2C lcd(0x27, 20, 4); 

void handleRoot() {
  char msg[2000];

  float temp = readDHTTemperature();
  float humidity = readDHTHumidity();
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();

  snprintf(msg, sizeof(msg),
           "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 Sensor Data</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body>\
      <h2>ESP32 Sensor Data</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperature</span>\
        <span>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humidity</span>\
        <span>%.2f</span>\
        <sup class='units'>&percnt;</sup>\
      </p>\
      <p>\
        <i class='fas fa-bolt' style='color:#ffac1b;'></i>\
        <span class='dht-labels'>Voltage</span>\
        <span>%.2f</span>\
        <sup class='units'>V</sup>\
      </p>\
      <p>\
        <i class='fas fa-charging-station' style='color:#ffac1b;'></i>\
        <span class='dht-labels'>Current</span>\
        <span>%.2f</span>\
        <sup class='units'>A</sup>\
      </p>\
      <p>\
        <i class='fas fa-bolt' style='color:#ffac1b;'></i>\
        <span class='dht-labels'>Power</span>\
        <span>%.2f</span>\
        <sup class='units'>W</sup>\
      </p>\
      <p>\
        <i class='fas fa-bolt' style='color:#ffac1b;'></i>\
        <span class='dht-labels'>Energy</span>\
        <span>%.2f</span>\
        <sup class='units'>kWh</sup>\
      </p>\
      <p>\
        <i class='fas fa-bolt' style='color:#ffac1b;'></i>\
        <span class='dht-labels'>Frequency</span>\
        <span>%.2f</span>\
        <sup class='units'>Hz</sup>\
      </p>\
      <p>\
        <i class='fas fa-bolt' style='color:#ffac1b;'></i>\
        <span class='dht-labels'>Power Factor</span>\
        <span>%.2f</span>\
      </p>\
  </body>\
</html>",
           temp, humidity, voltage, current, power, energy, frequency, pf
          );

  server.send(200, "text/html", msg);

  updateLCD(temp, humidity);

  Serial.print("Temperature: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Voltage: ");
  Serial.println(voltage);
  Serial.print("Current: ");
  Serial.println(current);
  Serial.print("Power: ");
  Serial.println(power);
  Serial.print("Energy: ");
  Serial.println(energy);
  Serial.print("Frequency: ");
  Serial.println(frequency);
  Serial.print("Power Factor: ");
  Serial.println(pf);
}

void setup(void) {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(); 
  lcd.backlight(); 
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(5000); 
}

float readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  else {
    Serial.print("Temperature: ");
    Serial.println(t);
    return t;
  }
}

float readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  else {
    Serial.print("Humidity: ");
    Serial.println(h);
    return h;
  }
}

void updateLCD(float temp, float humidity) {
  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");
}