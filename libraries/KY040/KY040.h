/*
 Библиотека для работы с генератором сигналов AD9833 по интерфуйсу SPI
 Разработано Egor 'Nich1con' Zakharov
 V1.0 от 11.11.2020
*/

#pragma once
#include <Arduino.h>


class KY040 {
  public:
    enum class ButtonState
	{
		BUTTON_PRESSED=0,
		BUTTON_RELEASED=1
	};
	enum class RotateDirection
	{
		NONE_ROTATE_DIRECTION = 0,
		CLOCKWISE_ROTATE_DIRECTION=1,
		COUNTER_CLOCKWISE_ROTATE_DIRECTION=2
	};
	KY040(uint8_t clk_pin, uint8_t dt_pin, uint8_t sw_pin);								// Конструктор обьекта
    void begin(void);								// Инцииализация
    ButtonState check_button_state(void);
	RotateDirection check_rotate_direction(void);
   private:
    void writeFreqReg(bool reg, uint32_t data);
    void writePhaseReg(bool reg, uint16_t data);
    void writeCfgReg();
    void writeReg(uint16_t data);
    uint8_t _clk_pin = 0;
	uint8_t _dt_pin = 0;
	uint8_t _sw_pin = 0;
	int _last_clk_pin_state = 0;
	unsigned long _last_button_press_time = 0;
   
};

KY040::KY040(uint8_t clk_pin, uint8_t dt_pin, uint8_t sw_pin)
{
	_clk_pin = clk_pin;
	_dt_pin = dt_pin;
	_sw_pin = sw_pin;
}

void KY040::begin(void)
{
  pinMode(_clk_pin, INPUT);                // Указываем вывод CLK как вход
  pinMode(_dt_pin, INPUT);                 // Указываем вывод DT как вход
  pinMode(_sw_pin, INPUT_PULLUP);          // Указываем вывод SW как вход и включаем подтягивающий резистор
  _last_button_press_time = millis();
  _last_clk_pin_state = digitalRead(_clk_pin);
}

KY040::ButtonState KY040::check_button_state(void)
{
  int button_state = digitalRead(_sw_pin);	// Считываем состояние вывода SW
  unsigned long current_time = millis();
  
  if (button_state == LOW && (current_time-_last_button_press_time) > 500)
  {
	_last_button_press_time	= current_time;
	return ButtonState::BUTTON_PRESSED;
  }
  
  return ButtonState::BUTTON_RELEASED;
}

KY040::RotateDirection KY040::check_rotate_direction(void)
{
	RotateDirection res = RotateDirection::NONE_ROTATE_DIRECTION;
	int _current_clk_pin_state = digitalRead(_clk_pin); // Считываем значение с CLK
	int _current_dt_pin_state = digitalRead(_dt_pin);
	// Проверяем изменилось ли состояние CLK
	if (_current_clk_pin_state != _last_clk_pin_state)
	{
		switch(_current_clk_pin_state)
		{
			case LOW:
			{
				if (_current_dt_pin_state != _current_clk_pin_state)
					res = RotateDirection::COUNTER_CLOCKWISE_ROTATE_DIRECTION;
			}
			break;
			case HIGH:
			{
				if (_current_dt_pin_state == _current_clk_pin_state)
					res = RotateDirection::CLOCKWISE_ROTATE_DIRECTION;
			}
			break;
			default:
			break;
			
		}
	}
	_last_clk_pin_state = _current_clk_pin_state;
	return res;
}

