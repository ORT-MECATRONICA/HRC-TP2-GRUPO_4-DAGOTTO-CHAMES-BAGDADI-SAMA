// GRUPO 3 - SAMA BAGDADI CHAMES DAGOTTO - EJERCICIO 2

#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <ESP32Time.h>
#include <U8g2lib.h>
#include <WiFi.h>

#define PANTALLA1 1
#define ESTADO_CONFIRMACION1 2
#define PANTALLA2 3
#define ESTADO_CONFIRMACION2 4
#define SUBIR_HORA 5
#define BAJAR_HORA 6

#define DHTPIN 23      // pin del dht11
#define DHTTYPE DHT11  // tipo de dht (hay otros)

DHT dht(DHTPIN, DHTTYPE);

//Botones
#define PIN_BOTON1 34
#define PIN_BOTON2 35
#define PULSADO LOW
#define N_PULSADO !PULSADO

const int GMT_MAXIMO = 12;
const int GMT_MINIMO = -12;
const int GMT_RESET = 0;
const int CAMBIO_GMT = 1;

int GMTActual = GMT_RESET;
bool cambioHecho = LOW;
float temperatura = 0;

const char* SSID = "ORT-IoT";
const char* PASSWORD = "NuevaIOT$25";
const char* NTP_SERVER = "pool.ntp.org";

int estadoActual = PANTALLA1;  // inicia en PANTALLA1

unsigned long TiempoUltimoCambio = 0;
const long INTERVALO = 1000;
unsigned long TiempoAhora = millis();

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);  //inicializa la pantallita
ESP32Time rtc;


void setup() {
  Serial.begin(115200);

  pinMode(PIN_BOTON1, INPUT);
  pinMode(PIN_BOTON2, INPUT);

  dht.begin();   // inicializo el dht
  u8g2.begin();  //inicializo la pantallita

  WiFi.begin(SSID, PASSWORD);
  Serial.println("Conectando a wifi");
  TiempoUltimoCambio = millis();
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
  temperatura = dht.readTemperature();

  bool lecturaBoton1 = digitalRead(PIN_BOTON1);
  bool lecturaBoton2 = digitalRead(PIN_BOTON2);

  switch (estadoActual) {
    case PANTALLA1:
      if (TiempoAhora - TiempoUltimoCambio >= INTERVALO) {
        TiempoUltimoCambio = millis();

        char bufferTemperatura[5];
        char bufferTiempo[5];
        sprintf(bufferTiempo, "%s", timeActual);
        sprintf(bufferTemperatura, "%.2f", temperatura);

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB10_tf);
        u8g2.drawStr(10, 10, "Tiempo actual");
        u8g2.drawStr(10, 25, bufferTiempo);
        u8g2.drawStr(10, 40, "Temperatura");
        u8g2.drawStr(10, 55, bufferTemperatura);
        Serial.println(timeActual);
        Serial.println("_");
        u8g2.sendBuffer();
      }

      if (lecturaBoton1 == PULSADO && lecturaBoton2 == PULSADO) {
        estadoActual = ESTADO_CONFIRMACION1;
      }
      break;


    case ESTADO_CONFIRMACION1:
      if (lecturaBoton1 == N_PULSADO && lecturaBoton2 == N_PULSADO) {
        estadoActual = PANTALLA2;
      }
      break;


    case PANTALLA2:
      if (TiempoAhora - TiempoUltimoCambio >= INTERVALO)  ///delay sin bloqueo
      {
        TiempoUltimoCambio = millis();  /// importante actualizar el tiempo
        char bufferHora[5];

        sprintf(bufferHora, "%d", GMTActual);

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB10_tf);
        u8g2.drawStr(5, 5, "Hora:");
        u8g2.drawStr(5, 25, bufferHora);
        u8g2.sendBuffer();
      }

      if (lecturaBoton1 == PULSADO && lecturaBoton2 == PULSADO) {
        estadoActual = ESTADO_CONFIRMACION2;
      }

      if (lecturaBoton1 == PULSADO) {
        cambioHecho = HIGH;
        estadoActual = BAJAR_HORA;
        //Serial.println(cambioHecho);
      }
      if (lecturaBoton2 == PULSADO) {
        cambioHecho = HIGH;
        estadoActual = SUBIR_HORA;
        //Serial.println(cambioHecho);
      }
      break;


    case ESTADO_CONFIRMACION2:
      if (lecturaBoton1 == N_PULSADO && lecturaBoton2 == N_PULSADO) {
        estadoActual = PANTALLA1;
      }
      break;


    case SUBIR_HORA:
      if (lecturaBoton1 == PULSADO) {
        estadoActual = ESTADO_CONFIRMACION2;
      }

      if (lecturaBoton2 == N_PULSADO) {
        if (GMTActual == GMT_MAXIMO) {
          GMTActual = GMT_RESET;
          cambioHecho = LOW;
          //Serial.println(GMTActual);
        }

        if (cambioHecho == HIGH) {
          GMTActual = GMTActual + CAMBIO_GMT;
          cambioHecho = LOW;
          //Serial.println(GMTActual);
        }
        establecerGMT();
        estadoActual = PANTALLA2;
      }
      break;


    case BAJAR_HORA:
      if (lecturaBoton2 == PULSADO) {
        estadoActual = ESTADO_CONFIRMACION2;
      }

      if (lecturaBoton1 == N_PULSADO) {
        if (GMTActual == GMT_MINIMO) {
          GMTActual = GMT_RESET;
          cambioHecho = LOW;
          //Serial.println(GMTActual);
        }

        if (cambioHecho == HIGH) {
          GMTActual = GMTActual - CAMBIO_GMT;
          cambioHecho = LOW;
        }
        establecerGMT();
        estadoActual = PANTALLA2;
      }
      break;
  }
}

/*
DESCRIPCIÓN DE LA FUNCIÓN: establece la hora usando el GMT elegido por el usuario (GMTActual)
Matte profundiza esto dsp :)
*/

void establecerGMT() {
  const int HORAS_A_SEGUNDOS = 3600;
  const int DAYLIGHT_OFFSET = 0;
  long gmtOffset_sec = GMTActual * HORAS_A_SEGUNDOS;
  configTime(gmtOffset_sec, DAYLIGHT_OFFSET, NTP_SERVER);

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo);
  } else {
    Serial.println("Hubo un error");
  }
  return;
}