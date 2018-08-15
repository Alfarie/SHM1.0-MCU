#include<Task.h>
extern TaskManager taskManager;
#include "DFRobot_EC.h"
DFRobot_EC ecv2;

class ECSensor: public Task{
   public:
       ECSensor():
           Task(MsToTaskTime(100))
       { 
           ecv2.begin();
       };

       float GetEC(){
           return _ec;
       }

   private:
    float _ec;
    float _temp = 28;
    int analogReadVal[30];
    int readIndex = 0;
    
    virtual bool OnStart()
    {

    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        int val = analogRead(Gpio::EC_SENSOR);
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
            avgVoltage = avgVoltage * 5000 / 1024;
            _ec = ecv2.readEC(avgVoltage,_temp);
            _ec = _ec/GlobalControl::EC_CAL;
            if(DEBUG) mpuCom.println("EC: " + String(_ec) + ", cal:" + GlobalControl::EC_CAL);
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