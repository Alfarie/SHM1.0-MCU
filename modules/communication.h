#include <Task.h>
extern TaskManager taskManager;

class Communication : public Task
{
  public:
    Communication(Sensor *sensor, ECControl *ec, ECTimerControl *ectimer, PHControl *ph, CO2Control *co2, TimerControl *timer, DateTimeTask *dt) : Task(MsToTaskTime(100))
    {
        _sensor = sensor;
        _eccontrol = ec;
        _ectimercontrol = ectimer;
        _phcontrol = ph;
        _co2control = co2;
        _datetime = dt;
        _timercontrol = timer;
    };

  private:
    String res;
    ECControl *_eccontrol;
    ECTimerControl *_ectimercontrol;
    PHControl *_phcontrol;
    CO2Control *_co2control;
    Sensor *_sensor;
    TimerControl *_timercontrol;
    DateTimeTask *_datetime;

    virtual bool OnStart()
    {
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        ReadFromMPU();
    }
    void ReadFromMPU()
    {
        String res;
        bool flag = false;
        while (Serial.available())
        {
            if (Serial.read() == '{')
            {
                int r = 0;
                while (true)
                {
                    if (Serial.available())
                    {
                        char ch = Serial.read();
                        if (ch == '}')
                        {
                            Serial.println(res);
                            flag = true;
                            break;
                        }
                        res += ch;
                    }
                }
            }

            if (flag)
            {
                //checking header before push into mcuQ
                flag = false;
                // {sol,1.2,5,10,6.5,5,10}
                if (res.startsWith("sol"))
                {
                    res.replace("sol,", "");
                    res.trim();
                    float sol[6] = {0, 0, 0, 0, 0, 0};
                    ExtractDataFloat(sol, 6, res);

                    GlobalControl::EC_SETPOINT = sol[0];
                    GlobalControl::EC_WORKING = sol[1];
                    GlobalControl::EC_DETECTING = sol[2];

                    GlobalControl::PH_SETPOINT = sol[3];
                    GlobalControl::PH_WORKING = sol[4];
                    GlobalControl::PH_DETECTING = sol[5];

                    _eccontrol->SetTiming(GlobalControl::EC_WORKING, GlobalControl::EC_DETECTING);
                    _eccontrol->SetEC(GlobalControl::EC_SETPOINT);
                    GlobalControl::UpdateSolEEPROM();
                    String jsonStr = "{\"type\": \"set_setting_success\",\"data\": \"Setting successful.\"}";
                    Serial.println(jsonStr);
                }
                // {cal, 0.81, 1.33}
                if (res.startsWith("cal"))
                {
                    res.replace("cal,", "");

                    res.trim();
                    float cal[2] = {0, 0};
                    ExtractDataFloat(cal, 2, res);
                    GlobalControl::EC_CAL = cal[0];
                    GlobalControl::PH_CAL = cal[1];

                    Serial.println(GlobalControl::EC_CAL);
                    Serial.println(GlobalControl::PH_CAL);

                    GlobalControl::UpdateCalEEPROM();
                }
                //{gas,1600}
                if (res.startsWith("gas"))
                {
                    res.replace("gas,", "");
                    res.trim();
                    float gas[2] = {0, 0};
                    ExtractDataFloat(gas, 2, res);
                    GlobalControl::CO2_SETPOINT = gas[0];
                    _co2control->SetCO2((int)gas[0]);
                    GlobalControl::UpdateGasEEPROM();
                }
                //{datetime,day,month,year,hour,min}
                //{datetime,20,9,17,12,0}
                if (res.startsWith("datetime"))
                {
                    res.replace("datetime,", "");
                    res.trim();
                    int dt[5] = {0, 0, 0, 0, 0};
                    ExtractDataInt(dt, 5, res);
                    for (int i = 0; i < 5; i++)
                    {
                        Serial.print(String(dt[i]) + " ");
                    }
                    Serial.println();
                    // (byte s, byte m, byte h, byte dow, byte dom, byte mo, byte y)
                    _datetime->setDateDs1307(0, dt[4], dt[3], 0, dt[0], dt[1], dt[2]);
                    Serial.println("[Info] Set date time successfull");

                    String jsonStr = "{\"type\": \"set_setting_success\",\"data\": \"Setting successful.\"}";
                    Serial.println(jsonStr);
                }

                if (res.startsWith("ectimer"))
                {
                    res.replace("ectimer,", "");
                    res.trim();
                    int dt[30];
                    int size = 0;
                    us_ExtractDataInt(dt, size, res);

                    std::vector<int> timer_list;
                    for (int i = 1; i < size; i++)
                    {
                        Serial.print(String(dt[i]) + " ");
                        timer_list.push_back(dt[i]);
                    }
                    Serial.println();

                    GlobalControl::UpdateECTimer(timer_list, dt[0]);
                    Serial.println("[Info] Set ec timer successfull");

                    String jsonStr = "{\"type\": \"set_setting_success\",\"data\": \"Setting successful.\"}";
                    Serial.println(jsonStr);
                }
                if (res.startsWith("ecmode"))
                {
                    res.replace("ecmode,", "");
                    res.trim();
                    int dt[2];
                    ExtractDataInt(dt, 1, res);
                    GlobalControl::UpdateECMode(dt[0]);
                    Serial.println("[Info] Set ec mode successfull :" + String(dt[0]));

                    if (dt[0] == 0)
                    {
                        digitalWrite(Gpio::EC_A_PUMP, OFF);
                        taskManager.StopTask(_ectimercontrol);
                        taskManager.StartTask(_eccontrol);
                        
                    }
                    else if (dt[0] == 1)
                    {
                        digitalWrite(Gpio::EC_A_PUMP, OFF);
                        taskManager.StopTask(_eccontrol);
                        taskManager.StartTask(_ectimercontrol);
                    }
                    else if (dt[0] == 2)
                    {
                        digitalWrite(Gpio::EC_A_PUMP, OFF);
                        taskManager.StopTask(_ectimercontrol);
                        taskManager.StopTask(_eccontrol);
                    }
                }

                //{timer,20-60,90-150,200-260}
                if (res.startsWith("timer"))
                {
                    res.replace("timer,", "");
                    res.trim();
                    String timer[30];
                    int size = 0;
                    ExtractDataString(timer, size, res);
                    std::vector<Timer> timerlist = TimerStringToTimer(timer, size);
                    _timercontrol->SetTimerList(timerlist);
                    GlobalControl::UpdateTimer(timerlist);
                    Serial.println("[Info] Set timer successfull");
                }

                if (res.startsWith("cmd"))
                {
                    res.replace("cmd,", "");
                    res.trim();
                    String d[2];
                    int size = 0;
                    ExtractDataString(d, size, res);
                    if (d[0] == "close-all")
                    {

                        Serial.println("[Done] Close All pin: successful");
                    }
                    else if (d[0] == "open-all")
                    {

                        Serial.println("[Done] Open All pin: successful");
                    }
                    else if (d[0] == "show-setting")
                    {
                        GlobalControl::ShowSetting();
                    }
                    else if (d[0] == "req_setting")
                    {
                        String arrStr = "";
                        for(int i = 0; i < GlobalControl::EC_TIMER_LIST.size(); i++){
                            arrStr += String(GlobalControl::EC_TIMER_LIST.at(i));
                            if(i != GlobalControl::EC_TIMER_LIST.size() -1){
                                arrStr += ",";
                            }
                        }
                        String jsonStr = "{\"ec-setpoint\": " + String(GlobalControl::EC_SETPOINT) +
                                         ",\"ec-working\": " + String(GlobalControl::EC_WORKING) +
                                         ",\"ec-detecting\": " + String(GlobalControl::EC_DETECTING) +
                                         ",\"ec-mode\": " + String(GlobalControl::EC_MODE) +
                                         ",\"ect-working\": " + String(GlobalControl::EC_TIMER_WORKING) +
                                         ",\"ect-list\": [" + arrStr + "]" + 
                                         ",\"ec-cal\": " + String(GlobalControl::EC_CAL)  + 
                                         ",\"ph-cal\": " + String(GlobalControl::PH_CAL) + 
                                         "}";
                        String data = "{\"type\": \"req_setting\",\"data\": " + jsonStr + "}";
                        Serial.println(data);
                    }
                }
                //{debug,true}
                if (res.startsWith("debug"))
                {
                    res.replace("debug,", "");
                    res.trim();
                    String d[2];
                    int size = 0;
                    ExtractDataString(d, size, res);
                    if (d[0] == "true")
                    {
                        DEBUG = true;
                        Serial.println("[Done] Debug mode: enable");
                    }
                    else if (d[0] == "false")
                    {
                        DEBUG = false;
                        Serial.println("[Done] debug mode: disable");
                    }
                }
            }
        }
    }

    std::vector<Timer> TimerStringToTimer(String *timer, int size)
    {
        std::vector<Timer> vtimer;
        for (int i = 0; i < size; i++)
        {
            int index = timer[i].indexOf("-");
            String startStr = timer[i].substring(0, index);
            String endStr = timer[i].substring(index + 1);

            int start = startStr.toInt();
            int end = endStr.toInt();
            Timer t = {start, end};
            vtimer.push_back(t);
        }
        return vtimer;
    }
    /**
        Extract string format seperated by comma(,) to array of float
        @param 
    */
    void ExtractDataFloat(float *data, int size, String res)
    {
        int i = 0, si = 0, ei, j = 0;
        while (j < size)
        {
            int index = res.indexOf(",");
            String a = res.substring(0, index);
            data[j] = a.toFloat();

            si = index;
            res = res.substring(index + 1);
            j++;
        }
    }

    void ExtractDataInt(int *data, int size, String res)
    {
        int i = 0, si = 0, ei, j = 0;
        while (j < size)
        {
            int index = res.indexOf(",");
            String a = res.substring(0, index);
            data[j] = a.toInt();

            si = index;
            res = res.substring(index + 1);
            j++;
        }
    }

    void us_ExtractDataInt(int *data, int &size, String res)
    {
        int i = 0, si = 0, ei, j = 0;
        while (j < 30)
        {
            int index = res.indexOf(",");
            String a = res.substring(0, index);
            data[j] = a.toInt();

            si = index;
            res = res.substring(index + 1);
            j++;
            if (index == -1)
            {
                size = j;
                return;
            }
        }
    }

    void ExtractDataString(String *data, int &size, String res)
    {
        int i = 0, si = 0, ei, j = 0;
        while (j < 20)
        {
            int index = res.indexOf(",");

            String a = res.substring(0, index);
            data[j] = a;
            // Serial.println(data[j]);
            si = index;
            res = res.substring(index + 1);
            j++;
            if (index == -1)
            {
                size = j;
                return;
            }
        }
    }
};