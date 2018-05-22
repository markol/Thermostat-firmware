// Adjust your own custom setup

// Crystal frequency
#define F_CPU 7372800L   

// Pinouts
#define DIGIT_A_PIN (1 << PB2)
#define DIGIT_A_PORT PORTB
#define DIGIT_A_DDR DDRB

#define DIGIT_B_PIN (1 << PC3)
#define DIGIT_B_PORT PORTC
#define DIGIT_B_DDR DDRC

#define DIGIT_C_PIN (1 << PD6)
#define DIGIT_C_PORT PORTD
#define DIGIT_C_DDR DDRD

#define DIGIT_D_PIN (1 << PB0)
#define DIGIT_D_PORT PORTB
#define DIGIT_D_DDR DDRB

#define DIGIT_E_PIN (1 << PD5)
#define DIGIT_E_PORT PORTD
#define DIGIT_E_DDR DDRD

#define DIGIT_F_PIN (1 << PB1)
#define DIGIT_F_PORT PORTB
#define DIGIT_F_DDR DDRB

#define DIGIT_G_PIN (1 << PD7)
#define DIGIT_G_PORT PORTD
#define DIGIT_G_DDR DDRD

#define DIGIT_1_PIN (1 << PD0)
#define DIGIT_1_PORT PORTD
#define DIGIT_1_DDR DDRD

#define DIGIT_2_PIN (1 << PC5)
#define DIGIT_2_PORT PORTC
#define DIGIT_2_DDR DDRC

#define DIGIT_3_PIN (1 << PC4)
#define DIGIT_3_PORT PORTC
#define DIGIT_3_DDR DDRC

#define ENCODER_A_PIN (1 << PD2)
#define ENCODER_A_PORT PORTD
#define ENCODER_A_DDR DDRD

#define ENCODER_B_PIN (1 << PD3)
#define ENCODER_B_PORT PORTD
#define ENCODER_B_DDR DDRD

#define BUZZER_PIN (1 << PD4)
#define BUZZER_PORT PORTD
#define BUZZER_DDR DDRD

#define RELAY_PIN (1 << PD1)
#define RELAY_PORT PORTD
#define RELAY_DDR DDRD

#define KEYS_PIN (1 << PC0)
#define KEYS_MUX 0
#define KEYS_PORT PORTC
#define KEYS_DDR DDRC

#define SENSOR_1_PIN (1 << PC1)
#define SENSOR_1_MUX (1<<MUX0)
#define SENSOR_1_PORT PORTC
#define SENSOR_1_DDR DDRC

#define SENSOR_2_PIN (1 << PC2) 
#define SENSOR_2_MUX (1<<MUX1)
#define SENSOR_2_PORT PORTC
#define SENSOR_2_DDR DDRC

// Some params
#define HYSTERESIS 5
#define TEMP_PROPOSAL 180
#define TIME_PROPOSAL 5*60
#define MAX_TEMP 250
#define MIN_TEMP -30

// Uncomment if want to use second sensor
// Displayed temp becomes average of two sensors 
//#define USE_SENSOR2

// Do not tinker with those bellow

//modes 
#define MODE_TMP_SEL 0
#define MODE_TIME_SEL 1
#define MODE_TMP_DSP 2
#define MODE_TIME_DSP 3

/* macros */
#define SetPin(Port, Pin)    Port |= Pin
#define ClearPin(Port, Pin)    Port &= ~Pin
#define TogglePin(Port, Pin)   Port ^= Pin;

#define SetPinNumber(Port, Bit)    Port |= (1 << Bit)
#define ClearPinNumber(Port, Bit)    Port &= ~(1 << Bit)