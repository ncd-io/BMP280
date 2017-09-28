// This #include statement was automatically added by the Particle IDE.
#include "BMP280.h"

BMP280 sensor;

void setup() {
    //sensor.scale = TEMP_SCALE_FAHRENHEIT;
    sensor.init();
    Particle.variable("temperature", sensor.temperature);
    Particle.variable("pressure", sensor.pressure);
}
void loop() {
    sensor.loop();
}
