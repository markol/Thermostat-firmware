//src: http://aterlux.ru/article/ntcresistor-en
// Visit to generat your own tables
// by Pogrebnyak Dmitry

#include "thermistor.h"

/* Table of ADC sum value, corresponding to temperature. Starting from higher value to lower.
   Next parameters had been used to build table:
     R1(T1): 100kOhm(25°С)
     R/T characteristics table used: EPCOS R/T:8403; B0/100:3970K
     Scheme: A
     Ra: 4.7kOhm
     U0/Uref: 5.05V/5V
*/
const temperature_table_entry_type termo_table[] PROGMEM = {
    65474, 65374, 65263, 65137, 64998, 64844, 64673, 64484,
    64275, 64046, 63795, 63520, 63220, 62893, 62538, 62153,
    61737, 61288, 60806, 60288, 59734, 59143, 58515, 57848,
    57143, 56398, 55615, 54793, 53935, 53038, 52107, 51141,
    50145, 49116, 48061, 46979, 45877, 44752, 43612, 42457,
    41293, 40121, 38946, 37768, 36595, 35425, 34265, 33115,
    31979, 30858, 29757, 28674, 27615, 26578, 25567, 24580,
    23622, 22690, 21787, 20911, 20066, 19248, 18459, 17698,
    16966, 16261, 15584, 14933, 14309, 13709, 13135, 12585,
    12058, 11553, 11070, 10609, 10167, 9745, 9341, 8956,
    8588, 8235, 7899, 7578, 7272, 6979, 6699, 6432,
    6177, 5933, 5700, 5478, 5266, 5063, 4869, 4683,
    4506, 4337, 4175, 4020, 3872, 3730, 3594, 3465,
    3340, 3221, 3108, 2999, 2894, 2794, 2698, 2606,
    2518, 2433, 2352, 2274, 2199, 2128, 2059, 1992,
    1929, 1868, 1809
};

// This function is calculating temperature in tenth of degree of Celsius
// depending on ADC sum value as input parameter.
int16_t calc_temperature(temperature_table_entry_type adcsum) {
  temperature_table_index_type l = 0;
  temperature_table_index_type r = (sizeof(termo_table) / sizeof(termo_table[0])) - 1;
  temperature_table_entry_type thigh = TEMPERATURE_TABLE_READ(r);
  
  // Checking for bound values
  if (adcsum <= thigh) {
    #ifdef TEMPERATURE_UNDER
      if (adcsum < thigh) 
        return TEMPERATURE_UNDER;
    #endif
    return TEMPERATURE_TABLE_STEP * r + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type tlow = TEMPERATURE_TABLE_READ(0);
  if (adcsum >= tlow) {
    #ifdef TEMPERATURE_OVER
      if (adcsum > tlow)
        return TEMPERATURE_OVER;
    #endif
    return TEMPERATURE_TABLE_START;
  }

  // Table lookup using binary search 
  while ((r - l) > 1) {
    temperature_table_index_type m = (l + r) >> 1;
    temperature_table_entry_type mid = TEMPERATURE_TABLE_READ(m);
    if (adcsum > mid) {
      r = m;
    } else {
      l = m;
    }
  }
  temperature_table_entry_type vl = TEMPERATURE_TABLE_READ(l);
  if (adcsum >= vl) {
    return l * TEMPERATURE_TABLE_STEP + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type vr = TEMPERATURE_TABLE_READ(r);
  temperature_table_entry_type vd = vl - vr;
  int16_t res = TEMPERATURE_TABLE_START + r * TEMPERATURE_TABLE_STEP; 
  if (vd) {
    // Linear interpolation
    res -= ((TEMPERATURE_TABLE_STEP * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
  }
  return res;
}