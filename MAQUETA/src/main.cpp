/*
  Project: Building Occupancy Simulation
  Description:
  LED-based building simulation with random human and worker activity,
  smooth fade transitions, and time-based behavioral patterns.

  Author: Janeth Arellano V.
  Date: 2026-01-08
  Hardware: ESP32 + NeoPixel RGBW
  Notes:
  - Designed for realistic habitation simulation
  - Non-blocking logic using millis()
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
/*--------------------------------------------------------------------*/

#define IN_PIN 5    // The ESP32 pin GPIO16 connected to NeoPixel
#define NUM_LED 151 // The number of LEDs (pixels) on NeoPixel

#define DEPARTMENTS_END 91
#define FLOOR1_END 103
#define FLOOR2_END 115
#define FLOOR3_END 127
#define FLOOR4_END 139

/*--------------------------------------------------------------------*/

Adafruit_NeoPixel tira1 = Adafruit_NeoPixel(NUM_LED, IN_PIN, NEO_RGBW + NEO_KHZ800);

struct LED
{
  uint16_t id;
  uint16_t set;
  uint8_t brightness;
  uint8_t currentBrightness;
};

LED datos[NUM_LED] = {};
bool loop_iteration;

unsigned long lastPatternChange = 0;
unsigned long lastHumanEvent = 0;
unsigned long lastWorkerEvent = 0;

const unsigned long PATTERN_INTERVAL = 300000; // 5 min
const unsigned long HUMAN_INTERVAL_MIN = 20000;
const unsigned long HUMAN_INTERVAL_MAX = 60000;
const unsigned long WORKER_INTERVAL_MIN= 27000;
const unsigned long WORKER_INTERVAL_MAX= 52000;


unsigned long nextHumanInterval;
unsigned long nextWorkerInterval;


void INITIALIZATION();
void UPDATE_LOGIC();
void RENDER_LEDS();

void OFFICE_LEVEL();
void CLEAN();
void SET_ASIGNATION();
void ALL_LIGHTS_AT_ONCE();
void APARTMENT();
void ALL_LIGHTS_SPIRAL_ANIMATION(uint16_t);
void RANDOM_HUMAN_EVENT(uint16_t);
void RANDOM_WORKER_EVENT(uint16_t);
void ALL_LIGHTS_FADE();


void setup()
{
  Serial.begin(115200);
  delay(500);
  randomSeed(esp_random());
  // put your setup code here, to run once:
  tira1.begin();
  tira1.clear();
  tira1.show();
  INITIALIZATION();
  ALL_LIGHTS_SPIRAL_ANIMATION(50);
  ALL_LIGHTS_FADE();
  loop_iteration = true;
  nextHumanInterval = random(HUMAN_INTERVAL_MIN, HUMAN_INTERVAL_MAX);
  lastHumanEvent = millis();
  nextWorkerInterval = random(WORKER_INTERVAL_MIN,WORKER_INTERVAL_MAX);
  lastWorkerEvent = millis();
  
}

void loop()
{

  unsigned long now = millis();

  if (now - lastPatternChange > PATTERN_INTERVAL || loop_iteration)
  {
    UPDATE_LOGIC(); // Cambia patron aleatorio
    lastPatternChange = now;
    loop_iteration = false;
  }

  if (now - lastHumanEvent > nextHumanInterval)
  {
    uint8_t humans = random(2, 5);

    RANDOM_HUMAN_EVENT(humans); // solo 1 LED

    lastHumanEvent = now;
    nextHumanInterval = random(HUMAN_INTERVAL_MIN, HUMAN_INTERVAL_MAX);
  }

  if(now - lastWorkerEvent > nextWorkerInterval){

    uint8_t workers = random(3, 9);
    RANDOM_WORKER_EVENT(workers);

    lastWorkerEvent = now;
    nextWorkerInterval = random(WORKER_INTERVAL_MIN, WORKER_INTERVAL_MAX);

  }

  RENDER_LEDS();
}

void INITIALIZATION()
{
  CLEAN();
  SET_ASIGNATION();
}

void UPDATE_LOGIC()
{
  OFFICE_LEVEL();
  APARTMENT();
}

void RENDER_LEDS()
{
  //ALL_LIGHTS_AT_ONCE();
  ALL_LIGHTS_FADE();
  //ALL_LIGHTS_SPIRAL_ANIMATION(50);
}

void CLEAN()
{ // SE LLAMA SIEMPRE AL INICIO
  for (uint16_t i = 0; i < NUM_LED; i++)
  { // Este for es para asignar un id y un estado de apagado a todos los leds
    datos[i].id = i;
    datos[i].set = 0;
    datos[i].brightness = 0;
    datos[i].currentBrightness =0;
    
  }
}
void SET_ASIGNATION()
{
  for (uint16_t i = 0; i < NUM_LED; i++)
  {
    if (i < DEPARTMENTS_END) // departamentos
    {
      datos[i].set = 0;
    }
    else if (i < FLOOR1_END) // primer piso oficinas
    {
      datos[i].set = 1;
    }
    else if (i < FLOOR2_END) // segundo piso oficinas
    {
      datos[i].set = 2;
    }
    else if (i < FLOOR3_END) // tercer piso oficinas
    {
      datos[i].set = 3;
    }
    else if (i < FLOOR4_END) // cuarto piso oficinas
    {
      datos[i].set = 4;
    }
    else // quinto piso oficinas
    {
      datos[i].set = 5;
    }
  }
}

void OFFICE_LEVEL()
{
  uint8_t randomOfficeLevel = random(1, 6); // piso que no debe prender

  for (uint16_t i = 0; i < NUM_LED; i++)
  {
    if (datos[i].set == randomOfficeLevel && randomOfficeLevel != 1)
    {
      datos[i].brightness = 0;
    }
    else
    {
      datos[i].brightness = 250;
    }
  }
}
void APARTMENT()
{
  int16_t lastIndex = -1;
  for (uint16_t i = 0; i < 26; i++)
  {
    long apartmentIndex = random(DEPARTMENTS_END);//Variable para segurarme que el numero random no se repita
    do
    {
      apartmentIndex = random(DEPARTMENTS_END);// Departamento que se apagara
    } while (apartmentIndex == lastIndex);

    datos[apartmentIndex].brightness = 0;
    lastIndex = apartmentIndex;
  }
 
}
void RANDOM_HUMAN_EVENT(uint16_t times_it_happens)
{
  for (uint16_t i = 0; i < times_it_happens; i++)
  {

    uint16_t index = random(DEPARTMENTS_END);

    if (datos[index].brightness == 0)
    {
      datos[index].brightness = 250; // alguien llegó a casita
    }
    else
    {
      datos[index].brightness = 0; // alguien salió de casita
    }
  }
}
void RANDOM_WORKER_EVENT(uint16_t times_it_happens)
{
  for (uint16_t i = 0; i < times_it_happens; i++)
  {

    uint16_t index = random(FLOOR1_END, NUM_LED);

    if (datos[index].brightness == 0)
    {
      datos[index].brightness = 250; // alguien llego a su trabajo
    }
    else
    {
      datos[index].brightness = 0; // alguien salió de su trabajo 
    }
  }
}

void ALL_LIGHTS_AT_ONCE()
{
  for (uint16_t i = 0; i < NUM_LED; i++)
  {
    if (datos[i].brightness != 0)
    {
      tira1.setPixelColor(i, 0, 0, 0, datos[i].brightness);
    }
    else
    {
      tira1.setPixelColor(i, 0);
    }
  }
  tira1.show();
}
void ALL_LIGHTS_SPIRAL_ANIMATION(uint16_t delayMs)
{
  tira1.clear();
  for (uint16_t i = 0; i < NUM_LED; i++)
  {
    datos[i].brightness=250;
    tira1.setPixelColor(i, 0, 0, 0, datos[i].brightness);
    datos[i].currentBrightness=250;
    tira1.show();
    delay(delayMs);
  }
}
void ALL_LIGHTS_FADE(){

  bool letsUpdate = false;

  for (uint16_t i = 0; i < NUM_LED; i++) {
    if (datos[i].currentBrightness < datos[i].brightness) {
      datos[i].currentBrightness++;
      letsUpdate = true;
    } 
    else if (datos[i].currentBrightness > datos[i].brightness) {
      datos[i].currentBrightness--;
      letsUpdate = true;
    }

    tira1.setPixelColor(i, 0, 0, 0, datos[i].currentBrightness);
  }

  if (letsUpdate) {
    tira1.show();   
  }

}
