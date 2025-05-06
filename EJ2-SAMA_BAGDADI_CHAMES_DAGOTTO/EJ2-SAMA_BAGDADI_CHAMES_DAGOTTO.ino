// GRUPO 3 - SAMA BAGDADI CHAMES DAGOTTO - EJERCICIO 1
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <ESP32Time.h>
#include <U8g2lib.h>
#include <WiFi.h>




#define PANTALLA1 1
#define ESTADO_CONFIRMACION1 2
#define ESTADO_CONFIRMACION2 3
#define PANTALLA2 4
#define SUBIR_HORA 5
#define BAJAR_HORA 6


#define DHTPIN 23      // pin del dht11
#define DHTTYPE DHT11  // tipo de dht (hay otros)


DHT dht(DHTPIN, DHTTYPE);


#define PULSADO LOW
#define N_PULSADO !PULSADO


//Pines
#define PIN_BOTON1 34
#define PIN_BOTON2 35


const int GMT_MAXIMO = 12;
const int GMT_MINIMO = -12;
const int GMT_RESET = 0;


int GMTActual = 0;
bool cambioHecho = LOW;
float temperatura = dht.readTemperature();


const char* ssid = "ORT-IoT";
const char* password = "NuevaIOT$25";
const char* NTP_SERVER = "pool.ntp.org";




int estadoActual = PANTALLA1;  // inicia en PANTALLA1


unsigned long TiempoUltimoCambio = 0;
const long INTERVALO = 1000;
unsigned long TiempoAhora = millis();


U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);  //inicializa la pantallita


ESP32Time rtc;


void setup() {
  pinMode(PIN_BOTON1, INPUT);
  pinMode(PIN_BOTON2, INPUT);


  Serial.begin(115200);
  dht.begin();   // inicializo el dht
  u8g2.begin();  //inicializo la pantallita
  TiempoUltimoCambio = millis();


  WiFi.begin(ssid, password);
  Serial.println("Conectando a wifi");
  while (WiFi.status() != WL_CONNECTED) {
    TiempoAhora = millis();
    if (TiempoAhora - TiempoUltimoCambio >= INTERVALO) {
      TiempoUltimoCambio = millis();
      Serial.println("Intentando conectar...");
    }
  }
  Serial.println("Conexión exitosa");
  establecerGMT();
}




void loop() {
  TiempoAhora = millis();
  String timeActual = rtc.getTime();
  
  float temperatura = dht.readTemperature();


  bool lecturaboton1 = digitalRead(PIN_BOTON1);
  bool lecturaboton2 = digitalRead(PIN_BOTON2);
  //Serial.println(estadoActual);


  switch (estadoActual) {
    case PANTALLA1:
    {
      if (TiempoAhora - TiempoUltimoCambio >= INTERVALO) {
        TiempoUltimoCambio = millis();

<<<<<<< HEAD:Ej2_Grupo4_Bagdadi_Chames_Dagotto_Sama/Ej2_Grupo4_Bagdadi_Chames_Dagotto_Sama.ino
        String timeActual = rtc.getTime();
        
        char bufferTemperatura[7];
=======
        char bufferTemperatura[10];
>>>>>>> 18d4f76d96ef231d28c9b36c227f77d4d3fe068f:EJ2-SAMA_BAGDADI_CHAMES_DAGOTTO/EJ2-SAMA_BAGDADI_CHAMES_DAGOTTO.ino
        char bufferTiempo[10];
        sprintf(bufferTiempo, "%s", timeActual);
        sprintf(bufferTemperatura, "%.2f", temperatura);

          u8g2.clearBuffer();
          u8g2.setFont(u8g2_font_helvB10_tf);
        u8g2.drawStr(10, 15, "Temp:");
        u8g2.drawStr(60, 15, bufferTemperatura);
        u8g2.drawStr(10, 45, "H:");
        u8g2.drawStr(40, 45, bufferTiempo);
          Serial.println(timeActual);
          Serial.println("_");
          u8g2.sendBuffer();
      }


      if (lecturaboton1 == PULSADO && lecturaboton2 == PULSADO) {
        estadoActual = ESTADO_CONFIRMACION1;
      }
      break;
  }

    case ESTADO_CONFIRMACION1:
    {
      if (lecturaboton1 == N_PULSADO && lecturaboton2 == N_PULSADO) {
        estadoActual = PANTALLA2;
      }
      break;
  }

    case PANTALLA2:
    {
      if (TiempoAhora - TiempoUltimoCambio >= INTERVALO)  ///delay sin bloqueo
      {
        TiempoUltimoCambio = millis();  /// importante actualizar el tiempo
        char bufferHora[5];

        sprintf(bufferHora, "%d", GMTActual);


        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB10_tf);
        u8g2.drawStr(15, 15, "Hora:");
        u8g2.drawStr(60, 15, bufferHora);
        u8g2.sendBuffer();
      }

      if (lecturaboton1 == PULSADO && lecturaboton2 == PULSADO) {
        estadoActual = ESTADO_CONFIRMACION2;
      }
      if (lecturaboton1 == PULSADO) {
        cambioHecho = 1;
        estadoActual = BAJAR_HORA;
        Serial.println(cambioHecho);
      }
      if (lecturaboton2 == PULSADO) {
        cambioHecho = 1;
        estadoActual = SUBIR_HORA;
        Serial.println(cambioHecho);
      }
      break;
  }

    case ESTADO_CONFIRMACION2:
    {
      if (lecturaboton1 == N_PULSADO && lecturaboton2 == N_PULSADO) {
        estadoActual = PANTALLA1;
      }
      break;
    }

    case SUBIR_HORA:
    {
      if (lecturaboton1 == PULSADO) {
        estadoActual = ESTADO_CONFIRMACION2;
      }
      //Serial.print("CAMBIO:");
      //Serial.println(cambioHecho);
      if (lecturaboton2 == N_PULSADO) {
        Serial.print("CAMBIO:");
        Serial.println(cambioHecho);
        if (cambioHecho == HIGH) {
          GMTActual = GMTActual + 1;
          establecerGMT();
          cambioHecho = LOW;
          //Serial.println(GMTActual);
        }
        estadoActual = PANTALLA2;
      }
      break;
    }

    case BAJAR_HORA:
    {
      if (lecturaboton2 == PULSADO) {
        estadoActual = ESTADO_CONFIRMACION2;
      }
      //Serial.print("CAMBIO:");
      //Serial.println(cambioHecho);
      if (lecturaboton1 == N_PULSADO) {
        //Serial.print("CAMBIO:");
        //Serial.println(cambioHecho);
        if (cambioHecho == HIGH) {
          GMTActual = GMTActual - 1;
          establecerGMT();
          cambioHecho = LOW;
          //Serial.println(GMTActual);
        }
        estadoActual = PANTALLA2;
      }
      //Serial.println(cambioHecho);
      break;
    }
  }
}

void establecerGMT() {
  const int HORAS_A_SEGUNDOS = 3600;
  long gmtOffset_sec = GMTActual * HORAS_A_SEGUNDOS;
  configTime(gmtOffset_sec, 0, NTP_SERVER);


  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo);
  } else {
    Serial.println("Hubo un error");
  }
<<<<<<< HEAD:Ej2_Grupo4_Bagdadi_Chames_Dagotto_Sama/Ej2_Grupo4_Bagdadi_Chames_Dagotto_Sama.ino
=======
  return;
>>>>>>> 18d4f76d96ef231d28c9b36c227f77d4d3fe068f:EJ2-SAMA_BAGDADI_CHAMES_DAGOTTO/EJ2-SAMA_BAGDADI_CHAMES_DAGOTTO.ino
}
