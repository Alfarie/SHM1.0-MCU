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
    static const uint16_t PH_SENSOR = A2;
    static const uint16_t EC_SENSOR = A3;

    // actuator
    static const uint16_t FEED_WATER_MOTOR = 6;
    static const uint16_t FRESH_WATER_VALVE = 9;

    static const uint16_t EC_A_PUMP = 6;
    static const uint16_t EC_B_PUMP = 6;

    static const uint16_t PH_PUMP = 5;

    static const uint16_t CO2_VALVE = 7;

    static const uint16_t DEBUG_PIN = 22;

    static const uint16_t LED = 8;

    static const uint16_t DOOR_BUTTON = 23;

    static const uint16_t WATER_SENSOR = A2;

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
        
        digitalWrite(FEED_WATER_MOTOR, OFF);
        digitalWrite(FRESH_WATER_VALVE, OFF);
        digitalWrite(EC_A_PUMP, OFF);
        digitalWrite(EC_B_PUMP, OFF);
        digitalWrite(PH_PUMP, OFF);
        digitalWrite(CO2_VALVE, OFF);
        digitalWrite(LED, OFF);
    }

    static void AllON()
    {
        digitalWrite(FEED_WATER_MOTOR, ON);
        digitalWrite(FRESH_WATER_VALVE, ON);
        digitalWrite(EC_A_PUMP, ON);
        digitalWrite(EC_B_PUMP, ON);
        digitalWrite(PH_PUMP, ON);
        digitalWrite(CO2_VALVE, ON);
    }
};
