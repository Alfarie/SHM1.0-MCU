#include <Task.h>
#include <Wire.h>
extern TaskManager taskManager;

#define ADDRESS 0x68

struct DateTime
{
    byte day;
    byte dayOfWeek;
    byte month;
    byte year;
    byte second;
    byte minute;
    byte hour;
};

class DateTimeTask : public Task
{
  public:
    DateTimeTask() : Task(MsToTaskTime(1000)){};

    void SetDateTime(DateTime dt)
    {
        Wire.beginTransmission(ADDRESS);
        Wire.write(decToBcd(0));
        Wire.write(decToBcd(dt.second)); // 0 to bit 7 starts the clock
        Wire.write(decToBcd(dt.minute));
        Wire.write(decToBcd(dt.hour)); // If you want 12 hour am/pm you need to set
        // bit 6 (also need to change readDateDs1307)
        Wire.write(decToBcd(dt.dayOfWeek));
        Wire.write(decToBcd(dt.day));
        Wire.write(decToBcd(dt.month));
        Wire.write(decToBcd(dt.year));
        Wire.endTransmission();
    }


    int GetCurrentMin()
    {
       return _datetime.hour*60 + _datetime.minute;
         
    }
    DateTime GetDateTime()
    {
        return _datetime;
    }


    String GetDateString()
    {
        // 2014-08-20 15:30:00
        String day = (_datetime.day > 10)? String(_datetime.day): String(( "0"+String(_datetime.day) ));
        
        String str = "20" + String(_datetime.year) + "-" + String(_datetime.month) + "-" + day;
        return str;
    }

    String GetTimeString()
    {

        String hour = (_datetime.hour >= 10)? String(_datetime.hour): String(( "0"+String(_datetime.hour) ));
        String min = (_datetime.minute >= 10)? String(_datetime.minute): String(( "0"+String(_datetime.minute) ));
        String second = (_datetime.second >= 10)? String(_datetime.second): String(( "0"+String(_datetime.second) ));
        String str = hour + ":" + min + ":" + second;
        return str;
    }

    void ShowDateTime(DateTime dt)
    {
        // 12:00:00 12/12/17
        String str = "[PlantLab] Date Time: " + String(dt.hour) + ":" + String(dt.minute) + ":" + String(dt.second) + " " + String(dt.day) + "/" + String(dt.month) + "/" + String(dt.year);
        Serial.println(str);
    }

    void setDateDs1307(byte s, byte m, byte h, byte dow, byte dom, byte mo, byte y)
    {
        Wire.beginTransmission(ADDRESS);
        Wire.write(decToBcd(0));
        Wire.write(decToBcd(s)); // 0 to bit 7 starts the clock
        Wire.write(decToBcd(m));
        Wire.write(decToBcd(h)); // If you want 12 hour am/pm you need to set
        // bit 6 (also need to change readDateDs1307)
        Wire.write(decToBcd(dow));
        Wire.write(decToBcd(dom));
        Wire.write(decToBcd(mo));
        Wire.write(decToBcd(y));
        Wire.endTransmission();
    }

  private:
    DateTime _datetime;
    virtual bool OnStart()
    {
        //Serial.println("[PlantLab] DateTime Task Start...");
        // Wire.begin();
        //  setDateDs1307(0, 47, 19, 5, 22, 9, 17);
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        if (DEBUG)
        {
            ShowDateTime(_datetime);
        }
        Refresh();
    }

    void Refresh()
    {
        Wire.beginTransmission(ADDRESS);
        Wire.write(decToBcd(0));
        Wire.endTransmission();
        Wire.requestFrom(ADDRESS, 7);

        _datetime.second = bcdToDec(Wire.read() & 0x7f);
        _datetime.minute = bcdToDec(Wire.read());
        _datetime.hour = bcdToDec(Wire.read() & 0x3f); // Need to change this if 12 hour am/pm
        _datetime.dayOfWeek = bcdToDec(Wire.read());
        _datetime.day = bcdToDec(Wire.read());
        _datetime.month = bcdToDec(Wire.read());
        _datetime.year = bcdToDec(Wire.read());
    }

    

    byte decToBcd(byte val)
    {
        return ((val / 10 * 16) + (val % 10));
    }
    byte bcdToDec(byte val)
    {
        return ((val / 16 * 10) + (val % 16));
    }
};

/*
void setDateDs1307(byte s , byte m , byte h , byte dow , byte dom , byte mo , byte y)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(decToBcd(0));
  Wire.write(decToBcd(s)); // 0 to bit 7 starts the clock
  Wire.write(decToBcd(m));
  Wire.write(decToBcd(h)); // If you want 12 hour am/pm you need to set
  // bit 6 (also need to change readDateDs1307)
  Wire.write(decToBcd(dow));
  Wire.write(decToBcd(dom));
  Wire.write(decToBcd(mo));
  Wire.write(decToBcd(y));
  Wire.endTransmission();
}
void getDateDs1307()
{

  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(decToBcd(0));
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
  second = bcdToDec(Wire.read() & 0x7f);
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0x3f); // Need to change this if 12 hour am/pm
  dayOfWeek = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());

  //////Serial.println(String(hour) + ":"+String(minute)+":"+String(second));
}

byte decToBcd(byte val) {
  return ( (val / 10 * 16) + (val % 10) );
}
byte bcdToDec(byte val) {
  return ( (val / 16 * 10) + (val % 16) );
}

*/
