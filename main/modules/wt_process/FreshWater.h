#include <Task.h>
extern TaskManager taskManager;

#include "../sensor/floating_sensor.h"

class FreshWater : public Task
{
  public:
    typedef void (*callback)(uint8_t);
    FreshWater(callback function) : 
        Task(MsToTaskTime(1000)),
        _callback(function),
        _Pin(Gpio::SAFETY_FLOATING_SENSOR),
        _state(0)
    {};

  private:
    uint32_t _timer,waiting, _total_time;
    uint8_t _Pin;
    callback _callback;
    uint8_t _state;
    bool finish = false;


    virtual bool OnStart()
    {
        _timer = 0;
        waiting = 0;
        finish = false;
        _total_time = 0;
        pinMode(_Pin, INPUT);
        int state = digitalRead(_Pin);
        //Serial.println("Initial state: " + String(state));
        if(state == HIGH){
            finish = true;
        }
        else{
            digitalWrite(Gpio::FRESH_WATER_VALVE, ON);
        }
        
    }

    virtual void OnUpdate(uint32_t deltaTime)
    {
        _total_time += deltaTime;
        //Serial.println("Freshwater update");
        waiting += deltaTime;
        //Serial.println("Finish: " + String(finish));
        String jsonStr = "{\"type\": \"water\",\"data\": { \"time\":" + String(_total_time) + ", \"total_time\":0 , \"process\": \"Fill water\"} }";
        Serial.println(jsonStr);
        if(waiting >= 2000){
             if(finish){
                _callback(1);
                finish = false;
            }
            waiting = 0;
        }

        if (digitalRead(_Pin) != _state)
        {
            _timer += deltaTime;
            if (_timer >= 5000)
            {
                _state = digitalRead(_Pin);
                _callback(_state);
                _state = 0;
                _timer = 0;
                digitalWrite(Gpio::FRESH_WATER_VALVE, OFF);
            }

            if (digitalRead(_Pin) == _state)
                _timer = 0;
        }
    }
};