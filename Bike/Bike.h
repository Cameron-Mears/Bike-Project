#define MENU_OPTIONS 4
#define MENU_STRING_LEN 30

typedef struct light light;
typedef struct menu menu;

float calc_vel_km(void);
float calc_vel_m(void);
void cycle_light(light *l);
void cycle_rear(void);
void cycle_front(void);
void signal_right(void);
void signal_left(void);
void signal_light(light *l);
void print_menu(void);
void change_vel_func(void);
void print_string(char *string, uint8_t len);
void print_display(void);
void stop_menu(void);




struct light
{
    uint8_t mode;
    uint8_t pin;
    uint16_t light_delta;
    uint64_t next_event;
    uint8_t state; //1 on, 0 off
};

struct menu
{
    uint8_t current_selection;
    char *options[MENU_OPTIONS];
    void (*functions[MENU_OPTIONS]) (void);

};
