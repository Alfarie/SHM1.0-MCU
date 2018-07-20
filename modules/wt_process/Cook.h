#include <Task.h>
extern TaskManager taskManager;


// PHControl phcontrol(&sensor);
// ECControl eccontrol(&sensor);
class Cook : public Task
{
  public:
    typedef void (*callback)(uint8_t);
    Cook(callback function, ECControl *ec, PHControl *ph) : Task(MsToTaskTime(1000)),
             _timer(0),
             _callback(function)
             {
                _eccontrol = ec;
                _phcontrol = ph;
             };

  private:
    uint32_t _timer;
    uint32_t rtime = 10000;
    callback _callback;
    ECControl *_eccontrol;
    PHControl *_phcontrol;
    virtual bool OnStart()
    {
         _timer = 0;
        taskManager.StartTask(&phcontrol);
        taskManager.StartTask(&eccontrol);
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        //Serial.println("Cook update");

        
        _timer += delta_time;
        lcd.setCursor(0,1);
        String str = "Timer: " + String(_timer/1000) +"/" + String(rtime/1000);

        String jsonStr = "{\"type\": \"water\",\"data\": { \"time\":" + String(_timer) + ", \"total_time\": "+ String(rtime) + ", \"process\": \"Cooking\"} }";
        Serial.println(jsonStr);
        lcd.print(str);
        if (_timer >= rtime)
        {
            _callback(1);
        }
    }
};