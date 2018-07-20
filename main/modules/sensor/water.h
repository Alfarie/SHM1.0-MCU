#include <Task.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define StartConvert 0
#define ReadTemperature 1
extern TaskManager taskManager;

OneWire ds(Gpio::WATER_SENSOR);
DallasTemperature sensors(&ds);
class Water : public Task
{
  public:
    Water() : Task(MsToTaskTime(500))
    {
        sensors.begin();
    };
    float GetWater()
    {
        return _water;
    }

  private:
    float _water;
    uint32_t _timer;
    byte data[12];
    byte addr[8];
    float TemperatureSum;
    bool isRequest = true;

    virtual bool OnStart()
    {
        //  sensors.requestTemperatures(); 
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
       _timer += delta_time;
       if(_timer >= 2000){
         _water = sensors.getTempCByIndex(0); 
         sensors.requestTemperatures(); 
         _timer = 0;
       }
       
    }
};