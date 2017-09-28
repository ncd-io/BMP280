#include "BMP280.h"

void BMP280::init(){
    begin();
    writeByte(BMP280_CTRL_REG, osrs_p | osrs_t | mode);
    writeByte(BMP280_CONFIG_REG, sb_time | filter);
    getParams();
}

void BMP280::setAddress(int a0){
    address |= a0;
}

void BMP280::getParams(){
    int ptdata[25];
    readBuffer(BMP280_PARAMS_START, ptdata, 24);
    temp_comp[0] = ptdata[0] + (ptdata[1] << 8);
    temp_comp[1] = _signed(ptdata[2] + (ptdata[3] << 8), 16);
    temp_comp[2] = _signed(ptdata[4] + (ptdata[5] << 8), 16);

    press_comp[0] = ptdata[6] + (ptdata[7] << 8);
    press_comp[1] = _signed(ptdata[8] + (ptdata[9] << 8), 16);
    press_comp[2] = _signed(ptdata[10] + (ptdata[11] << 8), 16);
    press_comp[3] = _signed(ptdata[12] + (ptdata[13] << 8), 16);
    press_comp[4] = _signed(ptdata[14] + (ptdata[15] << 8), 16);
    press_comp[5] = _signed(ptdata[16] + (ptdata[17] << 8), 16);
    press_comp[6] = _signed(ptdata[18] + (ptdata[19] << 8), 16);
    press_comp[7] = _signed(ptdata[20] + (ptdata[21] << 8), 16);
    press_comp[8] = _signed(ptdata[22] + (ptdata[23] << 8), 16);
}

int BMP280::readRate(){
    int rates[6] = {0,1,2,4,8,16};
    int tmeas = (2.3 * rates[(osrs_t >> 5)]) + (2.3 * rates[(osrs_p & 0x1C) >> 2] + .5);
    if(mode == BMP280_MODE_NORMAL){
        switch(sb_time){
            case BMP280_TSB_0_5:
                tmeas += .5;
                break;
            case BMP280_TSB_62_5:
                tmeas += 62.5;
                break;
            case BMP280_TSB_125:
                tmeas += 125;
                break;
            case BMP280_TSB_250:
                tmeas += 250;
                break;
            case BMP280_TSB_500:
                tmeas += 500;
                break;
            case BMP280_TSB_1000:
                tmeas += 1000;
                break;
            case BMP280_TSB_2000:
                tmeas += 2000;
                break;
            case BMP280_TSB_4000:
                tmeas += 4000;
                break;
        }
    }
    float hz = 1000 / tmeas;
    if(filter){
        switch(filter){
            //Filter settings (filtering out noise)
            case BMP280_FILTER_2:
                hz = 2000/hz;
            case BMP280_FILTER_4:
                hz = 5000/hz;
            case BMP280_FILTER_8:
                hz = 11000/hz;
            case BMP280_FILTER_16:
                hz = 22000/hz;
        }
    }
    return 1/hz*1000;
}

void BMP280::takeReadings(){
    int vardata[7];
    readBuffer(BMP280_PRESS_REG0, vardata, 6);

    int press_var = (vardata[0] << 12) + (vardata[1] << 4) + (vardata[2] >> 4);
    int temp_var = (vardata[3] << 12) + (vardata[4] << 4) + (vardata[5] >> 4);

    //Calculate temperature
    int tvar1 = (((double)temp_var)/16384.0 - ((double)temp_comp[0])/1024.0) * ((double)temp_comp[1]);
    int tvar2 = ((((double)temp_var)/131072.0 - ((double)temp_comp[0])/8192.0) * (((double)temp_var)/131072.0 - ((double) temp_comp[0])/8192.0)) * ((double)temp_comp[2]);
    int t_fine = tvar1 + tvar2;
    temperature = t_fine / 5120.0;

    if(scale == TEMP_SCALE_KELVIN) temperature = temperature + 273.15;
    else if(scale == TEMP_SCALE_FAHRENHEIT) temperature = temperature * 1.8 + 32;

    //Calculate pressure
    pressure = 256;

    double pvar1 = ((double)t_fine/2.0) - 64000.0;
    double pvar2 = pvar1 * pvar1 * ((double)press_comp[5]) / 32768.0;
    pvar2 = pvar2 + pvar1 * ((double)press_comp[4]) * 2.0;
    pvar2 = (pvar2/4.0)+(((double)press_comp[3]) * 65536.0);
    pvar1 = (((double)press_comp[2]) * pvar1 * pvar1 / 524288.0 + ((double)press_comp[1]) * pvar1) / 524288.0;
    pvar1 = (1.0 + pvar1 / 32768.0)*((double)press_comp[0]);
    if (pvar1 != 0.0){
        pressure = 1048576.0 - (double)press_var;
        pressure = (pressure - (pvar2 / 4096.0)) * 6250.0 / pvar1;
        pvar1 = ((double)press_comp[8]) * pressure * pressure / 2147483648.0;
        pvar2 = pressure * ((double)press_comp[7]) / 32768.0;
        pressure = pressure + (pvar1 + pvar2 + ((double)press_comp[6])) / 16.0;
    }

}

void BMP280::loop(){
    if(loop_delay == 0){
        loop_delay = readRate();
    }
    int now = millis();
    if(now-last_checked > 1000){
        last_checked = now;
        takeReadings();
    }
}

void BMP280::begin(){
    if(!Wire.isEnabled()) Wire.begin();
}

void BMP280::writeByte(int reg, int data){
    Wire.beginTransmission(address);
    Wire.write(reg);
    if(data < 256) Wire.write(data);
    Wire.endTransmission();
}

int BMP280::readByte(int reg){
    writeByte(reg, 256);
    Wire.requestFrom(address, 1);
    return Wire.read();
}

void BMP280::readBuffer(int reg, int *buff, int len){
    writeByte(reg, 256);
    Wire.requestFrom(address, len);
    for(int i=0;i<len;i++){
        buff[i] = Wire.read();
    }
}

int BMP280::_signed(int n, int bits){
	if(n > (1 << (bits-1))) return n - (1 << bits);
	return n;
}
