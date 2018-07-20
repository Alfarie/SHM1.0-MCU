#include <Task.h>
#include <Wire.h>
extern TaskManager taskManager;
class CO2Control : public Task
{
public:
  CO2Control(Sensor *sensor) : Task(MsToTaskTime(1000)),
                               state(1),
                               _upper(1100),
                               _lower(900)
  {
    _sensor = sensor;
  };

  void SetBound(uint16_t upper, uint16_t OFFer)
  {
    _upper = upper;
    _lower = OFFer;
  }

  void SetCO2(int co2)
  {
    _upper = co2 + 200;
    _lower = co2 - 200;
  }

private:
  Sensor *_sensor;
  uint16_t _upper, _lower;
  uint8_t state = 1;
  virtual bool OnStart()
  {
    _upper = (int)GlobalControl::CO2_SETPOINT + 150;
    _lower = (int)GlobalControl::CO2_SETPOINT - 150;

    Serial.println("[Info] CO2 control Initializing...");
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    // //Serial.println("[Info] CO2Control update " + String(_sensor->GetCO2()));
    String str = "CO2: " + String(_sensor->GetCO2()) + " ppm";
    // Serial.println(str);
    lcd.setCursor(0, 0);
    lcd.print(str);
    bool OFFCon = _sensor->GetCO2() <= _lower;
    bool upCon = _sensor->GetCO2() <= _upper;
    if (state == 1)
    {
      if (OFFCon && upCon)
      {
        if (digitalRead(Gpio::DOOR_BUTTON) == LOW)
        {
          String jsonStr = "{\"type\": \"co2-control\",\"data\": { \"values\": \"" + String(_sensor->GetCO2()) + "/" + String(_upper) + "\", \"process\": \"Feed CO2\"} }";
          Serial.println(jsonStr);
          digitalWrite(Gpio::CO2_VALVE, ON);
        }
        state = 2;
      }
      else
      {
        String jsonStr = "{\"type\": \"co2-control\",\"data\": { \"values\": \"" + String(_sensor->GetCO2()) + "/" + String(_upper) + "\", \"process\": \"No Feed CO2\"} }";
        Serial.println(jsonStr);
        digitalWrite(Gpio::CO2_VALVE, OFF);
      }
    }
    else if (state == 2)
    {
      if (!upCon)
      {
         String jsonStr = "{\"type\": \"co2-control\",\"data\": { \"values\": \"" + String(_sensor->GetCO2()) + "/" + String(_upper) + "\", \"process\": \"No Feed CO2\"} }";
          Serial.println(jsonStr);
        digitalWrite(Gpio::CO2_VALVE, OFF);
        state = 1;
      }
    }
  }
};
