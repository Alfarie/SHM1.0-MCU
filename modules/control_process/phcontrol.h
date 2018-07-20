#include <Task.h>
extern TaskManager taskManager;

class PHControl : public Task
{
  public:
    PHControl(Sensor *sensor) : Task(MsToTaskTime(1000)),
        _setpoint(6.5)
    {
        _sensor = sensor;
    };

    void SetPH(float ph)
    {
        _setpoint = ph;
    }

    void SetTiming(int working, int detecting)
    {
        _working = working * 1000;
        _detecting = detecting * 1000;
    }

  private:
    Sensor *_sensor;
    float _setpoint;
    float _ph = 0;
    int _working,_detecting;
    uint16_t state = 1;
    uint32_t _timer = 0;
    bool working_state = false;
    virtual bool OnStart()
    {
         _setpoint = GlobalControl::PH_SETPOINT;
        _working = GlobalControl::PH_WORKING * 1000;
        _detecting = GlobalControl::PH_DETECTING * 1000;
         Serial.println("[Info] pH Control initialize...");
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        // //Serial.println(String(_sensor->GetEC()));
        float sensor = _sensor->GetpH();
        String co2interval = String(sensor) + "/" + String(_setpoint);
        if (sensor > _setpoint)
        {
            working_state = true;
        }
        else
        {
            Serial.println("ec Stop Task no feed");
            String jsonStr = "{\"type\": \"pH-control\",\"data\": { \"time\":" + String(_timer) + ", \"working\": " + String(_working) + ", \"process\": \"No Feed "+co2interval+"\"} }";
            Serial.println(jsonStr);

            taskManager.StopTask(this);
            
        }

        if (working_state)
        {
            analogWrite(Gpio::PH_PUMP, 200);
            _timer += delta_time;
            //
            
            String jsonStr = "{\"type\": \"pH-control\",\"data\": { \"time\":" + String(_timer) + ", \"working\": " + String(_working) + ", \"process\": \"Feeding " + co2interval + " \"} }";

            Serial.println(jsonStr);
            if (_timer >= _working)
            {
                digitalWrite(Gpio::PH_PUMP, LOW);
                _timer = 0;
                working_state = false;

                String jsonStr = "{\"type\": \"pH-control\",\"data\": { \"time\":" + String(_timer) + ", \"working\": " + String(_working) + ", \"process\": \"No Feed "+co2interval+"\"} }";
                Serial.println(jsonStr);

                taskManager.StopTask(this);
            }
        }
    }
};

// float value = _sensor->GetpH();
//         if (value <= 0)
//             return;
//         bool lowerCon = value >= _lower;
//         bool upCon = value >= _upper;
//         if (state == 1)
//         {
//             if (lowerCon && upCon)
//             {
//                 analogWrite(Gpio::PH_PUMP, 255);
//                 // //Serial.println("GPIO ON");
//                 state = 2;
//             }
//             else
//             {
//                 // //Serial.println("GPIO OFF");
//                 analogWrite(Gpio::PH_PUMP, 0);
//             }
//         }
//         else if (state == 2)
//         {
//             if (!upCon)
//             {
//                 // //Serial.println("GPIO ON");
//                 analogWrite(Gpio::PH_PUMP, 0);
//                 state = 1;
//             }
//         }