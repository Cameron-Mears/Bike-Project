#include "Bike.h"
#include "LiquidCrystal.h"
const float WHEEL_CIRCUMFRENCE_KM;
const float WHEEL_CIRCUMFRENCE_M;
const uint8_t FLASHING = 2;
const uint8_t OFF = 0;
const uint8_t STATIC_ON = 1;
const uint8_t number_of_lights = 4;
const uint8_t hall_sensor;
const uint8_t left_button;
const uint8_t right_button;
const uint8_t menu_button;
const uint8_t menu_up;
const uint8_t menu_down;
const uint8_t rotary_press;

uint64_t time_last = 0;
uint64_t vel_time_last = 0;
uint8_t velocity = 0;
uint8_t in_menu = 0;

menu *bike_menu;

float (*vel_func)(void);

light *rear, *left, *right, *front;
light *lights[4] = {front, rear, left, right};

LiquidCrystal lcd(0,1,2,3,4,5,6);

void setup(void)
{
    rear = (light *) malloc(sizeof(light));

        rear->mode = STATIC_ON;
        rear->pin = 1;
        rear->state = 0;
        rear->light_delta = 250;
        rear->next_event = 0;

    front = (light *) malloc(sizeof(light));

        front->mode = STATIC_ON;
        front->pin = 2;
        front->state = 0;
        front->light_delta = 250;
        front->next_event = 0;

    right = (light *) malloc(sizeof(light));

        right->mode = OFF;
        right->pin = 3;
        right->state = 0;
        right->light_delta = 500;
        right->next_event = 0;

    left = (light *) malloc(sizeof(light));

        left->mode = OFF;
        left->pin = 4;
        left->state = 0;
        left->light_delta = 500;
        left->next_event = 0;
        
    bike_menu = (menu *) malloc(sizeof(menu));
    for (int8_t index = 0; index < MENU_OPTIONS; index ++)
    {
      bike_menu->options[index] = malloc(MENU_STRING_LEN); //alot characters per menu option title
    }

    bike_menu->options[0] = "Cycle Front Light";
    bike_menu->functions[0] = &cycle_front;
    bike_menu->options[1] = "Cycle rear Light";
    bike_menu->functions[1] = &cycle_rear;
    bike_menu->options[2] = "Change speed display";
    bike_menu->functions[2] = &change_vel_func;
    bike_menu->options[3] = "Exit menu";
    bike_menu->functions[3] = &stop_menu;
    
    
    
    
    
    pinMode(hall_sensor, INPUT);
    vel_func = &calc_vel_km;
    time_last = millis();
};//-------------------------------END OF SETUP-------------------

void loop(void)
{
    if (digitalRead(rotary_press)) start_menu(); 
    print_display();
    update_lights();
    if (digitalRead(right_button)) signal_right();
    if (digitalRead(left_button)) signal_left();   

};

void print_menu(void)
{
  for (uint8_t index = 0; index < MENU_OPTIONS; index ++)
  {
    if (bike_menu->current_selection == index)
    {
      print_string(bike_menu->options[index], MENU_STRING_LEN);
      lcd.print("<-\n");
    }
    else
    {
      print_string(bike_menu->options[index], MENU_STRING_LEN);
      lcd.print("\n");
    }
  }
}

void print_string(char *string, uint8_t len)
{
  for (uint8_t index = 0; index < len; index ++)
  {
    char temp = *(string + index);
    if (temp != "\0")
    {
      lcd.print(temp);
    }
    else return;
  }
  
}

void print_time(void)
{
  uint64_t now = millis();
  uint8_t hours = now % (3600000);
  now -= (hours * 3600000);
  uint8_t minutes = now % 60000;
  now -= minutes * 60000;
  uint8_t seconds = now % 1000;
  lcd.print("UpTime:  ");
  lcd.print(hours);
  lcd.print(":");
  lcd.print(minutes);
  lcd.print(":");
  lcd.print(seconds);
  lcd.print("\n");
  
}

void print_display(void)
{
  float vel = vel_func();
  lcd.print("Speed: ");
  lcd.print(vel);
  lcd.print((vel_func == &calc_vel_m)? "m/s":"km/h");
  lcd.print("\n");
  lcd.print("Signal: ");
  if (right->state) lcd.print("->\n");
  else if (left->state) lcd.print("<-\n");
  print_time();
  
}

float calc_vel_km(void)
{
    uint64_t now = millis();
    uint16_t delta = now - time_last;
    time_last = now;
    return WHEEL_CIRCUMFRENCE_KM/ (delta/3600000); //return velocity in kmph
};

float calc_vel_m(void)
{
  uint64_t now = millis();
    uint16_t delta = now - time_last;
    time_last = now;
    return WHEEL_CIRCUMFRENCE_M/ (delta/1000); //return velocity in kmph
};

void change_vel_func(void)
{
  if (vel_func == &calc_vel_m) vel_func = &calc_vel_km;
  else vel_func = &calc_vel_km;
}

void signal_left(void)
{
    signal_light(left);
};

void signal_right(void)
{
    signal_light(right);
}

void signal_light(light *l)
{
    if (l->mode == FLASHING)
    {
        l->mode = OFF;
        l->state = 0;
        digitalWrite(l->pin, LOW);
    }
    else if (l->mode == OFF)
    {
      l->mode = FLASHING;
      l->next_event = millis() + l->light_delta;
    }
};

void cycle_front(void)
{
  cycle_mode(front);
}

void cycle_rear(void)
{
  cycle_mode(rear);
}

void cycle_mode(light *l)
{
    l->mode = (l->mode ++) % FLASHING;
    if (l->mode == FLASHING)
    {
      l->mode = FLASHING;
      l->next_event = millis() + l->light_delta;
    }
    else if (l->mode == STATIC_ON)
    {
      digitalWrite(l->pin, HIGH);
    }
    else
    {
      digitalWrite(l->pin, LOW);
    }
}

void stop_menu(void)
{
  in_menu = 0;
}

void start_menu(void)
{
    uint64_t now = millis();
    uint64_t last = now;
    uint8_t selection = 0;
    
    in_menu = 1;
    while (in_menu)
    {
        int8_t dir = (digitalRead(menu_up) - digitalRead(menu_down));
        bike_menu->current_selection += dir;
        if (bike_menu->current_selection > MENU_OPTIONS - 1) selection = 0;
        if (bike_menu->current_selection < 0) selection = MENU_OPTIONS - 1;

        print_menu();
        
        if (digitalRead(rotary_press))
        {
          bike_menu->functions[bike_menu->current_selection]();
        }

        update_lights();
    }
}


void update_lights(void)
{
    uint64_t now = millis();
    for (uint8_t index = 0; index < number_of_lights; index ++)
    {
        light *temp = lights[index];
        if (temp->mode == FLASHING)
        {
          if (temp->next_event <= now)
          {
            temp->state = !temp->state;
            temp->next_event += temp->light_delta;
            digitalWrite(temp->pin, temp->state);
          }
        }
    }
}


#include <stdio.h>

typedef struct Triangle Triangle;
typedef struct Line Line;

int nNumbers(int nRows)
{
	int sum = 0;
	for (int row = nRows; row > 0; row--)
	{
		sum += row;
	}

	return sum;
}

struct Triangle
{
	int nLines;
	int val;
};

int main(int argc, char *argv[]) {
	int baseSize;
	scanf

	return(0);
}

