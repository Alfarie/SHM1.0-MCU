#include <StandardCplusplus.h>
#include <vector>
class GlobalControl
{
  public:
    // sensor
    static float EC_SETPOINT;
    static float EC_DETECTING;
    static float EC_WORKING;
    static float EC_CAL;

    static float PH_SETPOINT;
    static float PH_DETECTING;
    static float PH_WORKING;
    static float PH_CAL;

    static float CO2_SETPOINT;

    static int TIMER_SIZE;
    static std::vector<Timer> TIMER_LIST;

    static std::vector<int> EC_TIMER_LIST;
    static int EC_TIMER_SIZE;
    static int EC_MODE;
    static int EC_TIMER_WORKING;

    static void InitEEPROM()
    {
        int isInit = 0;
        EEPROM.get(EEPROM_ADDR::ISINIT_EEPROM, isInit);
        if (isInit != 2)
        {
            EEPROM.put(EEPROM_ADDR::EC_SETPOINT, GlobalControl::EC_SETPOINT);
            EEPROM.put(EEPROM_ADDR::EC_DETECTING, GlobalControl::EC_DETECTING);
            EEPROM.put(EEPROM_ADDR::EC_WORKING, GlobalControl::EC_WORKING);
            EEPROM.put(EEPROM_ADDR::EC_CAL, GlobalControl::EC_CAL);

            EEPROM.put(EEPROM_ADDR::PH_SETPOINT, GlobalControl::PH_SETPOINT);
            EEPROM.put(EEPROM_ADDR::PH_DETECTING, GlobalControl::PH_DETECTING);
            EEPROM.put(EEPROM_ADDR::PH_WORKING, GlobalControl::PH_WORKING);
            EEPROM.put(EEPROM_ADDR::PH_CAL, GlobalControl::PH_CAL);
            EEPROM.put(EEPROM_ADDR::CO2_SETPOINT, GlobalControl::CO2_SETPOINT);

            EEPROM.put(EEPROM_ADDR::EC_MODE, GlobalControl::EC_MODE);
            EEPROM.put(EEPROM_ADDR::EC_TIMER_WORKING, GlobalControl::EC_TIMER_WORKING);

            GlobalControl::InitTimer();
            GlobalControl::InitECTimer();
            GlobalControl::GetECTimer();
            isInit = 2;
            EEPROM.put(EEPROM_ADDR::ISINIT_EEPROM, isInit);
            Serial.println("Init all setting...");
        }
        else if (isInit == 2)
        {
            EEPROM.get(EEPROM_ADDR::EC_SETPOINT, GlobalControl::EC_SETPOINT);
            EEPROM.get(EEPROM_ADDR::EC_DETECTING, GlobalControl::EC_DETECTING);
            EEPROM.get(EEPROM_ADDR::EC_WORKING, GlobalControl::EC_WORKING);
            EEPROM.get(EEPROM_ADDR::EC_CAL, GlobalControl::EC_CAL);

            EEPROM.get(EEPROM_ADDR::PH_SETPOINT, GlobalControl::PH_SETPOINT);
            EEPROM.get(EEPROM_ADDR::PH_DETECTING, GlobalControl::PH_DETECTING);
            EEPROM.get(EEPROM_ADDR::PH_WORKING, GlobalControl::PH_WORKING);
            EEPROM.get(EEPROM_ADDR::PH_CAL, GlobalControl::PH_CAL);
            EEPROM.get(EEPROM_ADDR::CO2_SETPOINT, GlobalControl::CO2_SETPOINT);

            EEPROM.get(EEPROM_ADDR::EC_MODE, GlobalControl::EC_MODE);
            EEPROM.get(EEPROM_ADDR::EC_TIMER_WORKING, GlobalControl::EC_TIMER_WORKING);
            GlobalControl::GetECTimer();
        }
        GlobalControl::ShowSetting();
    }
    static void ShowSetting()
    {
        Serial.println("[Info] EC Setpoint: " + String(GlobalControl::EC_SETPOINT));
        Serial.println("[Info] EC Detecting: " + String(GlobalControl::EC_DETECTING));
        Serial.println("[Info] EC Working: " + String(GlobalControl::EC_WORKING));
        Serial.println("[Info] EC Calibration: " + String(GlobalControl::EC_CAL));

        Serial.println("[Info] pH Setpoint: " + String(GlobalControl::PH_SETPOINT));
        Serial.println("[Info] pH Detecting: " + String(GlobalControl::PH_DETECTING));
        Serial.println("[Info] pH Working: " + String(GlobalControl::PH_WORKING));
        Serial.println("[Info] pH Calibration: " + String(GlobalControl::PH_CAL));

        Serial.println("[Info] CO2 Setpoint: " + String(GlobalControl::CO2_SETPOINT));

        Serial.println("[Info] EC working: " + String(GlobalControl::EC_TIMER_WORKING));
        Serial.print("[Info] EC Timer: ");
        for (int i = 0; i < GlobalControl::EC_TIMER_SIZE; i++)
        {
            Serial.print(GlobalControl::EC_TIMER_LIST.at(i));
            Serial.print(" ");
        }
        Serial.println();
        Serial.println("[Info] EC Timer Size: " + String(GlobalControl::EC_TIMER_SIZE));
        Serial.println("[Info] EC Timer Mode: " + String(GlobalControl::EC_MODE));
    }
    static void UpdateSolEEPROM()
    {
        EEPROM.put(EEPROM_ADDR::EC_SETPOINT, GlobalControl::EC_SETPOINT);
        EEPROM.put(EEPROM_ADDR::EC_DETECTING, GlobalControl::EC_DETECTING);
        EEPROM.put(EEPROM_ADDR::EC_WORKING, GlobalControl::EC_WORKING);

        EEPROM.put(EEPROM_ADDR::PH_SETPOINT, GlobalControl::PH_SETPOINT);
        EEPROM.put(EEPROM_ADDR::PH_DETECTING, GlobalControl::PH_DETECTING);
        EEPROM.put(EEPROM_ADDR::PH_WORKING, GlobalControl::PH_WORKING);
    }

    static void UpdateCalEEPROM()
    {
        EEPROM.put(EEPROM_ADDR::EC_CAL, GlobalControl::EC_CAL);
        EEPROM.put(EEPROM_ADDR::PH_CAL, GlobalControl::PH_CAL);
    }

    static void UpdateGasEEPROM()
    {
        EEPROM.put(EEPROM_ADDR::CO2_SETPOINT, GlobalControl::CO2_SETPOINT);
    }

    static void UpdateTimer(std::vector<Timer> timer_list)
    {
        for (int i = 0; i < timer_list.size(); i++)
        {
            EEPROM.put(EEPROM_ADDR::TIMER_ADDR_ST + (sizeof(Timer) * i), timer_list.at(i));
        }

        EEPROM.put(EEPROM_ADDR::TIMER_SIZE, timer_list.size());
    }

    static void InitTimer()
    {
        Timer timer[4];
        timer[0] = {300, 480};
        timer[1] = {540, 720};
        timer[2] = {1020, 1200};
        timer[3] = {1260, 1440};
        // {timer,300-400, 540-720, 1020-1200, 1260-1440}
        for (int i = 0; i < 4; i++)
        {
            EEPROM.put(EEPROM_ADDR::TIMER_ADDR_ST + (sizeof(Timer) * i), timer[i]);
        }

        EEPROM.put(EEPROM_ADDR::TIMER_SIZE, 4);
        EEPROM.put(EEPROM_ADDR::EC_TIMER_WORKING, GlobalControl::EC_TIMER_WORKING);
    }

    static void InitECTimer()
    {
        int timer[2];
        timer[0] = 360;
        timer[1] = 1080;
        for (int i = 0; i < 2; i++)
        {
            EEPROM.put(EEPROM_ADDR::EC_TIMER_ADDR_ST + (sizeof(int) * i), timer[i]);
        }
        GlobalControl::EC_TIMER_LIST.push_back(timer[0]);
        GlobalControl::EC_TIMER_LIST.push_back(timer[1]);
        EEPROM.put(EEPROM_ADDR::EC_TIMER_SIZE, 2);
    }

    static std::vector<int> GetECTimer()
    {
        EEPROM.get(EEPROM_ADDR::EC_TIMER_SIZE, GlobalControl::EC_TIMER_SIZE);
        int size = GlobalControl::EC_TIMER_SIZE;
        std::vector<int> timer_list;
        for (int i = 0; i < size; i++)
        {
            int timer = 0;
            EEPROM.get(EEPROM_ADDR::EC_TIMER_ADDR_ST + (sizeof(int) * i), timer);
            timer_list.push_back(timer);
            
        }
        GlobalControl::EC_TIMER_LIST = timer_list;
        return timer_list;
    }

    static void UpdateECTimer(std::vector<int> timer_list, int ec_working)
    {
        for (int i = 0; i < timer_list.size(); i++)
        {
            EEPROM.put(EEPROM_ADDR::EC_TIMER_ADDR_ST + (sizeof(int) * i), timer_list.at(i));
        }

        EEPROM.put(EEPROM_ADDR::EC_TIMER_SIZE, timer_list.size());
        EEPROM.put(EEPROM_ADDR::EC_TIMER_WORKING, ec_working);

        GlobalControl::EC_TIMER_LIST = timer_list;
        GlobalControl::EC_TIMER_SIZE = timer_list.size();
        GlobalControl::EC_TIMER_WORKING = ec_working;
    }

    static void UpdateECMode(int mode){
        EEPROM.put(EEPROM_ADDR::EC_MODE,mode);
        GlobalControl::EC_MODE = mode;
    }

    static std::vector<Timer> GetTimer()
    {
        EEPROM.get(EEPROM_ADDR::TIMER_SIZE, GlobalControl::TIMER_SIZE);
        int size = GlobalControl::TIMER_SIZE;
        Serial.println("size: " + String(size));
        std::vector<Timer> timer_list;
        for (int i = 0; i < size; i++)
        {
            Timer timer = {0, 0};
            EEPROM.get(EEPROM_ADDR::TIMER_ADDR_ST + (sizeof(Timer) * i), timer);
            timer_list.push_back(timer);
        }
        return timer_list;
    }
};
float GlobalControl::EC_SETPOINT = 1.5;
float GlobalControl::EC_DETECTING = 10;
float GlobalControl::EC_WORKING = 5;
float GlobalControl::EC_CAL = 0.81;

float GlobalControl::PH_SETPOINT = 6.5;
float GlobalControl::PH_DETECTING = 10;
float GlobalControl::PH_WORKING = 5;
float GlobalControl::PH_CAL = 1.33;

float GlobalControl::CO2_SETPOINT = 1500;
int GlobalControl::TIMER_SIZE = 0;

int GlobalControl::EC_TIMER_SIZE = 0;
int GlobalControl::EC_MODE = 0;
int GlobalControl::EC_TIMER_WORKING = 5;

std::vector<int> GlobalControl::EC_TIMER_LIST;