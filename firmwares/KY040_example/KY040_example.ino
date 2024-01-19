#include <Arduino.h> 
#include <KY040.h> 

#define PIN_CLK 12                         // Указываем к какому выводу CLK энкодер подключен к Arduino
#define PIN_DT 9                          // Указываем к какому выводу DT энкодер подключен к Arduino
#define PIN_SW 8                          // Указываем к какому выводу SW энкодер подключен к Arduino

KY040 encoder(PIN_CLK, PIN_DT, PIN_SW);
int counter = 0;

void setup() {
  encoder.begin();
  Serial.begin(9600); 
}

void loop() {
  KY040::RotateDirection dir = encoder.check_rotate_direction();
  switch(dir)
  {
    case KY040::RotateDirection::CLOCKWISE_ROTATE_DIRECTION:
    {
      counter++;
      Serial.println("Direction: clockwise");
    } 
    break;
    case KY040::RotateDirection::COUNTER_CLOCKWISE_ROTATE_DIRECTION:
    {
      counter--;
      Serial.print("Direction: counter clockwise");

    } 
    break;
    default:
    break;
  }

  if(dir != KY040::RotateDirection::NONE_ROTATE_DIRECTION)
  {
    Serial.print("Counter: ");
    Serial.println(counter);
  }

  KY040::ButtonState button = encoder.check_button_state();
  if(button == KY040::ButtonState::BUTTON_PRESSED)
  {
    Serial.println("Button pressed");
  }
  
  delay(1);      
  
}
