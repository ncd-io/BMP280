#include "Particle.h"

//Control register
#define BMP280_CTRL_REG 0xF4

#define BMP280_MODE_SLEEP 0x00
#define BMP280_MODE_FORCED 0x01
#define BMP280_MODE_NORMAL 0x03

#define BMP280_OSP_0 0x00
#define BMP280_OSP_1 0x04
#define BMP280_OSP_2 0x08
#define BMP280_OSP_4 0x0C
#define BMP280_OSP_8 0x10
#define BMP280_OSP_16 0x14

#define BMP280_OST_0 0x00
#define BMP280_OST_1 0x20
#define BMP280_OST_2 0x40
#define BMP280_OST_4 0x60
#define BMP280_OST_8 0x80
#define BMP280_OST_16 0xA0

//Config register
#define BMP280_CONFIG_REG 0xF5

#define BMP280_FILTER_0 0x00
#define BMP280_FILTER_2 0x04
#define BMP280_FILTER_4 0x08
#define BMP280_FILTER_8 0x0C
#define BMP280_FILTER_16 0x10

#define BMP280_TSB_0_5 0x00
#define BMP280_TSB_62_5 0x20
#define BMP280_TSB_125 0x40
#define BMP280_TSB_250 0x60
#define BMP280_TSB_500 0x80
#define BMP280_TSB_1000 0xA0
#define BMP280_TSB_2000 0xC0
#define BMP280_TSB_4000 0xE0

//Trimming Parameter Register
#define BMP280_PARAMS_START 0x88

//Read registers
#define BMP280_PRESS_REG0 0xF7
#define BMP280_TEMP_REG0 0xFA

//Temperature scales
#define TEMP_SCALE_CELSIUS 0x00
#define TEMP_SCALE_FAHRENHEIT 0x01
#define TEMP_SCALE_KELVIN 0x02

class BMP280{
public:
    int address = 0x76;

    void setAddress(int a0);
    void init();

    int scale = TEMP_SCALE_CELSIUS;
    int mode = BMP280_MODE_NORMAL;
    int osrs_t = BMP280_OST_2;
    int osrs_p = BMP280_OSP_2;
    int sb_time = BMP280_TSB_0_5;
    int filter = BMP280_FILTER_0;

    double temperature;
    double pressure;

    void takeReadings();
    void loop();
private:
    int last_checked = 0;
	int loop_delay = 0;

    int temp_comp[3];
    int press_comp[9];

    int readRate();
    void getParams();

    void begin();
    void writeByte(int reg, int data);
    void readBuffer(int reg, int *buff, int length);
    int readByte(int reg);
	int _signed(int n, int sign);
};
