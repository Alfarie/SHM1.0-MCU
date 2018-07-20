class EEPROM_ADDR{
    public:
        static const int EC_SETPOINT = sizeof(float)*0;
        static const int EC_DETECTING = sizeof(float)*1;
        static const int EC_WORKING = sizeof(float)*2;
        static const int EC_CAL = sizeof(float)*3;

        static const int PH_SETPOINT = sizeof(float)*4;
        static const int PH_DETECTING = sizeof(float)*5;
        static const int PH_WORKING = sizeof(float)*6;
        static const int PH_CAL = sizeof(float)*7;

        static const int CO2_SETPOINT = sizeof(float)*8;

        static const int ISINIT_EEPROM = 50 + sizeof(int)*0;

        static const int TIMER_SIZE = 100 - sizeof(int);
        static const int TIMER_ADDR_ST = 100;

        static const int EC_TIMER_ADDR_ST = 200;
        static const int EC_TIMER_SIZE = EC_TIMER_ADDR_ST - sizeof(int);
        static const int EC_MODE = EC_TIMER_ADDR_ST - (sizeof(int)*2);
        static const int EC_TIMER_WORKING = EC_TIMER_ADDR_ST - (sizeof(int)*3);
};