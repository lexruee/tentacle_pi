/*
 * @author 	Alexander Rüedlinger <a.rueedlinger@gmail.com>
 * @date 	22.02.2015
 * 
 * A driver written in C for the sensor TSL2561.
 *
 * This driver is a port of the Adafruit TSL2561 Light Sensor Driver.
 * 
 * The original driver is written by Kevin (KTOWN) Townsend 
 * for Adafruit Industries.
 * 
 * source: https://github.com/adafruit/Adafruit_TSL2561
 * 
 */


#ifndef __TSL2561__
#define __TSL2561__
#include "tsl2561.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <time.h>
#endif


/*
 * Define debug function.
 *
 */
#if __TSL2561_DEBUG__				
#define DEBUG(...)	printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif


/*
 * Shortcut to cast void pointer to a tsl2561_t pointer.
 *
 */
#define TO_TSL(x)	(tsl2561_t*) x


/*
 * TSL2561 constants.
 *
 */
#define TSL2561_REG_CTRL 0x00
#define TSL2561_REG_TIMING 0x01

#define TSL2561_REG_CH0_LOW 0x0C
#define TSL2561_REG_CH0_HIGH 0x0D
#define TSL2561_REG_CH1_LOW 0x0E
#define TSL2561_REG_CH1_HIGH 0x0F

#define TSL2561_CMD_BIT (0x80)
#define TSL2561_WORD_BIT (0x20)

#define TSL2561_CTRL_PWR_ON 0x03
#define TSL2561_CTRL_PWR_OFF 0x00



/* 
 * Autogain thresholds
 */
#define TSL2561_AGC_THI_13MS 4850	// Max value at Ti 13ms = 5047
#define TSL2561_AGC_TLO_13MS 100	
#define TSL2561_AGC_THI_101MS 36000 // Max value at Ti 101ms = 37177
#define TSL2561_AGC_TLO_101MS 200
#define TSL2561_AGC_THI_402MS 63000	// Max value at Ti 402ms = 65535
#define TSL2561_AGC_TLO_402MS 500	



/*
 * Clipping thresholds
 */
#define TSL2561_CLIPPING_13MS 4900
#define TSL2561_CLIPPING_101MS 37000
#define TSL2561_CLIPPING_402MS 65000



/*
 * Constats for simplified lux calculation
 * according TAOS Inc.
 */
#define LUX_SCALE 14
#define RATIO_SCALE 9

#define CH_SCALE 10
#define CH_SCALE_TINT0 0x7517
#define CH_SCALE_TINT1 0x0FE7



/* 
 * T, FN, and CL Package coefficients
 */
#define TSL2561_K1T 0x0040
#define TSL2561_B1T 0x01F2
#define TSL2561_M1T 0x01BE
#define TSL2561_K2T 0x0080
#define TSL2561_B2T 0x0214
#define TSL2561_M2T 0x02D1
#define TSL2561_K3T 0x00C0
#define TSL2561_B3T 0x023F
#define TSL2561_M3T 0x037B
#define TSL2561_K4T 0x0100
#define TSL2561_B4T 0x0270
#define TSL2561_M4T 0x03FE
#define TSL2561_K5T 0x0138
#define TSL2561_B5T 0x016F
#define TSL2561_M5T 0x01fC
#define TSL2561_K6T 0x019A
#define TSL2561_B6T 0x00D2
#define TSL2561_M6T 0x00FB
#define TSL2561_K7T 0x029A
#define TSL2561_B7T 0x0018
#define TSL2561_M7T 0x0012
#define TSL2561_K8T 0x029A
#define TSL2561_B8T 0x0000
#define TSL2561_M8T 0x0000



/* 
 * CS package coefficients
 */
#define TSL2561_K1C 0x0043
#define TSL2561_B1C 0x0204
#define TSL2561_M1C 0x01AD
#define TSL2561_K2C 0x0085
#define TSL2561_B2C 0x0228
#define TSL2561_M2C 0x02C1
#define TSL2561_K3C 0x00C8
#define TSL2561_B3C 0x0253
#define TSL2561_M3C 0x0363
#define TSL2561_K4C 0x010A
#define TSL2561_B4C 0x0282
#define TSL2561_M4C 0x03DF
#define TSL2561_K5C 0x014D
#define TSL2561_B5C 0x0177
#define TSL2561_M5C 0x01DD
#define TSL2561_K6C 0x019A
#define TSL2561_B6C 0x0101
#define TSL2561_M6C 0x0127
#define TSL2561_K7C 0x029A
#define TSL2561_B7C 0x0037
#define TSL2561_M7C 0x002B
#define TSL2561_K8C 0x029A
#define TSL2561_B8C 0x0000
#define TSL2561_M8C 0x0000



/*
 * Basic TSL2561 sensor object.
 *
 */
typedef struct {
	int file;
	int address;
	uint8_t gain;
	uint8_t integration_time;
	bool  autogain;
	uint8_t type;
	char *i2c_device;
} tsl2561_t;



/*
 * Prototypes for helper functions.
 */
int tsl2561_set_addr(void*);
uint8_t tsl2561_write_byte_data(void *_tsl, uint8_t reg, uint8_t value);
uint16_t tsl2561_write_word_data(void *_tsl, uint8_t reg, uint8_t value) ;
int16_t tsl2561_read_word_data(void *_tsl, uint8_t cmd);
unsigned long tsl2561_compute_lux(void *_tsl, int visible, int channel1);



/*
 * Implemetation of the helper functions
 */


 
/*
 * Writes a byte on the i2c bus.
 * 
 * @param tsl sensor
 * @param register
 * @param value
 * @return data
 */
uint8_t tsl2561_write_byte_data(void *_tsl, uint8_t reg, uint8_t value) {
	tsl2561_t *tsl = TO_TSL(_tsl);
	uint8_t data = i2c_smbus_write_byte_data(tsl->file, reg, value);
	
	DEBUG("device %#x: write %#x to register %#x\n", tsl->address, value, reg);
	
	if(data < 0)
		DEBUG("error: helper_write8()\n");
	
	return data;
}



/*
 * Writes a word on the i2c bus.
 * 
 * @param tsl sensor
 * @param register
 * @param value
 * @return data
 */
uint16_t tsl2561_write_word_data(void *_tsl, uint8_t reg, uint8_t value) {
	tsl2561_t *tsl = TO_TSL(_tsl);
	uint16_t data = i2c_smbus_write_word_data(tsl->file, reg, value);
	
	DEBUG("device %#x: write %#x to register %#x\n", tsl->address, value, reg);

	if(data < 0)
		DEBUG("error: helper_write16()\n");
	
	return data;
}



/*
 * Reads a word from the i2c bus.
 * 
 * @param tsl sensor
 * @param register
 * @return data
 */
int16_t tsl2561_read_word_data(void *_tsl, uint8_t reg) {
	tsl2561_t *tsl = TO_TSL(_tsl);

	int error = tsl2561_set_addr(_tsl);
	if(error < 0)
		return -1;

	int16_t data = i2c_smbus_read_word_data(tsl->file, reg);
	DEBUG("device %#x: read %#x from register %#x\n", tsl->address, data, reg);
 
	return data;
}



/*
 * Sets the address for the i2c device file.
 * 
 * @param tsl sensor
 */
int tsl2561_set_addr(void *_tsl) {
	tsl2561_t* tsl = TO_TSL(_tsl);
	int error;

	if((error = ioctl(tsl->file, I2C_SLAVE, tsl->address)) < 0)
		DEBUG("error: ioctl() failed\n");

	return error;
}


/*
 * Implementation of the interface functions
 */
 

/**
 * Creates a new TSL2561 sensor object with the specified i2c address and i2c device file path.
 * 
 * @param i2c address
 * @param i2c device file path
 * @return tsl sensor
 *
 * Possible i2c addresses are: 
 * 	0x29 	(low)
 *	0x39 	(default)
 *	0x49 	(high)
 *
 * Example values for i2c device file paths are;
 *	/dev/i2c-0 	(for Raspberry Pi Revision 1.0)
 *	/dev/i2c-1	(for Rasbperry Pi Revision 2.0)
 *
 * Use i2cdetect to find the correct i2c bus.
 *
 */
void* tsl2561_init(int address, const char* i2c_device_filepath) {
	DEBUG("device: init using address %#x and i2cbus %s\n", address, i2c_device_filepath);
	
	// setup tsl2561
	void *_tsl = malloc(sizeof(tsl2561_t));
	if(_tsl == NULL)  {
		DEBUG("error: malloc returns NULL pointer\n");
		return NULL;
	}

	tsl2561_t *tsl = TO_TSL(_tsl);
	tsl->address = address;
	tsl->gain = TSL2561_GAIN_0X;
	tsl->integration_time = TSL2561_INTEGRATION_TIME_402MS;
	tsl->autogain = false;
	tsl->type = 0;

	// setup i2c device path
	tsl->i2c_device = (char*) malloc(strlen(i2c_device_filepath) * sizeof(char));
	if(tsl->i2c_device == NULL) {
		DEBUG("error: malloc returns NULL pointer!\n");
		return NULL;
	}

	// copy string
	strcpy(tsl->i2c_device, i2c_device_filepath);
	
	// open i2c device
	int file;
	if((file = open(tsl->i2c_device, O_RDWR)) < 0) {
		DEBUG("error: open() failed\n");
		return NULL;
	}
	tsl->file = file;

	if(tsl2561_set_addr(_tsl) < 0)
		return NULL;

	// setup i2c device
	tsl2561_enable(_tsl);
	tsl2561_set_timing(_tsl, tsl->integration_time, tsl->gain);

	DEBUG("device: open ok\n");

	return _tsl;
}



/**
 * Sets the type for this TSL2561 sensor.
 * 
 * @param tsl sensor
 * @param type
 *
 * Possible type values are:
 *	0  represents T version (default) (Adafruit TSL2561 is a T version)
 *	1  represents CS version
 *
 */
void tsl2561_set_type(void *_tsl, int type) {
	tsl2561_t *tsl = TO_TSL(_tsl);
	tsl->type = type;
}



/**
 * Sets the time integration and gain value for this TSL2561 sensor.
 * 
 * @param tsl sensor
 * @param integration time
 * @param gain
 *
 * Possible time integration values are:
 *      TSL2561_INTEGRATION_TIME_13MS
 *      TSL2561_INTEGRATION_TIME_101MS
 *      TSL2561_INTEGRATION_TIME_402MS
 *
 * Possible gian values are:
 *      TSL2561_GAIN_0X
 *      TSL2561_GAIN_16X
 *
 */
void tsl2561_set_timing(void *_tsl, int integration_time, int gain) {
	tsl2561_t *tsl = TO_TSL(_tsl);

	// update values
	tsl->integration_time = integration_time;
	tsl->gain = gain;

	tsl2561_write_byte_data(_tsl, TSL2561_CMD_BIT | TSL2561_REG_TIMING, tsl->integration_time | tsl->gain);
}



/**
 * Sets the gain value for this TSL2561 sensor.
 * 
 * @param tsl sensor
 * @param gain
 *
 * Possible gain values are:
 * 	TSL2561_GAIN_0X
 * 	TSL2561_GAIN_16X
 *
 */
void tsl2561_set_gain(void *_tsl, int gain) {
	tsl2561_t *tsl = TO_TSL(_tsl);
	tsl2561_set_timing(_tsl, tsl->integration_time, gain);
}



/**
 * Sets the integration value for this TSL2561 sensor.
 * 
 * @param tsl sensor
 * @param time value
 *
 * Possible time values are:
 *	TSL2561_INTEGRATION_TIME_13MS
 *	TSL2561_INTEGRATION_TIME_101MS
 *	TSL2561_INTEGRATION_TIME_402MS
 *
 */
void tsl2561_set_integration_time(void *_tsl, int time) {
	tsl2561_t *tsl = TO_TSL(_tsl);
	tsl2561_set_timing(_tsl, time, tsl->gain);
}



/**
 * Enables autogain for this TSL2561 sensor.
 * 
 * @param tsl sensor
 */
void tsl2561_enable_autogain(void *_tsl) {
	tsl2561_t *tsl = TO_TSL(_tsl);
	tsl->autogain = true;
}



/**
 * Disables autogain for this TSL2561 sensor.
 *
 * @param tsl sensor
 */
void tsl2561_disable_autogain(void *_tsl) {
	tsl2561_t *tsl = TO_TSL(_tsl);
	tsl->autogain = false;
}


/**
 * Enables this TSL2561 sensor.
 * 
 * @param tsl sensor
 * @return error code
 */
int tsl2561_enable(void *_tsl) {
	return tsl2561_write_byte_data(_tsl, TSL2561_CMD_BIT | TSL2561_REG_CTRL, TSL2561_CTRL_PWR_ON);
}


/**
 * Disables this TSL2561 sensor.
 *
 * @param tsl sensor
 * @return error code
 */
int tsl2561_disable(void *_tsl) {
	return tsl2561_write_byte_data(_tsl, TSL2561_CMD_BIT | TSL2561_REG_CTRL, TSL2561_CTRL_PWR_OFF);
}


/**
 * Closes a TSL2561 object.
 *
 * @param tsl sensor
 */
void tsl2561_close(void *_tsl) {
	DEBUG("close tsl2561 device\n");
	tsl2561_t *tsl = TO_TSL(_tsl);
	
	if(close(tsl->file) < 0)
		DEBUG("error: %s close() failed\n", tsl->i2c_device);
	
	free(tsl->i2c_device); // free string
	tsl->i2c_device = NULL;
	free(tsl); // free tsl structure
	_tsl = NULL;
} 


/*
 * Reads values from a TSL2561 sensor.
 *
 */
#define TSL2561_FACTOR_US 1000000
	
void tsl2561_read(void *_tsl, int *broadband, int *ir) {
	tsl2561_enable(_tsl);
	tsl2561_t *tsl = TO_TSL(_tsl);

	// wait until ADC is complete
	switch(tsl->integration_time) {
		case TSL2561_INTEGRATION_TIME_402MS:
			usleep(0.403 * TSL2561_FACTOR_US);
			break;

		case TSL2561_INTEGRATION_TIME_101MS:
			usleep(0.102 * TSL2561_FACTOR_US);
			break;

		case TSL2561_INTEGRATION_TIME_13MS:
			usleep(0.014 * TSL2561_FACTOR_US);
			break;
		default:
			usleep(0.403 * TSL2561_FACTOR_US);
			break;
	}

	*broadband = tsl2561_read_word_data(_tsl, TSL2561_CMD_BIT | TSL2561_WORD_BIT | TSL2561_REG_CH0_LOW);
	*ir = tsl2561_read_word_data(_tsl, TSL2561_CMD_BIT | TSL2561_WORD_BIT | TSL2561_REG_CH1_LOW);
	
	if( *broadband < 0 || *ir < 0){
		DEBUG("error: i2c_smbus_read_word_data() failed\n");
	} else {
		DEBUG("bb=%i, ir=%i\n", *broadband, *ir);
	}
	tsl2561_disable(_tsl);
}


/*
 * Computes a lux value for this TSL2561 sensor.
 *
 */
long tsl2561_lux(void *_tsl) {
	tsl2561_t *tsl = TO_TSL(_tsl);
	int visible,  channel1, threshold;	
	tsl2561_luminosity(_tsl, &visible, &channel1);

	switch(tsl->integration_time) {
		case TSL2561_INTEGRATION_TIME_13MS:
			threshold = TSL2561_CLIPPING_13MS;
			break;
		case TSL2561_INTEGRATION_TIME_101MS:
			threshold = TSL2561_CLIPPING_101MS;
			break;
		default:
			threshold = TSL2561_CLIPPING_402MS;
			break;
	}

	if((visible > threshold) || (channel1 > threshold)) 
		return 0;

	return tsl2561_compute_lux(_tsl, visible, channel1);
}


void tsl2561_luminosity(void *_tsl, int *channel0, int *channel1) {
	tsl2561_t *tsl = TO_TSL(_tsl);
	uint16_t hi, lo;	
	bool agc_check = false, valid = false;

	if(!tsl->autogain) { 
		tsl2561_read(_tsl, channel0, channel1);	
		return;
	}

	while (!valid) {
		switch(tsl->integration_time) {
			case TSL2561_INTEGRATION_TIME_13MS:
				hi = TSL2561_AGC_THI_13MS;
				lo = TSL2561_AGC_TLO_13MS;
				break;

			case TSL2561_INTEGRATION_TIME_101MS:
				hi = TSL2561_AGC_THI_101MS;
				lo = TSL2561_AGC_TLO_101MS;
				break;
		
			default: 
				hi = TSL2561_AGC_THI_402MS;
				lo = TSL2561_AGC_TLO_402MS;
				break;	
		}
		
		tsl2561_read(_tsl, channel0, channel1);
		if(!agc_check) {
			if((*channel0 < lo) && (tsl->gain == TSL2561_GAIN_0X)) {
				tsl2561_set_gain(_tsl, TSL2561_GAIN_16X);
				tsl2561_read(_tsl, channel0, channel1);
				agc_check = true;

			} else if((*channel0 > hi) && (tsl->gain == TSL2561_GAIN_16X)) {
				tsl2561_set_gain(_tsl, TSL2561_GAIN_0X);
				tsl2561_read(_tsl, channel0, channel1);
				agc_check = true;
			} else {
				valid = true;
			}
		} else {
			valid = true;
		}
	}
}


/*
 * Helper function for computing lux values.
 * It uses a lux equation approximation without floating point calculations.
 *
 */
unsigned long tsl2561_compute_lux(void *_tsl, int ch0, int ch1) {
	tsl2561_t *tsl = TO_TSL(_tsl);
	unsigned long ch_scale, channel0, channel1;

	// first, scale the channel values depending on the gain and integration time
	// 16X, 402mS is nominal.
	// scale if integration time is NOT 402 msec
	switch(tsl->integration_time) {
		case TSL2561_INTEGRATION_TIME_13MS:
			ch_scale = CH_SCALE_TINT0;
			break;

		case TSL2561_INTEGRATION_TIME_101MS:
			ch_scale = CH_SCALE_TINT1;
			break;
		
		default:
			ch_scale = (1 << CH_SCALE);
			break;
	}

	// scale if gain is NOT 16X
	if(!tsl->gain) 
		ch_scale = (ch_scale << 4);	// scale 1X to 16X

	// scale the channel values
	channel0 = (ch0 * ch_scale) >> CH_SCALE;
	channel1 = (ch1 * ch_scale) >> CH_SCALE;

	unsigned long ratio = 0, ratio1 = 0;

	// find the ratio of the channel values (Channel1/Channel0)
	// protect against divide by zero
	if(channel0 != 0) 
		ratio1 = (channel1 << (RATIO_SCALE + 1)) / channel0;
	
	// round the ratio value
	ratio = (ratio1 + 1) >> 1;

	int b, m;

	// is ratio <= eachBreak ?
	switch(tsl->type){	
		case 1:
			if((ratio >= 0) && (ratio <= TSL2561_K1C)){ 
				b = TSL2561_B1C; m = TSL2561_M1C; 
			} else if(ratio <= TSL2561_K2C) {
				b = TSL2561_B2C; m = TSL2561_M2C;
			} else if(ratio <= TSL2561_K3C) {
				b = TSL2561_B3C; m = TSL2561_M3C;
			} else if(ratio <= TSL2561_K4C) {
				b = TSL2561_B4C; m = TSL2561_M4C;
			} else if (ratio <= TSL2561_K5T) {
				b = TSL2561_B5C; m = TSL2561_M5C;
			} else if(ratio <= TSL2561_K6T) {
				b = TSL2561_B6C; m = TSL2561_M6C;
			} else if(ratio <= TSL2561_K7T) {
				b = TSL2561_B7C; m = TSL2561_M7C;
			} else if(ratio > TSL2561_K8C) {
				b = TSL2561_B8C; m = TSL2561_M8C;
			}
			break;
		
		case 0:
		default:
			if((ratio >= 0) && (ratio <= TSL2561_K1T)){ 
				b = TSL2561_B1T; m = TSL2561_M1T;
			} else if(ratio <= TSL2561_K2T) {
				b = TSL2561_B2T; m = TSL2561_M2T;
			} else if(ratio <= TSL2561_K3T) {
				b = TSL2561_B3T; m = TSL2561_M3T;
			} else if(ratio <= TSL2561_K4T) {
				b = TSL2561_B4T; m = TSL2561_M4T;
			} else if (ratio <= TSL2561_K5T) {
				b = TSL2561_B5T; m = TSL2561_M5T;
			} else if(ratio <= TSL2561_K6T) {
				b = TSL2561_B6T; m = TSL2561_M6T;
			} else if(ratio <= TSL2561_K7T) {
				b = TSL2561_B7T; m = TSL2561_M7T;
			} else if(ratio > TSL2561_K8T) {
				b = TSL2561_B8T; m = TSL2561_M8T;
			}
			break;
	}	
	
	unsigned long tmp = (channel0 * b) - (channel1 * m);

	if(tmp < 0) 
		tmp = 0;

	tmp += (1 << (LUX_SCALE-1));
	unsigned long lux = (tmp >> LUX_SCALE);

	return lux;		
}
