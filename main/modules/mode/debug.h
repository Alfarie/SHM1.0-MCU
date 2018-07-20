#include <Task.h>
extern TaskManager taskManager;

class DebugCheck : public Task
{
  public:
    DebugCheck() : Task(MsToTaskTime(1000)),
              _pin(Gpio::DEBUG_PIN){

              };

  private:
    uint16_t _pin;

    virtual bool OnStart()
    {
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        if (digitalRead(_pin) == HIGH)
        {
            DEBUG = true;
        }
        else{
            DEBUG = false;
        }
    }
};