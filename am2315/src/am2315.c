/*
 * @author 	Alexander Rüedlinger <a.rueedlinger@gmail.com>
 * @date 	28.02.2015
 *
 * A C driver for the sensor AM2315.
 *  
 */
 
#ifndef __AM2315__
#define __AM2315__
#include <stdint.h>
#include "am2315.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <math.h>
#endif

/*
 * am2315 commands
 */
#define AM2315_CMD_READ_REG 0x03



/*
 * Define debug function.
 */
//#define __AM2315_DEBUG__
#ifdef __AM2315_DEBUG__				
#define DEBUG(...)	printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif


/*
 * Shortcut to cast void pointer to aam2315_t pointer
 */
#define TO_AM(x)	(am2315_t*) x



/*
 * Basic structure for the am2315 sensor
 */
typedef struct {
	/* file descriptor */
	int file;
	
	/* i2c device address */
	int address;
	
	/* i2c device file path */
	char *i2c_device;
} am2315_t;


/*
 * Prototypes for helper functions
 */
void am2315_wakeup(void *_am);
int am2315_set_addr(void *_am);
float am2315_compute_temperature(unsigned char temperature_h, unsigned char temperature_l);
float am2315_compute_humidty(unsigned char humidty_h, unsigned char humidty_l);
int am2315_read_data(void *_am, float *temperature, float *humidity);
unsigned short crc16(unsigned char *ptr, unsigned char len);


/*
 * Implemetation of the helper functions
 */


/*
 * Wakeup the am2315 sensor.
 */
void am2315_wakeup(void *_am) {
	am2315_t* am = TO_AM(_am);
	int32_t data = i2c_smbus_write_byte(am->file, 0);
	if(data < 0)
		DEBUG("error: wakeup()\n");
	usleep(900); // 900us
}



/*
 * Compute humidity value based on the msb and lsb bytes.
 * 
 * @param msb
 * @param lsb
 * @return humidity
 */
float am2315_compute_humidity(unsigned char msb, unsigned char lsb) {
	int humidity_h, humidity_l;
	float hum;
	humidity_h = msb << 8;
	humidity_l = lsb;
	hum = (humidity_h + humidity_l) / 10.0;
	return hum;
}


/*
 * Compute temperature value based on the msb and lsb bytes. 
 * 
 * @param msb
 * @param lsb
 * @return temperature
 */
float am2315_compute_temperature(unsigned char msb, unsigned char lsb) {
	int temperature_h, temperature_l;
	float tmp;
	
	temperature_h = msb & 0x7F; // ignore first bit
	temperature_l = lsb;
	tmp = (temperature_h << 8) + temperature_l;
	tmp /= 10.0;
	
	if(msb & 0x80) // check if negative
		tmp *= -1;
	
	return tmp;
}



/*
 * Computes the crc code.
 * 
 * @param char pointer
 * @param length
 * @return crc
 */
uint16_t am2315_crc16(unsigned char *ptr, unsigned char len) {
	unsigned short crc = 0xFFFF;
	unsigned char i;
	
	while(len--) {
		crc ^= *ptr++;
		for(i = 0; i < 8; i++) {
			if(crc & 0x01) {
				crc >>= 1;
				crc ^= 0xA001;
			} else {
				crc >>= 1;
			}
		}
		
	}
	return crc;
}



/*
 * Sets the address for the i2c device file.
 * 
 * @param am2315 sensor
 * @return error code
 */
int am2315_set_addr(void *_am) {
	am2315_t* am = TO_AM(_am);
	int error;

	if((error = ioctl(am->file, I2C_SLAVE, am->address)) < 0)
		DEBUG("error: ioctl() failed\n");

	return error;
}



/*
 * Implementation of the interface functions
 */


/**
 * Read temperature and humidity value from the am2315 sensor.
 * 
 * @param am2315 sensor
 * @param temperature
 * @param humidity
 * @return crc check (1 for ok)
 */
int am2315_read_data(void *_am, float *temperature, float *humidity) {
	am2315_t *am = TO_AM(_am);
	
	am2315_wakeup(_am);
	
	static unsigned char send[3] = {
		AM2315_CMD_READ_REG,	// read command
		0x00,					// use start register 0x00
		0x04					// read 4 bytes
	};
	// in other words we read the bytes 0x00, 0x01, 0x02, 0x03
	// 0x00 = humidity_h, 0x01 = humidity_l, 
	// 0x02 = temperature_h, 0x03 = temperature_l
	
	if(write(am->file, send, 3) < 0) {
		DEBUG("error: write()\n");
		return -1;
	}
	
	usleep(10*1000); // 10ms
	
	unsigned char buf[8]; // data buffer
	
	if(read(am->file, &buf, 8) < 0 ) {
		DEBUG("error: read()\n");
		return -1;
	} else {
		int i;
		for(i = 0; i < 8; i++) {
			DEBUG("byte %i: %#x\n", i, buf[i]);
		}
	}
		
	// compute humidity value
	*humidity = am2315_compute_humidity(buf[2], buf[3]);
	
	// compute temperature value
	*temperature = am2315_compute_temperature(buf[4], buf[5]);
	
	// compute crc
	uint16_t crc_res = am2315_crc16(buf, 6);
	uint16_t crc = (buf[7] << 8) + buf[6];
		
	DEBUG("tmp: %f\n", *temperature);
	DEBUG("hum: %f\n", *humidity);
	DEBUG("crc: %i\n", crc);
	DEBUG("crc_res: %i\n", crc_res);
	DEBUG("crc_ok: %i\n", crc_res == crc);
		
	return crc_res == crc;
}



/**
 * Creates a AM2315 sensor object.
 *
 * @param i2c device address
 * @param i2c device file path
 * @return am2315 sensor
 */
void *am2315_init(int address, const char* i2c_device_filepath) {
	DEBUG("device: init using address %#x and i2cbus %s\n", address, i2c_device_filepath);
	
	// setup am2315
	void *_am = malloc(sizeof(am2315_t));
	if(_am == NULL)  {
		DEBUG("error: malloc returns NULL pointer\n");
		return NULL;
	}

	am2315_t *am = TO_AM(_am);
	am->address = address;

	// setup i2c device path
	am->i2c_device = (char*) malloc(strlen(i2c_device_filepath) * sizeof(char));
	if(am->i2c_device == NULL) {
		DEBUG("error: malloc returns NULL pointer!\n");
		return NULL;
	}

	// copy string
	strcpy(am->i2c_device, i2c_device_filepath);
	
	// open i2c device
	int file;
	if((file = open(am->i2c_device, O_RDWR)) < 0) {
		DEBUG("error: %s open() failed\n", am->i2c_device);
		return NULL;
	}
	am->file = file;

	if(am2315_set_addr(_am) < 0)
		return NULL;

	// setup i2c device
	
	DEBUG("device: open ok\n");

	return _am;
}



/**
 * Closes a AM2315 object.
 * 
 * @param am2315 sensor
 */
void am2315_close(void *_am) {
	DEBUG("close am2315 device\n");
	am2315_t *am = TO_AM(_am);
	
	if(close(am->file) < 0)
		DEBUG("error: %s close() failed\n", am->i2c_device);
	
	free(am->i2c_device); // free string
	am->i2c_device = NULL;
	free(am); // free bmp structure
	_am = NULL;
} 



/**
 * Returns the measured temperature in celsius.
 * 
 * @param am2315 sensor
 * @return temperature
 */
float am2315_temperature(void *_am) {
	float temperature, humidity;
	am2315_read_data(_am, &temperature, &humidity);
	return temperature;
}



/**
 * Returns the measured humidity in percentage.
 * 
 * @param am2315 sensor
 * @return humidity
 */
float am2315_humidity(void *_am) {
	float temperature, humidity;
	am2315_read_data(_am, &temperature, &humidity);
	return humidity;
}
