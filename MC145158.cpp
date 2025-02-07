/*
  Lib: PLL MC145158
  Version: 1.0.3
  Date: 2025/02/07
  Author: Junon M
*/

#include "MC145158.h"

#define DEBUG   1
//#define EXAMPLE_DATA  1 

MC145158::MC145158() {

  // Default config for dip switch pins (0 to 7)
  for (int i = 0; i < SW_NUM_BITS; i++) _sw_pins[i] = i;

  _dip_sw_value = 0;

  _freq = 10.0f; // MHz (10MHz)
  _freq_shift = 0.0f; // MHz (0)
  _prescaler = 1.0f; // Ex: [div by 8 LB3500] (1)
  _xtal = 10000.0f; // KHz (10MHz)
  _phase_det_freq = 0.5f; // KHz (500Hz)
}


MC145158::~MC145158() {

  digitalWrite(_le_pin, LOW);
  digitalWrite(_clock_pin, LOW);
  digitalWrite(_data_pin, LOW);

  pinMode(_clock_pin, INPUT);
  pinMode(_data_pin, INPUT);
  pinMode(_le_pin, INPUT);
}


void MC145158::begin(const uint8_t clock_pin, const uint8_t data_pin, const uint8_t le_pin) {

  _clock_pin  = clock_pin;
  _data_pin  = data_pin;
  _le_pin = le_pin;

  pinMode(_clock_pin, OUTPUT);
  pinMode(_data_pin, OUTPUT);
  pinMode(_le_pin, OUTPUT);

  digitalWrite(_le_pin, LOW);
  digitalWrite(_clock_pin, LOW);
  digitalWrite(_data_pin, LOW);
}


void MC145158::setDipSwPinout(const uint8_t b7, const uint8_t b6, const uint8_t b5, const uint8_t b4, 
const uint8_t b3, const uint8_t b2, const uint8_t b1, const uint8_t b0) 
{
_sw_pins[7] = b7;
_sw_pins[6] = b6;
_sw_pins[5] = b5;
_sw_pins[4] = b4;
_sw_pins[3] = b3;
_sw_pins[2] = b2;
_sw_pins[1] = b1;
_sw_pins[0] = b0;

for (int i = 0; i < SW_NUM_BITS; i++) pinMode(_sw_pins[i], INPUT_PULLUP);

}


void MC145158::bitDelay()
{
  delay(1);
}


void MC145158::sendData(uint32_t data, uint8_t len)
{
  uint32_t D = data;
    
  for (int i = len-1; i >= 0; i--) {

    if (bitRead(D, i) == HIGH){
      digitalWrite (_data_pin, HIGH);
#ifdef DEBUG
      Serial.print("1");
#endif
    }else{
      digitalWrite (_data_pin, LOW);
#ifdef DEBUG
      Serial.print("0");
#endif
    }

    MC145158::bitDelay();
    digitalWrite (_clock_pin, HIGH); // rising edge latches data
    MC145158::bitDelay();
    digitalWrite (_clock_pin, LOW);
  }

  digitalWrite (_data_pin, HIGH);

#ifdef DEBUG
  Serial.print(F(", HEX="));
  Serial.print(D, HEX);
  Serial.print(F(", DEC="));
  Serial.print(D);
  Serial.println();
#endif

}


void MC145158::pulseLe(void)
{
  digitalWrite (_le_pin, HIGH);
  MC145158::bitDelay();
  digitalWrite (_le_pin, LOW);
}


void MC145158::config(float f_shift_MHz, float prescaler, float xtal_KHz, float phase_det_freq_KHz)
{
  _freq_shift = f_shift_MHz;
  _prescaler = prescaler;
  _xtal = xtal_KHz;
  _phase_det_freq = phase_det_freq_KHz;
}


void MC145158::setFrequency(float MHz)
{
  _freq = MHz;
  MC145158::commitConfig();
}


void MC145158::setFreqShift(float MHz)
{
  _freq_shift = MHz;
  MC145158::commitConfig();
}


void MC145158::setFrequencyByDipSw()
{
  for (int i = 0; i < SW_NUM_BITS; i++)
  {
    _dip_sw_value = bitWrite(_dip_sw_value, i, !digitalRead(_sw_pins[i])); 
  }

  // freq = 1080 - 255; // = 825
  float freq = FACTOR - _dip_sw_value;
  freq /= 10; // = 82,5 MHz;
  MC145158::setFrequency(freq);
}


void MC145158::commitConfig()
{
  _R_counter = _xtal / _phase_det_freq; 

  float _f = _freq;
  _f += _freq_shift; // FI
  _f *= 1000.0f; // MHz to KHz 
  _f /= _prescaler;
  _f /= _phase_det_freq;

  uint32_t _d = _f;

  // copies the seven most significant bits after the tenth bit
  _A_counter = (_d >> 10) & 0x7F; // 0x7F = 0b1111111

  // copy only the first 10 bits
  _N_counter = (_d & 0x3FF); // 0x3FF = 0b1111111111

#ifdef DEBUG
   Serial.println();
   Serial.println(F("Calculado:"));
   Serial.print(F("A+N=")); Serial.println(_d);
   Serial.print(F("A=0x")); Serial.print(_A_counter, HEX); Serial.print(F(", A=")); Serial.print(_A_counter);
   Serial.print(F(", N=0x")); Serial.print(_N_counter, HEX); Serial.print(F(", N=")); Serial.println(_N_counter);
   Serial.print(F("R=")); Serial.println(_R_counter);
   Serial.println();
#endif

  // Send "(_N_counter << 8)|(_A_counter << 1)", 17 bits, MSB first, LSB always zero
#ifdef DEBUG
  Serial.println(F("Conteúdo dos Registradores:"));
  Serial.print("N+A = ");
#endif
  MC145158::sendData((_N_counter << 8) | (_A_counter << 1), 18);
  MC145158::pulseLe(); // Latch it

  // Send "(_R_counter << 1) | 1", 14 bits, MSB first, LSB always one
#ifdef DEBUG
  Serial.print("R   =    ");
#endif
  MC145158::sendData((_R_counter << 1) | 1, 15);
  MC145158::pulseLe(); // Latch it


#ifdef EXAMPLE_DATA
  Serial.println();
  Serial.println(F("Dados de Exemplo:"));
  Serial.print("N+A = ");
  MC145158::sendData(0b010000000100000000, 18);
  MC145158::pulseLe();
  Serial.print("R   =    ");
  MC145158::sendData(0b100001000010001, 15);
  MC145158::pulseLe();
  Serial.println();
#endif

}
