#include <Task.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <SPI.h>
#include <MCP320X.h>
#include <LibHumidity.h>

LibHumidity humidity = LibHumidity(0);

MCP320X adc(53);

LiquidCrystal_I2C lcd(0x3E, 16, 2);

struct Timer
{
    int start;
    int end;
};

TaskManager taskManager;
boolean DEBUG = false;
int ON = HIGH;
int OFF = LOW;
#include "./global/gpio.h"
#include "./global/eeprom_addr.h"

#include "./global/global_control.h"
#include "./modules/mode/debug.h"

#include "./modules/DateTime.h"
DateTimeTask dt;

void FreshWaterCallback(uint8_t);
void CookCallback(uint8_t);
void CirculateCallback(uint8_t);

#include "./modules/Sensor.h"
Sensor sensor;

#include "./modules/control_process/co2Control.h"
#include "./modules/control_process/eccontrol.h"
#include "./modules/control_process/ectimercontrol.h"
#include "./modules/control_process/phcontrol.h"
#include "./modules/control_process/timercontrol.h"


#include "./modules/communication.h"

PHControl phcontrol(&sensor);
ECControl eccontrol(&sensor);
CO2Control co2control(&sensor);
TimerControl timercontrol(&dt);
ECTimerControl ectimercontrol(&dt);

#include "./modules/wt_process/FreshWater.h"
#include "./modules/wt_process/Cook.h"
#include "./modules/wt_process/CirculateWater.h"
FreshWater freshWater(FreshWaterCallback);
Cook cook(CookCallback, &eccontrol, &phcontrol);
CirculateWater circulateWater(CirculateCallback);

Communication com(&sensor, &eccontrol,&ectimercontrol, &phcontrol, &co2control, &timercontrol, &dt);

DebugCheck debug;

void setup()
{
    SPI.begin();
    adc.begin();
    Wire.begin();
    
    lcd.begin();
    lcd.print("PlantLab");

    Serial.begin(57600);
    Serial3.begin(57600);

    SerialOnStart();
    // Initialize EEPROM
    GlobalControl::InitEEPROM();

    // set pin mode to all pin
    Gpio::InitPin();

    taskManager.StartTask(&dt);
    taskManager.StartTask(&com);
    //start Water process

    taskManager.StartTask(&sensor);

    //control module
    if(GlobalControl::EC_MODE == 0){
        taskManager.StartTask(&eccontrol);
    }
    else if (GlobalControl::EC_MODE == 1){
        taskManager.StartTask(&ectimercontrol);
    }
}

void loop()
{
    taskManager.Loop();
}

void ProcessStart()
{
    taskManager.StartTask(&freshWater);
    lcd.setCursor(0, 0);
    lcd.print("State: FreshWater");
}

void FreshWaterCallback(uint8_t state)
{
    //Serial.println("FreshWater Callback" + String(state));
    taskManager.StopTask(&freshWater);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("State: Cook");
    taskManager.StartTask(&cook);
}

void CookCallback(uint8_t state)
{
    //Serial.println("Cook Callback" + String(state));
    taskManager.StopTask(&cook);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("State: Circulate");

    taskManager.StartTask(&circulateWater);
}

void CirculateCallback(uint8_t state)
{
    //Serial.println("FeedWater Callback" + String(state));
    taskManager.StopTask(&circulateWater);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("State: FreshWater");

    taskManager.StartTask(&freshWater);
}

void SerialOnStart()
{
    String debugMode = (DEBUG) ? "true" : "false";
    Serial.println();
    Serial.println("[Info] PlantLab Demo Version.");
    Serial.println("[Info] Initialize...");
    Serial.println("[Info] Debug Mode: " + debugMode);
    Serial.println("[Info] successful.");
    Serial.println();
}