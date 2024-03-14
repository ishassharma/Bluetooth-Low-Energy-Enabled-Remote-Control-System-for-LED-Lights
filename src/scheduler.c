/**
 * @file scheduler.c
 * @author Isha Sharma
 * @date 25-Sep-2023
 * @brief  All Scheduler functions are defined in this file
**/

#include "scheduler.h"
#include "gpio.h"
#include "math.h"
#include <em_common.h>
#include "lcd.h"
#include "ble.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

uint16_t event_processing_bitmask =0;
uint16_t button_state =0;

void schedulerSetEvent_PB0(void)
{
  // enter critical section
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();// enter critical, turn off interrupts in NVIC

  sl_bt_external_signal(evt_pb0_pressed);

  // exit critical section
  CORE_EXIT_CRITICAL();// exit critical, re-enable interrupts in NVIC
}

void schedulerSetEvent_Keypad_EVEN(void)
{
  // enter critical section
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();// enter critical, turn off interrupts in NVIC

  sl_bt_external_signal(evt_button_even_pressed);

  // exit critical section
  CORE_EXIT_CRITICAL();// exit critical, re-enable interrupts in NVIC
}

void schedulerSetEvent_Keypad_ODD( void)
{
  // enter critical section
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_CRITICAL();// enter critical, turn off interrupts in NVIC

    sl_bt_external_signal(evt_button_odd_pressed);

    // exit critical section
    CORE_EXIT_CRITICAL();// exit critical, re-enable interrupts in NVIC

}

//state machine to control the sequencing of taking a temperature measurement where the state machine is driven by events.
void keypad_server_state_machine (sl_bt_msg_t *evt)
{
  static Keypad_State currentState= stateIDLE; // DOS

  ble_data_struct_t *ble_data = getBleDataPtr();
  if((ble_data->connection_open) && (SL_BT_MSG_ID(evt->header) == sl_bt_evt_system_external_signal_id))
     {
    switch(currentState)
    {
      case stateIDLE://state0

        if((evt->data.evt_system_external_signal.extsignals == evt_button_even_pressed)||(evt->data.evt_system_external_signal.extsignals == evt_button_odd_pressed))
          {
            //LOG_INFO("To1\n");
            //advance state
            currentState = stateKEYPAD_TO_GATT;
          }
        break;

      case stateKEYPAD_TO_GATT://state1 to gatt
        if(ble_data->ok_to_send_htm_notif)
        {
          currentState = stateIDLE;
          displayPrintf(DISPLAY_ROW_TEMPVALUE, "Handling notif");
        }
        break;

      default: break;

    }
 }
 }

//state machine to control the sequence of service and characteristic discovery
void discovery_state_machine (sl_bt_msg_t *evt)
{
  ble_data_struct_t *ble_data = getBleDataPtr();

  //current_state = current_state;
  static client_states_t current_state = stateOPEN ; // DOS initial state after reset

  sl_status_t sc;

  switch(current_state)
  {
    case stateOPEN:
      //current_state = stateOPEN; // default
      //LOG_INFO("in1\n\r");

      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_opened_id)
        {
          //LOG_INFO("in1.b\n\r");
          ble_data->connection_handle = evt->data.evt_connection_opened.connection;
          sc = sl_bt_gatt_discover_primary_services_by_uuid(ble_data->connection_handle,
                                                            sizeof(Keypad_service_uuid),//uuid_len Length of data in @p uuid
                                                            (const uint8_t*)Keypad_service_uuid); //uuid Service UUID in little endian format
          if(sc!= SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_gatt_discover_primary_services_by_uuid() returned != 0 status=0x%04x", (unsigned int) sc);
            }
          else
          {
          //LOG_INFO("noerror1\n\r");
          current_state = stateDISCOVER_KEYPAD_SERVICES;
          LOG_INFO("To1");
          }
        }
      break;

    case stateDISCOVER_KEYPAD_SERVICES:
      //LOG_INFO("in2\n\r");
      if (SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
        {

          sc = sl_bt_gatt_discover_primary_services_by_uuid(ble_data->connection_handle,
                                                            sizeof(LED_service_uuid),//uuid_len Length of data in @p uuid
                                                            (const uint8_t*)LED_service_uuid); //uuid Service UUID in little endian format
          if(sc!= SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_gatt_discover_primary_services_by_uuid() returned != 0 status=0x%04x", (unsigned int) sc);
            }
          current_state=stateDISCOVER_LED_SERVICES;
          LOG_INFO("To2");
        }
      else if(SL_BT_MSG_ID(evt->header) ==sl_bt_evt_connection_closed_id )
              {
                current_state = stateOPEN;
              }
      break;


    case stateDISCOVER_LED_SERVICES:
      //current_state = stateDISCOVER_SERVICES; // default
      //LOG_INFO("in3\n\r");
      if (SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
        {

          sc = sl_bt_gatt_discover_characteristics_by_uuid(ble_data->connection_handle,
                                                           ble_data->Keypad_service_handle,
                                                           sizeof(Keypad_charac_uuid), //uuid_len Length of data in @p uuid
                                                           (const uint8_t*) Keypad_charac_uuid);//uuid Characteristic UUID in little endian format

          if(sc!= SL_STATUS_OK)
             {
               LOG_ERROR("sl_bt_gatt_discover_characteristics_by_uuid() returned != 0 status=0x%04x", (unsigned int) sc);
             }
          current_state= stateDISCOVER_KEYPAD_CHARACTERISTICS;
          LOG_INFO("To3");
        }
      else if(SL_BT_MSG_ID(evt->header) ==sl_bt_evt_connection_closed_id )
        {
          current_state = stateOPEN;
        }
      break;

    case stateDISCOVER_KEYPAD_CHARACTERISTICS:
      //current_state = stateDISCOVER_SERVICES; // default
      //use event directly
      //LOG_INFO("in4\n\r");
      if (SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
        {

          sc = sl_bt_gatt_discover_characteristics_by_uuid(ble_data->connection_handle,
                                                           ble_data->LED_service_handle,
                                                           sizeof(LED_charac_uuid), //uuid_len Length of data in @p uuid
                                                           (const uint8_t*) LED_charac_uuid);//uuid Characteristic UUID in little endian format

          if(sc!= SL_STATUS_OK)
             {
               LOG_ERROR("sl_bt_gatt_discover_characteristics_by_uuid() returned != 0 status=0x%04x", (unsigned int) sc);
             }
          current_state= stateDISCOVER_LED_CHARACTERISTICS;
          LOG_INFO("To4");
        }
      else if(SL_BT_MSG_ID(evt->header) ==sl_bt_evt_connection_closed_id )
        {
          current_state = stateOPEN;
        }
      break;

    case stateDISCOVER_LED_CHARACTERISTICS:
      //LOG_INFO("in5\n\r");
      if (SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
      {

          sc = sl_bt_gatt_set_characteristic_notification (ble_data->connection_handle,
                                                           ble_data->Keypad_characteristics_handle,
                                                           sl_bt_gatt_notification
                                                           );
          if(sc!= SL_STATUS_OK)
         {
           LOG_ERROR("sl_bt_gatt_set_characteristic_notification() returned != 0 status=0x%04x", (unsigned int) sc);
         }
         displayPrintf(DISPLAY_ROW_TEMPVALUE, "Handling notif");
         current_state= stateKEYPADNOTIF;
         LOG_INFO("To5");
      }
      else if(SL_BT_MSG_ID(evt->header) ==sl_bt_evt_connection_closed_id )
      {
        current_state = stateOPEN;
      }
      break;

    case stateKEYPADNOTIF:
      //LOG_INFO("in6\n\r");
      if (SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
           {

             ble_data->button_notification_flag=true;
             displayPrintf(DISPLAY_ROW_TEMPVALUE, "Handling notif");
             current_state= stateWAIT;
             LOG_INFO("To6");
           }
      else if(SL_BT_MSG_ID(evt->header) ==sl_bt_evt_connection_closed_id )
           {
             current_state = stateOPEN;
           }
      break;

    case stateWAIT:
      //LOG_INFO("in7\n\r");
      current_state=stateWAIT;

      if(SL_BT_MSG_ID(evt->header) ==sl_bt_evt_connection_closed_id )
       {
         current_state = stateOPEN;
         LOG_INFO("To0");
       }

      break;

    default: break;

  }

}

