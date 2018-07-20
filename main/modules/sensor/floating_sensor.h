class FloatingSensorDetect : public Task
{
  public:
    typedef void(*callback)(uint8_t );
    FloatingSensorDetect(uint16_t pin,callback function) :
        Task(MsToTaskTime(1000)),
        _Pin(pin),
        _callback(function),
        _timer(0),
        _state(3)
    {};

    void setState(bool state){
        _state = state;
    }
    
  private:
    uint32_t _timer;
    uint8_t _Pin;
    callback _callback;
    uint8_t _state;

    virtual bool OnStart(){
        pinMode(_Pin, INPUT);
        //Serial.println("Start Floating Sensor");
    }

    virtual void OnUpdate(uint32_t deltaTime){
        // //Serial.println("Test");
        if(digitalRead(_Pin) != _state){
             _timer += deltaTime;
             if(_timer >= 5000){
                 _state = digitalRead(_Pin);
                 _callback(_state);
                 _timer = 0;
             }
             
             if(digitalRead(_Pin) == _state)
                _timer = 0;
        }
    }
};