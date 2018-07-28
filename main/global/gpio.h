class Gpio
{
    //ec = 4;
    //ph = 5;
    //water pump = 6;
    //co2 = 7;
    //light = 8;
    // fresh water = 9;
  public:
    // sensor
    static const uint16_t SAFETY_FLOATING_SENSOR = A10;

    //AC
    static const uint16_t PH_SENSOR = A1;
    static const uint16_t EC_SENSOR = A0;
    static const uint16_t WATER_SENSOR = A2;

    // actuator
    static const uint16_t FEED_WATER_MOTOR = 6;
    static const uint16_t FRESH_WATER_VALVE = 9;

    static const uint16_t EC_A_PUMP = 8;
    static const uint16_t EC_B_PUMP = 8;
    static const uint16_t PH_PUMP = 9;

    static const uint16_t CO2_VALVE = 7;

    static const uint16_t DEBUG_PIN = 22;

    static const uint16_t LED = 8;

    static const uint16_t DOOR_BUTTON = 23;

   

    static void InitPin()
    {
        //sensor
        pinMode(SAFETY_FLOATING_SENSOR, INPUT);

        pinMode(DEBUG_PIN, INPUT);
        pinMode(FEED_WATER_MOTOR, OUTPUT);
        pinMode(FRESH_WATER_VALVE, OUTPUT);
        pinMode(WATER_SENSOR, INPUT);
        pinMode(EC_A_PUMP, OUTPUT);
        pinMode(EC_B_PUMP, OUTPUT);
        pinMode(PH_PUMP, OUTPUT);
        pinMode(CO2_VALVE, OUTPUT);
        pinMode(DOOR_BUTTON, INPUT);
        pinMode(LED, OUTPUT);
        
        digitalWrite(FEED_WATER_MOTOR, OFF_S);
        digitalWrite(FRESH_WATER_VALVE, OFF_S);
        digitalWrite(EC_A_PUMP, OFF_S);
        digitalWrite(EC_B_PUMP, OFF_S);
        digitalWrite(PH_PUMP, OFF_S);
        digitalWrite(CO2_VALVE, OFF_S);
        digitalWrite(LED, OFF_S);
    }

    static void AllON()
    {
        digitalWrite(FEED_WATER_MOTOR, ON_S);
        digitalWrite(FRESH_WATER_VALVE, ON_S);
        digitalWrite(EC_A_PUMP, ON_S);
        digitalWrite(EC_B_PUMP, ON_S);
        digitalWrite(PH_PUMP, ON_S);
        digitalWrite(CO2_VALVE, ON_S);
    }
};
