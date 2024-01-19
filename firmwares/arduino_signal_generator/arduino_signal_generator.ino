#include <AD9833.h>
#include <KY040.h>
#include <LiquidCrystal.h>

//KY040 pins
constexpr uint8_t KY_PIN_CLK = 12;                         
constexpr uint8_t KY_PIN_DT = 9;                          
constexpr uint8_t KY_PIN_SW = 8; 

// LCD pins
constexpr uint8_t LCD_PIN_RS = 2;
constexpr uint8_t LCD_PIN_EN = 3;
constexpr uint8_t LCD_PIN_DB4 = 4;
constexpr uint8_t LCD_PIN_DB5 = 5;
constexpr uint8_t LCD_PIN_DB6 = 6;
constexpr uint8_t LCD_PIN_DB7 = 7;

//AD9833 pins
constexpr uint8_t AD_PIN_CS = 10;


enum frequency_unit
{
  FREQUENCY_UNIT_HZ=0,
  FREQUENCY_UNIT_KHZ=1,
  FREQUENCY_UNIT_MHZ=2,
  FREQUENCY_UNIT_ALL=3
};

struct {
  unsigned int freq_coeff;
  const char *freq_str;
}freq_data[] = 
{
  {1, "Hz"},
  {1000, "kHz"},
  {1000000, "MHz"}
}; 

enum wave_shape
{
  WAVE_SHAPE_SINE=0,
  WAVE_SHAPE_TRIANGLE=1,
  WAVE_SHAPE_SQUARE=2,
  WAVE_SHAPE_ALL=3,
};

struct
{
  uint8_t wave_shape_mode;
  const char *wave_name;
}wave_data[]=
{
  {WAVEFORM_SINE, "Sine"},
  {WAVEFORM_TRIANGLE, "Triangle"},
  {WAVEFORM_SQUARE, "Square"}
};

enum menu_item
{
  MENU_ITEM_WAVEFORM=0,
  MENU_ITEM_FREQUENCY_UNIT=1,
  MENU_ITEM_FREQUENCY=2,
  MENU_ITEM_MAX=3
};


void _update_frequency(float *frequency, enum frequency_unit freq_unit, KY040::RotateDirection direction)
{
  switch(direction)
  {
    case KY040::RotateDirection::CLOCKWISE_ROTATE_DIRECTION:
      *frequency += 1.0*freq_data[freq_unit].freq_coeff;
    break;
    case KY040::RotateDirection::COUNTER_CLOCKWISE_ROTATE_DIRECTION:
      if(*frequency <= 1.0)
        *frequency = 0.0;
      else
        *frequency -= 1.0*freq_data[freq_unit].freq_coeff;
    break;
    default:
    break;
  }
  
}

void _update_wave_shape(enum wave_shape *wave_shape, KY040::RotateDirection direction)
{
  int wave_shap_val = (int)(*wave_shape);
  switch(direction)
  {
    case KY040::RotateDirection::CLOCKWISE_ROTATE_DIRECTION:
      wave_shap_val = ((int)(wave_shap_val)+1)%WAVE_SHAPE_ALL;
    break;
    case KY040::RotateDirection::COUNTER_CLOCKWISE_ROTATE_DIRECTION:
      if(*wave_shape == WAVE_SHAPE_SINE)
        wave_shap_val = WAVE_SHAPE_SQUARE;
      else
        wave_shap_val--;
    break;
    default:
    break;
  }
  *wave_shape = (enum wave_shape)wave_shap_val;
}

void _update_freq_unit(enum frequency_unit *freq_unit, KY040::RotateDirection direction)
{
  int freq_unit_val = (int)(*freq_unit);
  switch(direction)
  {
    case KY040::RotateDirection::CLOCKWISE_ROTATE_DIRECTION:
      freq_unit_val = (freq_unit_val+1)%FREQUENCY_UNIT_ALL;
    break;
    case KY040::RotateDirection::COUNTER_CLOCKWISE_ROTATE_DIRECTION:
      if(freq_unit_val == FREQUENCY_UNIT_HZ)
        freq_unit_val = FREQUENCY_UNIT_MHZ;
      else
        freq_unit_val--;
    break;
    default:
    break;
  }

  *freq_unit = (enum frequency_unit)freq_unit_val;
}

void _update_display(LiquidCrystal *lcd, enum wave_shape wave_shape, enum frequency_unit freq_unit, float frequency)
{
  lcd->clear();
  lcd->setCursor(0, 0);
  String row = "Waveform: " + String(wave_data[wave_shape].wave_name);
  lcd->print(row);

  lcd->setCursor(0, 1);
  row = "Freq: " + String(frequency/freq_data[freq_unit].freq_coeff) + String(freq_data[freq_unit].freq_str);
  lcd->print(row);
  
}

void _update_generator(AD9833 *myGen, enum wave_shape wave_shape, float frequency)
{
  myGen->outputEnable(false);
  myGen->writeFrequency(FREQ0, frequency);            
  myGen->writeFrequency(FREQ1, frequency);
  myGen->writePhaseDeg(FREQ0, 0);                 
  myGen->setWaveform (wave_data[wave_shape].wave_shape_mode);
  myGen->selectFrequency(FREQ0);              
  myGen->outputEnable(true); 
}

unsigned long current_time;
float frequency = 0.0;
enum frequency_unit freq_unit = FREQUENCY_UNIT_HZ;
enum menu_item menu_item = MENU_ITEM_WAVEFORM;
enum wave_shape wave_shape = WAVEFORM_SINE;
KY040::RotateDirection direction = KY040::RotateDirection::NONE_ROTATE_DIRECTION;
KY040::ButtonState button_state = KY040::ButtonState::BUTTON_RELEASED;


KY040 encoder(KY_PIN_CLK, KY_PIN_DT, KY_PIN_SW);
LiquidCrystal lcd(LCD_PIN_RS, LCD_PIN_EN, LCD_PIN_DB4, LCD_PIN_DB5, LCD_PIN_DB6, LCD_PIN_DB7);
AD9833 myGen (AD_PIN_CS);


void setup() 
{
  current_time = millis();
  lcd.begin(16, 2);
  myGen.begin();  
  encoder.begin();

  _update_generator(&myGen, wave_shape, frequency);
  _update_display(&lcd, wave_shape, freq_unit, frequency);
}

void loop() 
{
  KY040::ButtonState button = encoder.check_button_state();
  if(button == KY040::ButtonState::BUTTON_PRESSED)
  {
    menu_item = (int(menu_item)+1)%MENU_ITEM_MAX;
  }

  KY040::RotateDirection dir = encoder.check_rotate_direction();
  if(dir != KY040::RotateDirection::NONE_ROTATE_DIRECTION)
  {
    switch(menu_item)
    {
      case MENU_ITEM_WAVEFORM:
        _update_wave_shape(&wave_shape, dir);
        _update_generator(&myGen, wave_shape, frequency);
      break;
      case MENU_ITEM_FREQUENCY_UNIT:
        _update_freq_unit(&freq_unit, dir);
      break;
      case MENU_ITEM_FREQUENCY:
         _update_frequency(&frequency, freq_unit, dir);
         _update_generator(&myGen, wave_shape, frequency);
      break;
      default:
      break;
    }

    _update_display(&lcd, wave_shape, freq_unit, frequency);

  }
  

  // put your main code here, to run repeatedly:
  delay(1);

}
