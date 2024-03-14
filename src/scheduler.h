/**
 * @file scheduler.h
 * @author Isha Sharma
 * @date 25-Sep-2023
 * @brief  header file for scheduler.c
**/

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_
#include <em_core.h>
#include "ble.h"

//enum for events for schedulers
typedef enum {
  evt_button_even_pressed = 1,
  evt_button_odd_pressed = 2,
  evt_pb0_pressed =4,
} myEvent;

//changed to default counting numbers
//enum for states
typedef enum {
stateIDLE,
stateKEYPAD_TO_GATT,
} Keypad_State;

typedef enum {
stateOPEN,
stateDISCOVER_KEYPAD_SERVICES,
stateDISCOVER_LED_SERVICES,
stateDISCOVER_KEYPAD_CHARACTERISTICS,
stateDISCOVER_LED_CHARACTERISTICS,
stateKEYPADNOTIF,
stateWAIT,
} client_states_t;

//function prototypes
void schedulerSetEvent_Keypad_EVEN(void);
void schedulerSetEvent_Keypad_ODD( void);
void schedulerSetEvent_PB0(void);

void keypad_server_state_machine(sl_bt_msg_t *evt);
void discovery_state_machine (sl_bt_msg_t *evt);

#endif /* SRC_SCHEDULER_H_ */
