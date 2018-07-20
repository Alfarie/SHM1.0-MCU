#include <Task.h>



extern TaskManager taskManager;

class pHSensor : public Task
{
  public:
    pHSensor() : Task(MsToTaskTime(100)){};

    float GetpH(){
        return _ph;
    }

  private:
    float _ph;
    int analogReadVal[30];
    int readIndex = 0;
    virtual bool OnStart()
    {
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        int val = analogRead(Gpio::PH_SENSOR);
        analogReadVal[readIndex] = val;
        readIndex++;

        if(readIndex >= 20){
            float avgVoltage = 0;
            // qsort()
            qsort(analogReadVal, readIndex, sizeof(analogReadVal[0]), sort_desc);
            int num = 0;
            for(int i = 2; i< readIndex-2; i++){
                avgVoltage += analogReadVal[i];
                num++;
            }
            avgVoltage = avgVoltage / num;
            // Serial.println("pH:" + String(avgVoltage));
            _ph = avgVoltage * 5.0 / 1024.0;
            _ph = 3.5 * _ph;
            _ph = _ph/GlobalControl::PH_CAL;
            //Serial.println("[Info] [PlantLab] pH: " + String(_ph));
            lcd.setCursor(0,1);
            // lcd.print("pH:" + String(_ph));
            readIndex = 0;
        }
    }
    static int sort_desc(const void *cmp1, const void *cmp2)
    {
        // Need to cast the void * to int *
        int a = *((int *)cmp1);
        int b = *((int *)cmp2);
        // The comparison
        return a < b ? -1 : (a > b ? 1 : 0);
        // A simpler, probably faster way:
        //return b - a;
    }
};
