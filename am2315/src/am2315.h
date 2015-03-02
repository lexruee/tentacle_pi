/*
 * @author 	Alexander Rüedlinger <a.rueedlinger@gmail.com>
 * @date 	28.02.2015
 *
 * A C driver for the sensor AM2315.
 *  
 */
 
void *am2315_init(int address, const char* i2c_device_filepath);

void am2315_close(void *_am);

float am2315_temperature(void *_am);

float am2315_humidity(void *_am);

int am2315_read_data(void *_am, float *temperature, float *humidity);
