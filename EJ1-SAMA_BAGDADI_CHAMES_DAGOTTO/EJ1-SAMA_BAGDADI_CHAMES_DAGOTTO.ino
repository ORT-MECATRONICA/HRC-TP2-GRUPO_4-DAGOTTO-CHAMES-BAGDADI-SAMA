// GRUPO 3 - SAMA BAGDADI CHAMES DAGOTTO - EJERCICIO 1

#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <ESP32Time.h>
#include <U8g2lib.h>

#define PANTALLA1 1
#define ESTADO_CONFIRMACION1 2
#define PANTALLA2 3
#define ESTADO_CONFIRMACION2 4
#define SUBIR_HORA 5
#define SUBIR_MIN 6

#define DHTPIN 23      // pin del dht11
#define DHTTYPE DHT11  // tipo de dht (hay otros)

DHT dht(DHTPIN, DHTTYPE);

//Botones
#define PIN_BOTON1 34
#define PIN_BOTON2 35
#define PULSADO LOW
#define N_PULSADO !PULSADO

const int HORA_MAXIMA = 23;
const int MIN_MAXIMO = 59;
const int HORA_RESET = 0;
const int MIN_RESET = 0;
const int HORA_AUMENTO = 1;
const int MIN_AUMENTO = 1;

const int ANIO = 2025;
const int MES = 4;
const int SEGUNDO = 0;
const int DIA = 22;

int estadoActual = PANTALLA1;  // inicia en PANTALLA1

int minutoElegido = MIN_RESET;
int horaElegida = HORA_RESET;
bool cambioHecho = LOW;
float temperatura = O;

unsigned long TiempoUltimoCambio = 0;
const long INTERVALO = 1000;
unsigned long TiempoAhora = millis();

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);  //inicializa la pantallita
ESP32Time rtc;


void setup() {
  Serial.begin(115200);
  TiempoUltimoCambio = millis();

  pinMode(PIN_BOTON1, INPUT);
  pinMode(PIN_BOTON2, INPUT);

  dht.begin();   // inicializo el dht
  u8g2.begin();  //inicializo la pantallita

  rtc.setTime(SEGUNDO, minutoElegido, horaElegida, DIA, MES, ANIO);
}

void loop() {
  TiempoAhora = millis();
  temperatura = dht.readTemperature();
  String timeActual = rtc.getTime();

  bool lecturaBoton1 = digitalRead(PIN_BOTON1);
  bool lecturaBoton2 = digitalRead(PIN_BOTON2);

  switch (estadoActual) {
    case PANTALLA1:
      if (TiempoAhora - TiempoUltimoCambio >= INTERVALO) {
        TiempoUltimoCambio = millis();

        char bufferTemperatura[10];
        char bufferTiempo[10];
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
        char bufferMinuto[5];

        int hora = rtc.getHour(true);
        int minuto = rtc.getMinute();

        sprintf(bufferHora, "%d", hora);
        sprintf(bufferMinuto, "%d", minuto);

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvB10_tf);
        u8g2.drawStr(5, 5, "Hora:");
        u8g2.drawStr(5, 25, bufferHora);
        u8g2.drawStr(5, 45, "Minuto");
        u8g2.drawStr(5, 65, bufferMinuto);
        u8g2.sendBuffer();
      }

      if (lecturaBoton1 == PULSADO && lecturaBoton2 == PULSADO) {
        estadoActual = ESTADO_CONFIRMACION2;
      }

      if (lecturaBoton1 == PULSADO) {
        cambioHecho = HIGH;
        estadoActual = SUBIR_MIN;
      }

      if (lecturaBoton2 == PULSADO) {
        cambioHecho = HIGH;
        estadoActual = SUBIR_HORA;
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
        if (horaElegida == HORA_MAXIMA) {
          horaElegida = HORA_RESET;
          cambioHecho = LOW;
        }

        if (cambioHecho == HIGH) {
          horaElegida = horaElegida + HORA_AUMENTO;
          cambioHecho = LOW;
          Serial.println(horaElegida);
        }

        rtc.setTime(SEGUNDO, minutoElegido, horaElegida, DIA, MES, ANIO);
        estadoActual = PANTALLA2;
      }
      break;


    case SUBIR_MIN:
      if (lecturaBoton2 == PULSADO) {
        estadoActual = ESTADO_CONFIRMACION2;
      }

      if (lecturaBoton1 == N_PULSADO) {
        if (minutoElegido == MIN_MAXIMO) {
          minutoElegido = MIN_RESET;
          cambioHecho = LOW;
        }

        if (cambioHecho == HIGH) {
          minutoElegido = minutoElegido + MIN_AUMENTO;
          cambioHecho = LOW;
          Serial.println(minutoElegido);
        }

        rtc.setTime(SEGUNDO, minutoElegido, horaElegida, DIA, MES, ANIO);
        estadoActual = PANTALLA2;
      }
      break;
  }
}
