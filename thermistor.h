#ifndef __THERMISTOR_H_
#define __THERMISTOR_H_

#include <avr/pgmspace.h>

// Value when sum of ADC values is more than first value in table
#define TEMPERATURE_UNDER -50
// Value when sum of ADC values is less than last value in table 
#define TEMPERATURE_OVER 3000
// Value corresponds to first entry in table
#define TEMPERATURE_TABLE_START -50
// Table step
#define TEMPERATURE_TABLE_STEP 25

// Number of samples in adc sum
#define ADC_SAMPLES 64

// Type of each table item. If sum fits into 16 bits - uint16_t, else - uint32_t
typedef uint16_t temperature_table_entry_type;
// Type of table index. If table has more than 255 items, then uint16_t, else - uint8_t
typedef uint8_t temperature_table_index_type;
// Access method to table entry. Should correspond to temperature_table_entry_type
#define TEMPERATURE_TABLE_READ(i) pgm_read_word(&termo_table[i])

int16_t calc_temperature(temperature_table_entry_type adcsum);

#endif