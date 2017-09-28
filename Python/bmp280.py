#Control register
BMP280_CTRL_REG = 0xF4

BMP280_MODE_SLEEP = 0x00
BMP280_MODE_FORCED = 0x01
BMP280_MODE_NORMAL = 0x03

BMP280_OSP_0 = 0x00
BMP280_OSP_1 = 0x04
BMP280_OSP_2 = 0x08
BMP280_OSP_4 = 0x0C
BMP280_OSP_8 = 0x10
BMP280_OSP_16 = 0x14

BMP280_OST_0 = 0x00
BMP280_OST_1 = 0x20
BMP280_OST_2 = 0x40
BMP280_OST_4 = 0x60
BMP280_OST_8 = 0x80
BMP280_OST_16 = 0xA0

#Config register
BMP280_CONFIG_REG = 0xF5

BMP280_FILTER_0 = 0x00
BMP280_FILTER_2 = 0x04
BMP280_FILTER_4 = 0x08
BMP280_FILTER_8 = 0x0C
BMP280_FILTER_16 = 0x10

BMP280_TSB_0_5 = 0x00
BMP280_TSB_62_5 = 0x20
BMP280_TSB_125 = 0x40
BMP280_TSB_250 = 0x60
BMP280_TSB_500 = 0x80
BMP280_TSB_1000 = 0xA0
BMP280_TSB_2000 = 0xC0
BMP280_TSB_4000 = 0xE0

#Trimming Parameter Register
BMP280_PARAMS_START = 0x88

#Read registers
BMP280_PRESS_REG0 = 0xF7
BMP280_TEMP_REG0 = 0xFA

#Temperature scales
TEMP_SCALE_CELSIUS = 0x00
TEMP_SCALE_FAHRENHEIT = 0x01
TEMP_SCALE_KELVIN = 0x02

class BMP280():
	def __init__(self, smbus, kwargs = {}):
		self.__dict__.update(kwargs)
		if not hasattr(self, 'address'):
			self.address = 0x76
		if not hasattr(self, 'mode'):
			self.mode = BMP280_MODE_NORMAL
		if not hasattr(self, 'scale'):
			self.scale = TEMP_SCALE_CELSIUS
		if not hasattr(self, 'osrs_t'):
			self.osrs_t = BMP280_OST_2
		if not hasattr(self, 'osrs_p'):
			self.osrs_p = BMP280_OSP_2
		if not hasattr(self, 'sb_time'):
			self.sb_time = BMP280_TSB_0_5
		if not hasattr(self, 'filter'):
			self.filter = BMP280_FILTER_0
		self.smbus = smbus

		self.temp_comp = []
		self.press_comp = []

		self.smbus.write_byte_data(self.address, BMP280_CTRL_REG, self.osrs_p | self.osrs_t | self.mode)
		self.smbus.write_byte_data(self.address, BMP280_CONFIG_REG, self.sb_time | self.filter)
		self.get_compensations()

	def get_compensations(self):
		ptdata = self.smbus.read_i2c_block_data(self.address, BMP280_PARAMS_START, 24)
		print ptdata
		self.temp_comp.append(ptdata[0] + (ptdata[1] << 8))
		self.temp_comp.append(ptdata[2] + (ptdata[3] << 8))
		self.temp_comp.append(ptdata[4] + (ptdata[5] << 8))
		print self.temp_comp
		for (i, comp_value) in enumerate(self.temp_comp):
			if comp_value > 32767 and i != 0:
				self.temp_comp[i] -= 65536
		print self.temp_comp
		self.press_comp.append(ptdata[6] + (ptdata[7] << 8))
		self.press_comp.append(ptdata[8] + (ptdata[9] << 8))
		self.press_comp.append(ptdata[10] + (ptdata[11] << 8))
		self.press_comp.append(ptdata[12] + (ptdata[13] << 8))
		self.press_comp.append(ptdata[14] + (ptdata[15] << 8))
		self.press_comp.append(ptdata[16] + (ptdata[17] << 8))
		self.press_comp.append(ptdata[18] + (ptdata[19] << 8))
		self.press_comp.append(ptdata[20] + (ptdata[21] << 8))
		self.press_comp.append(ptdata[22] + (ptdata[23] << 8))
		for (i, comp_value) in enumerate(self.press_comp):
			if comp_value > 32767 and i != 0:
				self.press_comp[i] -= 65536

	def get_readings(self):
		vardata = self.smbus.read_i2c_block_data(self.address, BMP280_PRESS_REG0, 6)

		press_var = (vardata[0] << 12) + (vardata[1] << 4) + (vardata[2] >> 4)
		temp_var = (vardata[3] << 12) + (vardata[4] << 4) + (vardata[5] >> 4)
		print temp_var
		print '+++'
		tvar1 = ((float(temp_var))/16384.0 - (float(self.temp_comp[0]))/1024.0) * (float(self.temp_comp[1]))
		tvar2 = (((float(temp_var))/131072.0 - (float(self.temp_comp[0]))/8192.0) * ((float(temp_var))/131072.0 - (float(self.temp_comp[0]))/8192.0)) * (float(self.temp_comp[2]))
		print tvar2
		print self.temp_comp[2]
		t_fine = tvar1 + tvar2
		print t_fine
		temperature = t_fine / 5120.0

		if(self.scale == TEMP_SCALE_KELVIN):
			temperature = temperature + 273.15
		elif(self.scale == TEMP_SCALE_FAHRENHEIT):
			temperature = temperature * 1.8 + 32

		pressure = 256

		pvar1 = (float(t_fine)/2.0) - 64000.0
		pvar2 = pvar1 * pvar1 * (float(self.press_comp[5])) / 32768.0
		pvar2 = pvar2 + pvar1 * (float(self.press_comp[4])) * 2.0
		pvar2 = (pvar2/4.0)+((float(self.press_comp[3])) * 65536.0)
		pvar1 = ((float(self.press_comp[2])) * pvar1 * pvar1 / 524288.0 + (float(self.press_comp[1])) * pvar1) / 524288.0
		pvar1 = (1.0 + pvar1 / 32768.0)*(float(self.press_comp[0]))
		if (pvar1 != 0.0):
			pressure = 1048576.0 - float(press_var)
			pressure = (pressure - (pvar2 / 4096.0)) * 6250.0 / pvar1
			pvar1 = (float(self.press_comp[8])) * pressure * pressure / 2147483648.0
			pvar2 = pressure * (float(self.press_comp[7])) / 32768.0
			pressure = pressure + (pvar1 + pvar2 + (float(self.press_comp[6]))) / 16.0

		return {'temperature': temperature, 'pressure': pressure}
