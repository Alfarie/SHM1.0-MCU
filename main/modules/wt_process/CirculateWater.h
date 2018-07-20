#include <Task.h>
extern TaskManager taskManager;

class CirculateWater : public Task
{
  public:
    typedef void (*callback)(uint8_t);
    CirculateWater(callback function) : Task(MsToTaskTime(1000)),
                                       _callback(function)
                                        {
                                            rtime = 10000;
                                        };
  private:
    uint32_t _timer;
    long rtime;
    callback _callback;
    virtual bool OnStart()
    {
        analogWrite(Gpio::FEED_WATER_MOTOR, 200);
        _timer = 0;
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        //Serial.println("Circulate Water");
        _timer += delta_time;
        lcd.setCursor(0,1);
        String str = "Timer: " + String(_timer/1000) +"/" + String(rtime/1000);

        String jsonStr = "{\"type\": \"water\",\"data\": { \"time\":" + String(_timer) + ", \"total_time\": "+ String(rtime) + ", \"process\": \"Circulating\"} }";
        Serial.println(jsonStr);

        lcd.print(str);
        if (_timer >= rtime)
        {
            digitalWrite(Gpio::FEED_WATER_MOTOR, LOW);
            _callback(1);
            _timer = 0;
        }
    }
};

// {
//     'type': 'water',
//     'data': {
//         'time': 50000,
//         'total_time': 90000,
//         'process': "Circulating"
//     }
// }