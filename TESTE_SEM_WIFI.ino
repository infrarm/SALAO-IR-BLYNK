#include <DHT.h>
#include <Adafruit_ImageReader.h>
#include <Adafruit_SPIFlash.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
char auth[] = "GtbDs089gvqc_UGjE3rQeCWzppwOkIIj";

// Your WiFi credentials.
char ssid[] = "Zhone_623E";
char pass[] = "znid309134654";

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

float temp = 0;
float umid = 0;

unsigned long tempo_atual;
unsigned long tempo_ultimo;

int comando = 0;
int temperatura_alvo = 3;

boolean first_init = true;

#define DHTPIN 12          // What digital pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11     // DHT 11
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);

BLYNK_WRITE(V0)
{
  comando = param.asInt(); // assigning incoming value from pin V1 to a variable
}

BLYNK_WRITE(V7)
{
  temperatura_alvo = param.asInt(); // assigning incoming value from pin V1 to a variable
}

void setup() {


  Serial.begin(9600);
  dht.begin();
  
  tft.initR(INITR_BLACKTAB); // Inicializa a tela com um fundo preto
  tft.fillScreen(ST7735_WHITE); // Preenche a tela com a cor branca
  tela_fundo();
  
  Blynk.begin(auth, ssid, pass);
  
}

void loop() {

leitura_dht();
comandos();
seta();
inicializa();
Blynk.run();


//delay(100);
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

//-----------------------------------------------------------------------------------------------------

void inicializa(){
  if (first_init == true){              //INICIALIZA DESLIGADO E ENVIA DADOS PARA APP
    Blynk.virtualWrite(V0, comando);
    Blynk.virtualWrite(V7, temperatura_alvo);
  }
  
}

//-----------------------------------------------------------------------------------------------------

void checa_conn(){

  bool conectado = Blynk.connected();
    if (conectado == true){
      Serial.println("CONECTADO");
    }
    else {
      ESP.restart();
    }
}


//-----------------------------------------------------------------------------------------------------
void leitura_dht(){
  tempo_atual = millis();
  if ((tempo_atual - tempo_ultimo) > 5000){
    tempo_ultimo = tempo_atual;
    Serial.println(tempo_ultimo);  
    float h = dht.readHumidity();
    float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
    umid = h;
    temp = t;
    
    Serial.println(umid, 1);
    Blynk.virtualWrite(V6, umid);
    
    Serial.println(temp, 1);
    Blynk.virtualWrite(V5, temp);

    Serial.println(temperatura_alvo, 1);
    Serial.println(comando, 1);

    
    tft.fillRect(78, 30, 46, 14, ST7735_WHITE);   //LIMPA FUNDO TEMPERATURA
    tft.setFont();
    tft.setTextSize(2);
    tft.setCursor(78, 30);
    tft.print(temp, 1);

    tft.fillRect(78, 50, 46, 14, ST7735_WHITE);   //LIMPA FUNDO UMIDADE
    tft.setCursor(78, 50);
    tft.print(umid, 1);
  
    tft.setTextColor(ST7735_BLACK); // Seta a cor do texto
    tft.setFont(&FreeSansBold12pt7b); // Seleciona a fonte
    tft.setCursor(5, 120); // Move o cursor x y 
    tft.fillRect(7, 88, 50, 35, ST7735_WHITE);
    if (temperatura_alvo == 1)
    {
      tft.print("21"); // Escreve a palavra Vida Com a cor que foi setada na linha acima 
    }
    if (temperatura_alvo == 2)
    {
      tft.print("22"); // Escreve a palavra Vida Com a cor que foi setada na linha acima 
    }
    if (temperatura_alvo == 3)
    {
      tft.print("23"); // Escreve a palavra Vida Com a cor que foi setada na linha acima 
    }
    if (temperatura_alvo == 4)
    {
      tft.print("24"); // Escreve a palavra Vida Com a cor que foi setada na linha acima 
    }


    if (comando == 1){
      //LIGA
      tft.fillRoundRect(7, 130, 50, 25, 3, ST7735_WHITE);
      tft.fillRoundRect(7, 130, 50, 25, 3, ST7735_GREEN);
      tft.setFont(&FreeSansBold9pt7b);
      tft.setTextColor(ST7735_BLACK);
      tft.setTextSize(1);
      tft.setCursor(17, 148);
      tft.print("ON");
    }

    if (comando == 0){
      //DESLIGA
      tft.fillRoundRect(7, 130, 50, 25, 3, ST7735_WHITE);
      tft.fillRoundRect(7, 130, 50, 25, 3, ST7735_RED);
      tft.setFont(&FreeSansBold9pt7b);
      tft.setTextColor(ST7735_BLACK);
      tft.setTextSize(1);
      tft.setCursor(13, 148);
      tft.print("OFF");
    }
  
    if (isnan(h) || isnan(t)) {
      Serial.println("Falha na leitura do DHT22!");
      //return;
    }
  }
}

//-----------------------------------------------------------------------------------------------------

void comandos(){
  
  tft.fillCircle(88, 108, 5, ST7735_GREEN); //STATUS A           
  tft.drawCircle(88, 108, 7, ST7735_BLUE); //SELECIONA A
  
  tft.fillCircle(113, 108, 5, ST7735_GREEN);
  tft.drawCircle(113, 108, 7, ST7735_BLUE);
  
  tft.fillCircle(88, 147, 5, ST7735_GREEN); 
  tft.drawCircle(88, 147, 7, ST7735_BLUE); 
  
  tft.fillCircle(113, 147, 5, ST7735_GREEN);
  tft.drawCircle(113, 147, 7, ST7735_BLUE);//
}

//-----------------------------------------------------------------------------------------------------

void seta(){
  tft.fillTriangle(32, 83, 27, 78, 38, 78, ST7735_BLUE);
  //tft.drawPixel(32, 83, ST7735_BLACK);

}

//-----------------------------------------------------------------------------------------------------
