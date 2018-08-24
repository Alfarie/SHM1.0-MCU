#include<Task.h>


extern TaskManager taskManager;

class ECSensor: public Task{
   public:
       ECSensor():
           Task(MsToTaskTime(100))
       { };

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
            
            _temp = = water.GetWater();

            avgVoltage = avgVoltage / num;
            avgVoltage = avgVoltage * 5000 / 1024;
            float TempCoefficient=1.0+0.0185*(_temp-25.0);
            float CoefficientVolatge=(float)avgVoltage/TempCoefficient;
            
            if(CoefficientVolatge<=448) _ec= 6.84*CoefficientVolatge-64.32;   //1ms/cm<EC<=3ms/cm
            else if(CoefficientVolatge<=1457) _ec= 6.98*CoefficientVolatge-127;  //3ms/cm<EC<=10ms/cm
            else _ec=5.3*CoefficientVolatge+2278;    
            _ec = _ec/1000;
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