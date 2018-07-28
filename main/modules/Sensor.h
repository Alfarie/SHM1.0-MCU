#include <Task.h>

#include <SHT21.h> // include SHT21 library

extern TaskManager taskManager;
extern int  __bss_end;
extern int  *__brkval;

#include "./sensor/co2.h"
#include "./sensor/ph.h"
#include "./sensor/ec.h"
#include "./sensor/water.h"

ECSensor ec;
pHSensor ph;
Water water;
CO2 co2;
SHT21 sht;


class Sensor : public Task
{
  public:
    Sensor() : Task(MsToTaskTime(50)),
               _timer(0),
               _co2(0){};

    uint32_t GetCO2(){
        return _co2;
    }
    float GetEC(){
        return _ec;
    }
    float GetpH(){
        return _ph;
    }


  private:
    uint32_t _timer;
    int32_t _co2;
    float _temperature;
    float _humidity;
    float _ec;
    float _ph;
    float _water;

    virtual bool OnStart()
    {
        mpuCom.println("[PlantLab] Sensor module initialize..");
        
        taskManager.StartTask(&co2);
        taskManager.StartTask(&water);
        taskManager.StartTask(&ph);
        taskManager.StartTask(&ec);
    } 
    virtual void OnUpdate(uint32_t delta_time)
    {
        _timer += delta_time;
        if (_timer >= 1000)
        {
            
            // _humidity = sht.getHumidity();
            // _temperature = sht.getTemperature();

            _co2 = co2.GetCO2();
            _humidity = humidity.GetHumidity();
            _temperature = humidity.GetTemperatureC();

            _ec = ec.GetEC();
            _ph = ph.GetpH();
            _water = water.GetWater();

            // _co2 = 1120;
            // _humidity = 57.0;
            // _temperature = 23;
            // _ec = 1.56;
            // _ph = 6.5;
            showAllSensor();
            _timer = 0;
        }
    }

    void showAllSensor()
    {
        String sensorDataJsonStr = "{ \"co2\":" + String(_co2)+ ",\"ec\":" + String(_ec)   + ",\"water\":" + String(_water) + "," + " \"ph\":"+String(_ph) + "," + " \"temp\":" + String(_temperature)+ "," +
            " \"humi\":"+ String(_humidity) +  ", \"date\":" + "\"" + RTC::instance()->GetDateString() + "\"" +  ", \"time\":"  + "\"" + RTC::instance()->GetTimeString()  + "\"" +  "}";
        String jsonStr = "{\"type\": \"sensor\",\"data\": " + sensorDataJsonStr + "}" ;
        mpuCom.println(jsonStr);
    }
    
    int freemem()
    {
        int free_memory;
        if ((int)__brkval == 0)
            free_memory = ((int)&free_memory) - ((int)&__bss_end);
        else
            free_memory = ((int)&free_memory) - ((int)__brkval);
        return free_memory;
    }
};