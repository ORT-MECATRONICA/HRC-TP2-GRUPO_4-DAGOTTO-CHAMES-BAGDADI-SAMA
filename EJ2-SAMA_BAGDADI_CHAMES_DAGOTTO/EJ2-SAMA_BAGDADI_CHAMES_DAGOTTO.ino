// GRUPO 3 - SAMA BAGDADI CHAMES DAGOTTO - EJERCICIO 2


// Incluyo librerias
#include <DHT.h> //Sensor DHT de temperatura
#include <DHT_U.h>
#include <Adafruit_Sensor.h> //Librería adicional del sensor DHT
#include <ESP32Time.h>       //Librería de reloj interno
#include <U8g2lib.h>         //Oled
#include <WiFi.h>            //Modulo de conexion a wifi


// Defino estados
#define PANTALLA1 1
#define ESTADO_CONFIRMACION1 2
#define PANTALLA2 3
#define ESTADO_CONFIRMACION2 4
#define SUBIR_HORA 5
#define BAJAR_HORA 6


#define DHTPIN 23      // pin del dht11
#define DHTTYPE DHT11  // tipo de dht (hay otros)


DHT dht(DHTPIN, DHTTYPE); // Configura el dht. Le dice en qué pin está y qué tipo de sensor es (ver variables de las líneas 19 y 20)


//Botones y en que pin están
#define PIN_BOTON1 34
#define PIN_BOTON2 35


//Estados de los botones
#define PULSADO LOW
#define N_PULSADO !PULSADO


//Constantes de ayuda para la sintaxis
const int GMT_MAXIMO = 12;  //Hora máxima
const int GMT_MINIMO = -12; //Hora mínima
const int GMT_RESET = 0;    //Reset de la hora, valor 0
const int CAMBIO_GMT = 1;   //Cuanto cambia la hora


//Variables que se van a usar
int GMTActual = GMT_RESET; //Hora actual. Al principio vale 0
bool cambioHecho = LOW; //cambioHecho se usa para que al apretar cualquier boton, el valor aumente 1 sola vez
float temperatura = 0; //Temperatura inicializada en el 0


//Otorgamos acceso a Time.h a wifi
const char* SSID = "ORT-IoT";
const char* PASSWORD = "NuevaIOT$25";
const char* NTP_SERVER = "pool.ntp.org";


int estadoActual = PANTALLA1;  // inicia en PANTALLA1


unsigned long TiempoUltimoCambio = 0; //Variable que almacenará millis. Explicada mejor abajo
const long INTERVALO = 1000; //Cuanto tiempo dura el delay sin bloqueo
unsigned long TiempoAhora = millis(); //Guarda los milisegundos desde que se ejecutó el programa


U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);  //inicializa la pantallita
ESP32Time rtc; //Configuración del esp32time. Sincroniza el reloj interno con el real time clock, sin offset.




void setup() {
  Serial.begin(115200); //Baudios


  //Modo de los botones
  pinMode(PIN_BOTON1, INPUT);
  pinMode(PIN_BOTON2, INPUT);


  dht.begin();   // inicializo el dht
  u8g2.begin();  //inicializo la pantallita


  //Otorgamos acceso a Time.h a wifi con datos constantes definidos anteriormente
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Conectando a wifi"); //Confirmación de inicio de intento de conexión
  TiempoUltimoCambio = millis(); //Cuantos milisegundos pasaron desde el último cambio
  while (WiFi.status() != WL_CONNECTED) { //Mientras el wifi esté conectándose, que cada un intervalo imprima que intenta conectarse
    TiempoAhora = millis();
    if (TiempoAhora - TiempoUltimoCambio >= INTERVALO) {
      TiempoUltimoCambio = millis();
      Serial.println("Intentando conectar...");
    }
  }
  Serial.println("Conexión exitosa"); //Cuando sale del while (Se conecta), avisa del éxito de conexión


  establecerGMT();
}




void loop() {
  TiempoAhora = millis(); //Milisegundos desde que se ejecutó el código
  temperatura = dht.readTemperature(); //Temperatura medida por el DHT
  String timeActual = rtc.getTime(); //Tiempo obtenido por el RTC


  //Lectura de bitibes
  bool lecturaBoton1 = digitalRead(PIN_BOTON1);
  bool lecturaBoton2 = digitalRead(PIN_BOTON2);


  //Inicializo máquina de estados
  switch (estadoActual) { //Empieza en el estado que estadoActual tiene como valor
    case PANTALLA1: //Pantalla 1, muestra hora establecida y temperatura
      if (TiempoAhora - TiempoUltimoCambio >= INTERVALO) { //Delay sin bloqueo
        TiempoUltimoCambio = millis(); //actualizo el tiempo


        char bufferTemperatura[10]; //Reserva espacios en el búfer para la variable de temperatura
        char bufferTiempo[7]; //Reserva espacios en el búfer para la variable de tiempo
       
        sprintf(bufferTiempo, "%s", timeActual); //Guarda la variable timeActual en los espacios reservados para bufferTiempo
        sprintf(bufferTemperatura, "%.2f", temperatura); //Guarda la variable temperatura en los espacios reservados para bufferTemperatura


        u8g2.clearBuffer(); //Borra lo almacenado en el búfer anteriormente
        u8g2.setFont(u8g2_font_helvB10_tf); //La fuente del texto del oled
        u8g2.drawStr(10, 10, "Tiempo actual"); //Los primeros números son coordenadas en la pantalla
        u8g2.drawStr(10, 25, bufferTiempo);
        u8g2.drawStr(10, 40, "Temperatura");
        u8g2.drawStr(10, 55, bufferTemperatura);
        Serial.println(timeActual); //Se imprime también por monitor serial
        Serial.println("_"); //Un string de separación, no relevante
        u8g2.sendBuffer(); //Envía los datos guardados en el búfer a la pantalla
      }


      if (lecturaBoton1 == PULSADO && lecturaBoton2 == PULSADO) { //Si los dos botones son pulsados, se sale de pantalla 1 y se entra a un estado de confirmación (Línea 123)
        estadoActual = ESTADO_CONFIRMACION1;
      }
      break;




    case ESTADO_CONFIRMACION1:
      if (lecturaBoton1 == N_PULSADO && lecturaBoton2 == N_PULSADO) { //Si los dos botones se dejan de pulsar, se entra a la pantalla 2 (Línea 130)
        estadoActual = PANTALLA2;
      }
      break;




    case PANTALLA2:
      if (TiempoAhora - TiempoUltimoCambio >= INTERVALO)  ///delay sin bloqueo
      {
        TiempoUltimoCambio = millis();  /// importante actualizar el tiempo
        char bufferHora[5]; //Reserva espacios en el búfer para la variable de hora


        sprintf(bufferHora, "%d", GMTActual); //Guarda la variable GMTActual en los espacios reservados para bufferHora


        u8g2.clearBuffer(); //Véase línea 106 a 114
        u8g2.setFont(u8g2_font_helvB10_tf);
        u8g2.drawStr(5, 5, "Hora:");
        u8g2.drawStr(5, 25, bufferHora);
        u8g2.sendBuffer();
      }


      if (lecturaBoton1 == PULSADO && lecturaBoton2 == PULSADO) { //Si los dos botones son pulsados, se sale de pantalla 2 y se entra a un estado de confirmación (Línea 162)
        estadoActual = ESTADO_CONFIRMACION2;
      }


      if (lecturaBoton1 == PULSADO) { //Si solo el boton 1 es pulsado, entra al estado de bajar hora (Línea 192) y cambioHecho devuelve True
        cambioHecho = HIGH;
        estadoActual = BAJAR_HORA;
        //Serial.println(cambioHecho);
      }
      if (lecturaBoton2 == PULSADO) { //Si solo el boton 2 es pulsado, entra al estado de subir hora (Línea 169) y cambioHecho devuelve True
        cambioHecho = HIGH;
        estadoActual = SUBIR_HORA;
        //Serial.println(cambioHecho);
      }
      break;




    case ESTADO_CONFIRMACION2:
      if (lecturaBoton1 == N_PULSADO && lecturaBoton2 == N_PULSADO) { //Si los dos botones se dejan de pulsar, se entra a la pantalla 1 (Línea 96)
        estadoActual = PANTALLA1;
      }
      break;




    case SUBIR_HORA:
      if (lecturaBoton1 == PULSADO) { //Si el boton 1 es pulsado, se entra al estado de confirmación 2 (para llegar a SUBIR_HORA se necesitaba pulsar el boton 2)
        estadoActual = ESTADO_CONFIRMACION2;
      }


      if (lecturaBoton2 == N_PULSADO) { //Si se deja de presionar el boton 2 sin que se presione el boton 1
        if (horaElegida == GMT_MAXIMO) { //Si GMTActual supera las 23 hs se reinicia, ya que empieza el nuevo día
          horaElegida = GMT_RESET;
          cambioHecho = LOW;
        }


        if (cambioHecho == HIGH) { //Si cambio hecho sigue siendo HIGH, la hora aumenta 1, y se imprime en el monitor serial
          GMTActual = GMTActual + CAMBIO_GMT;
          cambioHecho = LOW;
          //Serial.println(GMTActual);
        }
        establecerGMT(); //Vuelve a establecer el tiempo actualizado y el estado vuelve a ser la pantalla 2
        estadoActual = PANTALLA2;
      }
      break;




    case BAJAR_HORA:
      if (lecturaBoton2 == PULSADO) { //Si el boton 2 es pulsado, se entra al estado de confirmación 2 (para llegar a BAJAR_HORA se necesitaba pulsar el boton 1)
        estadoActual = ESTADO_CONFIRMACION2;
      }


      if (lecturaBoton1 == N_PULSADO) { //Si se deja de presionar el boton 1 sin que se presione el boton 2
        if (GMTActual == GMT_MINIMO) {  //Si GMTActual supera las 23 hs se reinicia, ya que empieza el nuevo día
          GMTActual = GMT_RESET;
          cambioHecho = LOW;
          //Serial.println(GMTActual);
        }


        if (cambioHecho == HIGH) { //Si cambio hecho sigue siendo HIGH, la hora aumenta en 1, y se imprime en el monitor serial
          GMTActual = GMTActual - CAMBIO_GMT;
          cambioHecho = LOW;
        }
        establecerGMT(); //Vuelve a establecer el tiempo actualizado y el estado vuelve a ser la pantalla 2
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
  retu

