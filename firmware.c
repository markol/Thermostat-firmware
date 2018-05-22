#include <avr/io.h>
#include "definitions.h"
#include <avr/interrupt.h>
#include "thermistor.h"

#define START_BUZZER TCCR2 |= (1<<CS21) | (1<<CS22); // CTC 256 divide
#define STOP_BUZZER  TCCR2 &= 0B11111000; ClearPin(BUZZER_PORT, BUZZER_PIN);

//GLOBALS
//logic vars
volatile int16_t desired_temp = TEMP_PROPOSAL, 
    temp1, temp2;
uint16_t remaining_sec = 0, 
    desired_sec = TIME_PROPOSAL;

// ADC
volatile uint16_t adc_sums_tmp[3]= {0,0,0}, 
    adc_sums[3] = {0,0,0};
volatile uint8_t adc_samples_cnt[3] = {0,0,0};
const uint8_t mux_table[3] = {KEYS_MUX, SENSOR_1_MUX, SENSOR_2_MUX};

//flags and states
volatile uint8_t disp_state = MODE_TMP_DSP, 
    keys_enable = 1, 
    ticks = 0, 
    current_mux = 0, 
    buzzer_ticks = 0; 

// display vars
volatile uint8_t current_display_number[3];
volatile uint8_t current_display_digit = 0;
uint8_t digits_table[12][7] = 
{
    /* 0 */{1, 1, 1, 1, 1, 1, 0},
    /* 1 */{0, 1, 1, 0, 0, 0, 0},
    /* 2 */{1, 1, 0, 1, 1, 0, 1},
    /* 3 */{1, 1, 1, 1, 0, 0, 1},
    /* 4 */{0, 1, 1, 0, 0, 1, 1},
    /* 5 */{1, 0, 1, 1, 0, 1, 1},
    /* 6 */{1, 0, 1, 1, 1, 1, 1},
    /* 7 */{1, 1, 1, 0, 0, 0, 0},
    /* 8 */{1, 1, 1, 1, 1, 1, 1},
    /* 9 */{1, 1, 1, 1, 0, 1, 1},
    /*   */{0, 0, 0, 0, 0, 0, 0},
    /* - */{0, 0, 0, 0, 0, 0, 1},
};

int main(void)
{
    //setup ports directions
    //outs
    DIGIT_A_DDR |= DIGIT_A_PIN;
    DIGIT_B_DDR |= DIGIT_B_PIN;
    DIGIT_C_DDR |= DIGIT_C_PIN;
    DIGIT_D_DDR |= DIGIT_D_PIN;
    DIGIT_E_DDR |= DIGIT_E_PIN;
    DIGIT_F_DDR |= DIGIT_F_PIN;
    DIGIT_G_DDR |= DIGIT_G_PIN;
    DIGIT_1_DDR |= DIGIT_1_PIN;
    DIGIT_2_DDR |= DIGIT_2_PIN;
    DIGIT_3_DDR |= DIGIT_3_PIN;
    
    BUZZER_DDR |= BUZZER_PIN;
    RELAY_DDR |= RELAY_PIN;
    
    //ins
    ENCODER_A_DDR &= ~ENCODER_A_PIN;
    ENCODER_B_DDR &= ~ENCODER_B_PIN;
    SENSOR_1_DDR &= ~SENSOR_1_PIN;
    SENSOR_2_DDR &= ~SENSOR_2_PIN;
    KEYS_DDR &= ~KEYS_PIN;
    
    //setup important outs state
    //low
    ClearPin(BUZZER_PORT, BUZZER_PIN);
    ClearPin(RELAY_PORT, RELAY_PIN);
    //high
    SetPin(DIGIT_1_PORT, DIGIT_1_PIN);
    SetPin(DIGIT_2_PORT, DIGIT_2_PIN);
    SetPin(DIGIT_3_PORT, DIGIT_3_PIN);

    
    //timer setup
    TCCR0 |= (1<<CS00) | (1<<CS01); // prescaler division 64  
    
    //OCR1A = 7200; // 1/4s for 7.37MHz crystal
    OCR1A = 3600; // 1/8s for 7.37MHz crystal
    TCCR1B |= (1 << WGM12) // Mode 4
        |(1 << CS12); // set prescaler to 256 and start the timer
    
    TCCR2 |= (1<<WGM21); // CTC
    OCR2 = 4; // 7200 Hz
    
    //Set interrupt on compare match
    TIMSK |= (1<<TOIE0) | (1 << OCIE1A) | (1<<OCIE2);   //Overflow timer INT and  2x compare math 
 
    // externally trigged interrupts setup    
    MCUCR |= (1<<ISC01)|(1<<ISC11)|(1<<ISC10); /* INT0 - falling edge, INT1 - raising edge */
    //MCUCR |= (1<<ISC00)|(1<<ISC10); /* INT0 - any change, INT1 - any change */
    GICR |= (1<<INT0)|(1<<INT1);        /* enable INT0 and INT1 */
    
    
    // ADC setup
    ADCSRA =   (1<<ADEN)
    |(1<<ADIE) // enable interrupt
    |(1<<ADSC) // start single conversion
    |(1<<ADPS0)  //ADPS2:0: division = 128  
    |(1<<ADPS1)  
    |(1<<ADPS2);   
    
    ////ADMUX = (1<<REFS1) | (1<<REFS0) | mux_table[current_mux]; //Internal 2.56V Voltage Reference with external capacitor at AREF pin  
    ADMUX  =  (1<<REFS0) | mux_table[0]; //REFS1:0: Reference Selection Bits, ADC0

    sei();
    
   while(1)
   {
        temp1 = calc_temperature(adc_sums[1])/10;
#ifdef USE_SENSOR2
        temp2 = calc_temperature(adc_sums[2])/10;
        temp1 = (temp1 + temp2) / 2;
#endif
        
   }
   return 0;
}

void handle_keys()
{
    uint16_t keys = adc_sums[0]/ADC_SAMPLES;
    if(keys_enable)
    {
        // third button (not used) is: if(keys > 800)
        if(keys > 700) //start button pressed
        {
            START_BUZZER;
            keys_enable = 0;
            if(remaining_sec)
            {
                // stop by cleaning counter
                remaining_sec = 0;
                // turn off heater
                ClearPin(RELAY_PORT, RELAY_PIN);
            }
            else 
            {
                // start by setting counter above zero
                remaining_sec = desired_sec;
                disp_state = MODE_TMP_DSP;
            }
        }
        else if(keys > 500) // mode button pressed, switch between time and temp adjusting
        {
            START_BUZZER;
            keys_enable = 0;
            if(disp_state ==  MODE_TMP_SEL)
                if(remaining_sec)
                    disp_state = MODE_TMP_DSP;
                else
                    disp_state = MODE_TIME_SEL;
            else if(disp_state ==  MODE_TIME_SEL)
                disp_state = MODE_TMP_DSP;
            else if(disp_state ==  MODE_TMP_DSP)
                if(remaining_sec)
                    disp_state = MODE_TIME_DSP;
                else
                    disp_state = MODE_TMP_SEL;
            else 
                disp_state = MODE_TMP_SEL;
        }
    }
    else if(keys < 1)
        keys_enable = 1;
}

void display(int16_t value)
{
    current_display_number[2] = (value) % 10;
    current_display_number[1] = (value / 10) % 10;
    if(value < 0)
    {
        current_display_number[0] = 11;
        current_display_number[1] *= -1;
        current_display_number[2] *= -1;
    }
    else
    {
        current_display_number[0] = (value / 100) % 10;
        // do not display leading zeros
        if(current_display_number[0] == 0 && value < 1000)
        {
            current_display_number[0] = 10;
            if(current_display_number[1] == 0)
                current_display_number[1] = 10;
        }
    } 
}

void temp_reg_bangbang()
{
    //turn on heater
    if(temp1 < (desired_temp - HYSTERESIS))
    {
        SetPin(RELAY_PORT, RELAY_PIN);
    }
    else if(temp1 > desired_temp)
        ClearPin(RELAY_PORT, RELAY_PIN);
}

// called on every encoder action
void set_param(int8_t val)
{
    if(disp_state == MODE_TIME_SEL)
    {
        desired_sec += val*10;
        if(desired_sec < 10) // minimal time 10 sec
            desired_sec = 10;
    }
    else if(disp_state == MODE_TMP_SEL)
    {
        desired_temp += val;
        if(desired_temp > MAX_TEMP)
            desired_temp = MAX_TEMP;
        else if(desired_temp < MIN_TEMP)
            desired_temp = MIN_TEMP;
    }
}

// interrupts for rotary encoder
ISR(INT0_vect ) 
{ 
    int8_t jump = 0;
    
    if(PIND & ENCODER_B_PIN) 
        ++jump;
     
    else
        --jump;
    
    set_param(jump);
} 

ISR(INT1_vect ) 
{ 
    int8_t jump = 0;
    
    if(PIND & ENCODER_A_PIN) 
        ++jump;
     
    else
        --jump;
    
    set_param(jump);
}


// timer interrupt for buzzer
ISR (TIMER2_COMP_vect)
{
    TogglePin(BUZZER_PORT, BUZZER_PIN);
}

// timer interrupt for digit display
ISR(TIMER0_OVF_vect)   
{
    uint8_t *current_table_entry = digits_table[current_display_number[current_display_digit]];
    // not pretty but efficient solution because of avoiding passing port as parameter
    // turn off all digits 
    SetPin(DIGIT_1_PORT, DIGIT_1_PIN);
    SetPin(DIGIT_2_PORT, DIGIT_2_PIN);
    SetPin(DIGIT_3_PORT, DIGIT_3_PIN);
    
    // set current digit
    if(current_table_entry[0]) ClearPin(DIGIT_A_PORT, DIGIT_A_PIN); else SetPin(DIGIT_A_PORT, DIGIT_A_PIN);
    if(current_table_entry[1]) ClearPin(DIGIT_B_PORT, DIGIT_B_PIN); else SetPin(DIGIT_B_PORT, DIGIT_B_PIN);
    if(current_table_entry[2]) ClearPin(DIGIT_C_PORT, DIGIT_C_PIN); else SetPin(DIGIT_C_PORT, DIGIT_C_PIN);
    if(current_table_entry[3]) ClearPin(DIGIT_D_PORT, DIGIT_D_PIN); else SetPin(DIGIT_D_PORT, DIGIT_D_PIN);
    if(current_table_entry[4]) ClearPin(DIGIT_E_PORT, DIGIT_E_PIN); else SetPin(DIGIT_E_PORT, DIGIT_E_PIN);
    if(current_table_entry[5]) ClearPin(DIGIT_F_PORT, DIGIT_F_PIN); else SetPin(DIGIT_F_PORT, DIGIT_F_PIN);
    if(current_table_entry[6]) ClearPin(DIGIT_G_PORT, DIGIT_G_PIN); else SetPin(DIGIT_G_PORT, DIGIT_G_PIN);
    
    //display properly set digit
    if(current_display_digit == 0)
    {
        ClearPin(DIGIT_1_PORT, DIGIT_1_PIN);
        current_display_digit = 1;
    }
    else if(current_display_digit == 1)
    {
        ClearPin(DIGIT_2_PORT, DIGIT_2_PIN);
        current_display_digit = 2;
    }
    else
    {
        ClearPin(DIGIT_3_PORT, DIGIT_3_PIN);
        current_display_digit = 0;
    }
}

// adc conversion finish interrupt
ISR(ADC_vect)
{
    adc_sums_tmp[current_mux] += ADC;
    if(++adc_samples_cnt[current_mux] > ADC_SAMPLES)
    {
        adc_sums[current_mux] = adc_sums_tmp[current_mux];
        adc_sums_tmp[current_mux] = 0;
        adc_samples_cnt[current_mux] = 0;
    }

    if(++current_mux > 2)
        current_mux = 0;   
    
    ADMUX = (1<<REFS0) | mux_table[current_mux];
    // start new conversion
    ADCSRA |= (1<<ADSC);
}

// timer interrupt for some updates, called every 1/8 sec
ISR (TIMER1_COMPA_vect)
{
    if(!buzzer_ticks)
    {   STOP_BUZZER;}
    else
        --buzzer_ticks;
    
    if(++ticks > 8)
    { 
        // every second
        //update digits to display
        if(remaining_sec)
        {
            temp_reg_bangbang();
            --remaining_sec;
            if(!remaining_sec)
            {
                buzzer_ticks = 8*5; // keep buzzer on for 5 sec
                START_BUZZER;
                ClearPin(RELAY_PORT, RELAY_PIN);
            }
        }
        ticks = 0;
    }
    //display(adc_sums[1]/ADC_SAMPLES);
    if(disp_state == MODE_TMP_DSP)
        display(temp1);
    else if(disp_state == MODE_TIME_DSP)
        display(remaining_sec);
    else if(disp_state == MODE_TMP_SEL)
        display(desired_temp);
    else display(desired_sec);
    handle_keys();
}
