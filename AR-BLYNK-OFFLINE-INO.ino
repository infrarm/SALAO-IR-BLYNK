#define NAMEandVERSION "OfflineTask_Reconnect_V1.0"
/*
  This sketch is based on an example posted by Gunner with some modifications added in place to make it able to reconnect after Wifi or Server connection failures.
  It is able to check if it is a Wifi or a server connection issue and recover it when it is possible
  The MCU runs the task every second - It turns the builtin led on and off (allways) and post the millis/1000 to blynk server (only when a connection is available).
*/
//TELA
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <DHT.h>
#include <Button.h>

float temp = 0;
float umid = 0;

#define DHTPIN 12          // What digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);

#define D3 0
#define D4 2 
#define D5 14 
#define D7 13 
#define D8 15 
#define TFT_CS     D8
#define TFT_RST    D4  
#define TFT_DC     D3
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST); // Instância do objeto tipo Adafruit_ST7735 que recebe como argumentos os pinos de controle
#define TFT_SCLK D5   
#define TFT_MOSI D7

//#define BLYNK_DEBUG
#define BLYNK_TIMEOUT_MS  500  // must be BEFORE BlynkSimpleEsp8266.h doesn't work !!!
#define BLYNK_HEARTBEAT   17   // must be BEFORE BlynkSimpleEsp8266.h works OK as 17s
#define BLYNK_PRINT Serial    
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//#include "config.h"

BlynkTimer timer;


bool on = 0;
bool online = 0;

// You should get Auth Token in the Blynk App.
char auth[] = "Wpw8yfMzMY6-akX5e6KHbj1htaxtDvCd";

// Your WiFi credentials.
char ssid[] = "DELTA-28750";
char pass[] = "06128938";

char server[] = "blynk-cloud.com";  // URL for Blynk Cloud Server
int port = 8080;

int comando = 0;

Button MODE(5); // Connect your button between pin 2 and GND
Button SET(4); // Connect your button between pin 3 and GND

int set_id = 1;
BLYNK_WRITE(V0)
{
  comando = param.asInt(); // assigning incoming value from pin V1 to a variable
}

void setup() {
  //INICIALIZA TELA
  tft.initR(INITR_BLACKTAB); // Inicializa a tela com um fundo preto
  tft.fillScreen(ST7735_WHITE); // Preenche a tela com a cor branca

  tela_fundo();
  print_set();

  dht.begin();
  
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(ssid, pass);  // Non-blocking if no WiFi available
  Blynk.config(auth, server, port);
  Blynk.connect();


  CheckConnection();// It needs to run first to initiate the connection.Same function works for checking the connection!
  timer.setInterval(5000L, CheckConnection); 
  timer.setInterval(1000L, myTimerEvent);
  timer.setInterval(10000L, dados_DHT);
  timer.setInterval(15000L, send_DHT);          
}

void loop() {
  if(Blynk.connected()){
    Blynk.run();
  }
  timer.run();

  if (MODE.pressed()){
      limpa_set();
      print_set();
      set_id++;
      if (set_id == 7){
        set_id = 1;
      }
  }
}


void CheckConnection(){    // check every 11s if connected to Blynk server
  if(!Blynk.connected()){
    online = 0;
    yield();
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Sem conexão com Wifi! Conectando...");
      WiFi.begin(ssid, pass);
      delay(400); //give it some time to connect
      if (WiFi.status() != WL_CONNECTED)
      {
        Serial.println("Falha na conexão com WIFI!");
        online = 0;
      }
      else
      {
        Serial.println("Conectado na rede WIFI!");
      }
    }
    
    if ( WiFi.status() == WL_CONNECTED && !Blynk.connected() )
    {
      Serial.println("Sem conexão com Blynk Server! Conectando..."); 
      Blynk.connect();  // // It has 3 attempts of the defined BLYNK_TIMEOUT_MS to connect to the server, otherwise it goes to the enxt line 
      if(!Blynk.connected()){
        Serial.println("Falha na conexão com Blynk Server!"); 
        online = 0;
      }
      else
      {
        online = 1;
      }
    }
  }
  else{
    Serial.println("Conectado ao Blynk Server!"); 
    online = 1;    
  }
}


void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  if (online == 1)
  {
    Blynk.virtualWrite(V15, millis() / 1000); 
    Serial.println(comando);   
  }
  else 
  {
    Serial.println("Trabalhando Offline!");
    Serial.println(comando); 
  }
  
  if (on == 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)    
    on = 1;
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW                  
    on = 0;
  }
  Serial.println(millis() / 1000);
}

void dados_DHT(){

    float h = dht.readHumidity();
    float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

    if (isnan(h) || isnan(t)) {
      Serial.println("Falha na leitura do DHT!");
      //return;
    }
    else{

     umid = h;
     temp = t;
      
      tft.fillRect(78, 30, 48, 34, ST7735_WHITE);   //LIMPA FUNDO TEMP E UMID
      
      //tft.fillRect(78, 30, 46, 14, ST7735_GREEN);   //LIMPA FUNDO TEMPERATURA
      //tft.fillRect(78, 50, 46, 14, ST7735_GREEN);   //LIMPA FUNDO UMIDADE

      
      tft.setFont();
      tft.setTextColor(ST7735_BLACK);
      tft.setTextSize(2);
      
      tft.setCursor(78, 30);
      tft.print(t, 1);

      tft.setCursor(78, 50);
      tft.print(h, 1);
    }

  
}

void send_DHT(){
      if (online == 1){
        //Serial.println(h, 1);
        Blynk.virtualWrite(V6, umid); // PRECISA SER COLOCADO NA ROTINA DE COMUNICAÇÃO**PROBLEMA NO DISPLAY
    
        //Serial.println(t, 1);
        Blynk.virtualWrite(V5, temp);
      }
}

void limpa_set(){
    //FUNDO BRANCO
  tft.fillTriangle(32, 83, 27, 78, 38, 78, ST7735_WHITE); //SELECIONA TEMP
  tft.drawCircle(88, 108, 7, ST7735_WHITE); //SELECIONA A
  tft.drawCircle(113, 108, 7, ST7735_WHITE); //SELECIONA B
  tft.drawCircle(88, 147, 7, ST7735_WHITE); //SELECIONA C
  tft.drawCircle(113, 147, 7, ST7735_WHITE); //SELECIONA D
  tft.fillRoundRect(6, 129, 51, 26, 4, ST7735_WHITE);
}

void print_set(){
  if (set_id == 1){
    tft.fillTriangle(32, 83, 27, 78, 38, 78, ST7735_BLUE); //SELECIONA TEMP
    //delay(2000);
  }
  if (set_id == 2){
    tft.drawCircle(88, 108, 7, ST7735_BLUE); //SELECIONA A
    //delay(2000);
  }
  if (set_id == 3){
    tft.drawCircle(113, 108, 7, ST7735_BLUE); //SELECIONA B
    //delay(2000);
  }
  if (set_id == 4){
    tft.drawCircle(88, 147, 7, ST7735_BLUE); //SELECIONA C
    //delay(2000);
  }
  if (set_id == 5){
    tft.drawCircle(113, 147, 7, ST7735_BLUE); //SELECIONA D
    //delay(2000);
  }
  if (set_id == 6){
    tft.fillRoundRect(6, 129, 51, 26, 4, ST7735_BLUE);
    //delay(2000);
  } 
}

void tela_fundo(){
  tft.setTextColor(ST7735_BLACK);
  tft.setFont(&FreeSansBold12pt7b);
  tft.setCursor(3, 21);
  tft.print("O-BOT");

  tft.setFont(); 
  tft.setTextSize(1);
  tft.setCursor(83, 4);
  tft.print("ORION3D");
  tft.setCursor(83, 14);
  tft.print("IoT");

  tft.setFont(&FreeSansBold9pt7b);
  tft.setTextSize(1);
  tft.setCursor(3, 42);
  tft.print("TEMP");
  tft.setCursor(3, 62);
  tft.print("UMID"); 

  tft.drawLine(0, 70, 127, 70, ST7735_BLACK); //PRIMEIRA LINHA HORIZONTAL

  tft.fillRect(79, 79, 43, 21, ST7735_BLACK); //QUADRO PRETO PARA LETRA BRANCA
  tft.fillRect(79, 118, 43, 21, ST7735_BLACK);
  
    //ESCREVE UNIDADES A B C e D
  tft.setFont();
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  //A
  tft.setCursor(83, 82);
  tft.print("A");
  //C
  tft.setCursor(83, 121);
  tft.print("C");
  //B
  tft.setCursor(108, 82);
  tft.print("B");
  //D
  tft.setCursor(108, 121);
  tft.print("D");
 }
