#include <Task.h>
extern TaskManager taskManager;

#include <K30_I2C.h>
K30_I2C k30_i2c = K30_I2C(0x34);

class CO2 : public Task
{
  public:
    CO2() : Task(MsToTaskTime(1000)){

            };

    uint32_t GetCO2()
    {
        return _ppmCO2;
    }

  private:
    int _ppmCO2, tempco2;
    int rc;
    int valMultiplier = 1;
    virtual bool OnStart()
    {
        Serial.println("[PlantLab] CO2 sensor Initialize...");
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        rc = k30_i2c.readCO2(_ppmCO2);
        if (rc == 0)
        {
            tempco2 = _ppmCO2;
        }
        else
        {
            _ppmCO2 = tempco2;
            Serial.print("Failure to read sensor\n");
        }
    }
};