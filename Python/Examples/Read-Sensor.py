import time
import smbus
import bmp280
# Get I2C bus, this is I2C Bus 1
bus = smbus.SMBus(1)

#kwargs is a Python set that contains the address of your device as well as additional device and calibration values.
#kwargs does not have to be populated as every value is optional and will be replaced with a default value if not specified.

#below is an example of a kwarg declaration that is populated with all of the default values for each user configurable property
#refer to the datasheet for this chip to calculate what values you should be using for your project.
kwargs = {'address': 0x76, 'mode': 0x03, 'osrs_t': 0x40, 'osrs_p': 0x08, 'scale': 0x01, 'sb_time': 0x00, 'filter': 0x00}
#create the BMP280 object from the BMP280 library
#the object requires that you pass it the bus object so that it can communicate and share the bus with other chips if necessary
board = bmp280.BMP280(bus, kwargs)

while True :
	all_readings = board.get_readings()
	print 'All Readings:'
	print all_readings
	print '---'
	time.sleep(.25)
