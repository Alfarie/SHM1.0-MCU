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
            float adc = 0;
            // qsort()
            qsort(analogReadVal, readIndex, sizeof(analogReadVal[0]), sort_desc);
            int num = 0;
            for(int i = 2; i< readIndex-2; i++){
                adc += analogReadVal[i];
                num++;
            }
            
            //hint: adc 0-1023
            adc = adc / num;
            _ec = 0.0023 * adc - 0.4286;
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