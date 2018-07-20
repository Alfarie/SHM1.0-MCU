#include <Task.h>
#include <EEPROM.h>
extern TaskManager taskManager;

class ECControl : public Task
{
  public:
    ECControl(Sensor *sensor) : Task(MsToTaskTime(1000)),
                                _setpoint(1.4)
    {
        _sensor = sensor;
        //address , data
    };

    void SetEC(float ec)
    {
        _setpoint = ec;
        // //Serial.println("EC: " + String(_upper) + "," + String(_lower));
    }

    void SetTiming(int working, int detecting)
    {
        _working = (float)working * 1000;
        _detecting = (float)detecting * 1000;
    }

  private:
    Sensor *_sensor;
    float _setpoint;
    float _working, _detecting;
    uint16_t state = 1;
    uint32_t _timer = 0;
    bool working_state = false, detecting_state = true;
    virtual bool OnStart()
    {
        // EEPROM.get(EEPROM_ADDR::EC_SETPOINT,_setpoint);
        // EEPROM.get(EEPROM_ADDR::EC_WORKING, _working);
        // EEPROM.get(EEPROM_ADDR::EC_DETECTING, _detecting);
        _setpoint = GlobalControl::EC_SETPOINT;
        _working = GlobalControl::EC_WORKING * 1000;
        _detecting = GlobalControl::EC_DETECTING * 1000;
        Serial.println("[Info] EC Control initialize...");
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        float sensor = _sensor->GetEC();

        // detecting mode
        if (working_state && (sensor < _setpoint))
        {
            // Serial.println("working state");'
            _timer += (float)delta_time/1000;
            // Serial.println("EC working " + String(_timer) + "/" + String(_working));
            analogWrite(Gpio::EC_A_PUMP, 255);
            String jsonStr = "{\"type\": \"ec-control\",\"data\": { \"time\":" + String(_timer*1000) + ", \"endtime\": " + String(_working) + ", \"process\": \"Working\"} }";
            Serial.println(jsonStr);
            if (_timer >= GlobalControl::EC_WORKING)
            {
                digitalWrite(Gpio::EC_A_PUMP, LOW);
                _timer = 0;
                detecting_state = true;
                working_state = false;
            }
        }
        else
        {
            digitalWrite(Gpio::EC_A_PUMP, LOW);
            detecting_state = true;
            working_state = false;
        }
        if (detecting_state)
        {
            _timer += (float)delta_time/1000;
            // Serial.println("detecting state");
            // Serial.println("EC detecting " + String(_timer) + "/" + String(_detecting));
            String jsonStr = "{\"type\": \"ec-control\",\"data\": { \"time\":" + String(_timer*1000) + ", \"endtime\": " + String(_detecting) + ", \"process\": \"Waiting\"} }";
            Serial.println(jsonStr);
            if (_timer >= GlobalControl::EC_DETECTING)
            {
                _timer = 0;
                detecting_state = false;
                working_state = true;
            }
        }
    }
};

// detecting mode
//   if (working_state && (sensor < _setpoint))
//         {
//             // Serial.println("working state");
//             analogWrite(Gpio::EC_A_PUMP, 255);
//             if (_timer >= _working)
//             {
//                 digitalWrite(Gpio::EC_A_PUMP, LOW);
//                 _timer = 0;
//                 detecting_state = true;
//                 working_state = false;
//             }
//         }
//         else
//         {
//             detecting_state = true;
//             working_state = false;
//         }

//         if (detecting_state)
//         {
//             // Serial.println("detecting state");
//             if (_timer >= _detecting)
//             {
//                 _timer = 0;
//                 detecting_state = false;
//                 working_state = true;
//             }
//         }
