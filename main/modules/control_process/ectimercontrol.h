#include <Task.h>
#include <StandardCplusplus.h>
#include <vector>

extern TaskManager taskManager;

class ECTimerControl : public Task
{
  public:
    ECTimerControl(DateTimeTask *dt) : Task(MsToTaskTime(1000))
    {
        _datetime = dt;
    };
    void SetTimerList(std::vector<int> timer_list)
    {
        timerList = timer_list;
    }

  private:
    std::vector<int> timerList;
    DateTimeTask *_datetime;
    int state = 0; //0: check state, 1: working state;
    float working = 0;
    double _timer = 0;
    int CurrentMinMatch = 0;
    virtual bool OnStart()
    {
        mpuCom.println("[Info] EC Timer control Start..");
        timerList = GlobalControl::GetECTimer();
        working = GlobalControl::EC_TIMER_WORKING;
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        int currentMin = _datetime->GetCurrentMin();
        if (state == 0)
        {
            int flag = false;
            for (int i = 0; i < GlobalControl::EC_TIMER_LIST.size(); i++)
            {
                // PrintTimer(timerList.at(i));
                int timerMin = GlobalControl::EC_TIMER_LIST.at(i);
                // mpuCom.println(String(timerMin));
                if (timerMin == currentMin)
                {
                    CurrentMinMatch = timerMin;
                    flag = true;
                    break;
                }
            }
            if (flag)
            {
                if (DEBUG)
                    mpuCom.println("LED ON_S");
                digitalWrite(Gpio::EC_A_PUMP, ON_S);
                state = 1;
            }
            String jsonStr = "{\"type\": \"ectimer-control\",\"data\": { \"time\":" + String(currentMin) + ", \"endtime\": " + String(currentMin) + ", \"process\": \"Waiting\"} }";
            mpuCom.println(jsonStr);
        }
        else if (state == 1)
        {
            _timer += ( (float)delta_time/1000);
            String jsonStr = "{\"type\": \"ectimer-control\",\"data\": { \"time\":" + String(currentMin) + ", \"endtime\": " + String(working) + ", \"process\": \"Working\"} }";
            mpuCom.println(jsonStr);
           
            // mpuCom.println( "ectimer:" + String(_timer) + "/" + String(GlobalControl::EC_TIMER_WORKING));
            if (_timer >= GlobalControl::EC_TIMER_WORKING)
            {
                digitalWrite(Gpio::EC_A_PUMP, OFF_S);
                _timer = 0;
                state = 2;
            }
            // if( currentMin != CurrentMinMatch){
            //     state = 0;
            // }
        }
        if(state == 2 && currentMin != CurrentMinMatch){
            String jsonStr = "{\"type\": \"ectimer-control\",\"data\": { \"time\":" + String(currentMin) + ", \"endtime\": " + String(working) + ", \"process\": \"Working\"} }";
            mpuCom.println(jsonStr);
            state = 0;
        }
    }
};