#include <Task.h>
#include <StandardCplusplus.h>
#include <vector>

extern TaskManager taskManager;



class TimerControl : public Task
{
  public:
    TimerControl(DateTimeTask *dt) : Task(MsToTaskTime(1000))
    {
        _datetime = dt;
    };

    void SetTimerList(std::vector<Timer> timerlist){
        timerList = timerlist;
    }

  private:
    std::vector<Timer> timerList;
    DateTimeTask *_datetime;
    virtual bool OnStart()
    {
        timerList = GlobalControl::GetTimer();
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        int currentMin = _datetime->GetCurrentMin();
        int flag = false;
        for (int i = 0; i < timerList.size(); i++)
        {
            // PrintTimer(timerList.at(i));
            int startMin = timerList.at(i).start;
            int endMin = timerList.at(i).end;
            if(startMin <= currentMin && endMin >= currentMin){
                flag = true;
                break;
            }
        }
        if(flag){
            if(DEBUG) Serial.println("LED ON");
            digitalWrite(Gpio::LED, ON);
        }
        else {
            if(DEBUG) Serial.println("LED OFF");
            digitalWrite(Gpio::LED, OFF);
        }
    }
    void PrintTimer(Timer time)
    {
        Serial.println("time:" + String(time.start) + "," + String(time.end) + " currentMin:" + String(_datetime->GetCurrentMin()) );
    }
};