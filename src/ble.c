/**
 * @file ble.c
 * @author Isha Sharma
 * @brief File to hold all ble functions
**/

//includes
#include "ble.h"
#include "app_log.h"
#include "app_assert.h"
#include "lcd.h"
#include "ble_device_type.h"
#include "scheduler.h"
#include "gpio.h"
#define INCLUDE_LOG_DEBUG 1
#include "log.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SCAN_INTERVAL (80)
#define SCAN_WINDOW (40)
#define MIN_INTERVAL (60) //75/1.25
#define MAX_INTERVAL (60) //75/1.25
#define SLAVE_LATENCY (4) //300/75
#define TIMEOUT (825) //(1 + slave_latency) * (max_interval * 2) + max_interval //(1+4)(75*2)+75
#define MIN_CE_LENGTH (0)
#define MAX_CE_LENGTH (4)

int address_check=0;
// BLE private data
ble_data_struct_t ble_data;

/**
 * @brief  function to return a pointer to a type of ble_data_struct_t
 * @param  none
 * @return a pointer to a type of ble_data_struct_t
 **/
ble_data_struct_t* getBleDataPtr()
{
  return(&ble_data);
}

#if (DEVICE_IS_BLE_SERVER == 1)

/**
 * @brief  function to handle ble server events
 * @param  event of type sl_bt_msg_t
 * @return none
 **/
void handle_ble_event_server(sl_bt_msg_t *evt)
{
  sl_status_t status_sc;

  switch (SL_BT_MSG_ID(evt->header))
      {
          case sl_bt_evt_system_boot_id:
          // handle boot event
         //LOG_INFO("BOOT EVENT\n\r");

            ble_data.bonding_flag=false;
            ble_data.passkey_flag=false;

            status_sc = sl_bt_system_get_identity_address(&ble_data.myAddress, &ble_data.myAddressType);
            if(status_sc!= SL_STATUS_OK)
              {
                LOG_ERROR("sl_bt_system_get_identity_address() returned != 0 status=0x%04x", (unsigned int) status_sc);
              }

            status_sc = sl_bt_advertiser_create_set(&ble_data.advertisingSetHandle);
            if(status_sc!= SL_STATUS_OK)
              {
                LOG_ERROR("sl_bt_advertiser_create_set() returned != 0 status=0x%04x", (unsigned int) status_sc);
              }

            status_sc = sl_bt_advertiser_set_timing (ble_data.advertisingSetHandle,
                                                                 400, //Set the Advertising minimum and maximum to 250mS
                                                                 400, //250 *1000/625
                                                                 0,
                                                                 0);
            if(status_sc!= SL_STATUS_OK)
              {
                LOG_ERROR("sl_bt_advertiser_set_timing() returned != 0 status=0x%04x", (unsigned int) status_sc);
              }

            status_sc = sl_bt_advertiser_start(ble_data.advertisingSetHandle,
                                               sl_bt_advertiser_general_discoverable,
                                               sl_bt_advertiser_connectable_scannable);
            if(status_sc!= SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_advertiser_start() returned != 0 status=0x%04x", (unsigned int) status_sc);
            }

            //ADD BONDING FUNCTIONS

            //Delete all bonding information
            status_sc = sl_bt_sm_delete_bondings();
            if(status_sc!= SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_sm_delete_bondings() returned != 0 status=0x%04x", (unsigned int) status_sc);
            }

            //Server will use the DisplayYesNo IO capabilities
            status_sc= sl_bt_sm_configure(0x0f, sm_io_capability_displayyesno);
            if(status_sc!= SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_sm_configure() returned != 0 status=0x%04x", (unsigned int) status_sc);
            }

            //LCD actions for boot event
            displayInit();
            displayPrintf(DISPLAY_ROW_NAME, BLE_DEVICE_TYPE_STRING);
            displayPrintf(DISPLAY_ROW_ASSIGNMENT, "PROJECT");
            displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
            displayPrintf(DISPLAY_ROW_BTADDR, "%02x%02x%02x%02x%02x%02x", ble_data.myAddress.addr[0],ble_data.myAddress.addr[1],ble_data.myAddress.addr[2],ble_data.myAddress.addr[3],ble_data.myAddress.addr[4],ble_data.myAddress.addr[5]);
            displayPrintf(DISPLAY_ROW_PASSKEY," ");

          break;

          case sl_bt_evt_connection_opened_id:
            // handle open event
            //LOG_INFO("open EVENT\n\r");
            ble_data.connection_open = true;
            sl_bt_advertiser_stop(ble_data.advertisingSetHandle);
            ble_data.handle_for_client = evt->data.evt_connection_opened.connection;
            sl_bt_connection_set_parameters(ble_data.handle_for_client,
                                            MIN_INTERVAL, //connection interval min and max set to 75 mS
                                            MAX_INTERVAL, //75*1000/1250
                                            SLAVE_LATENCY, //Slave latency set to enable it to be “off the air” for up to 300mS, ie=300/75
                                            750,//(1+slave latency)*75*2= (1+4)*75*2
                                            //(1 + @p latency) * @p max_interval * 2
                                            0,
                                            0);
            displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");

          break;

          case sl_bt_evt_connection_closed_id:
            // handle close event
           // LOG_INFO("close EVENT\n\r");
            ble_data.connection_open = false;
            ble_data.bonding_flag=false;
            ble_data.passkey_flag=false;
            ble_data.button_notification_flag=false;

            status_sc = sl_bt_advertiser_start(ble_data.advertisingSetHandle,
                                                       sl_bt_advertiser_general_discoverable,
                                                       sl_bt_advertiser_connectable_scannable);
            if(status_sc!= SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_advertiser_start() returned != 0 status=0x%04x", (unsigned int) status_sc);
            }
            gpioLed0SetOn();

            gpioLed0SetOff();
            displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
            displayPrintf(DISPLAY_ROW_TEMPVALUE,"");
            displayPrintf(DISPLAY_ROW_9, " ");
            displayPrintf(DISPLAY_ROW_PASSKEY, " ");
            displayPrintf(DISPLAY_ROW_ACTION, " ");

            status_sc = sl_bt_sm_delete_bondings();
            if(status_sc!= SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_sm_delete_bondings() returned != 0 status=0x%04x", (unsigned int) status_sc);
            }

          break;

          case sl_bt_evt_connection_parameters_id:
          break;

          case sl_bt_evt_system_external_signal_id:
            //LOG_INFO("sl_bt_evt_system_external_signal_id returned the event %d\r\n ",(int) evt->data.evt_system_external_signal.extsignals);
            if((ble_data.bonding_flag == 1)&&(ble_data.button_notification_flag == false)&& (evt->data.evt_system_external_signal.extsignals == evt_pb0_pressed))
            {
              status_sc = sl_bt_sm_passkey_confirm(ble_data.handle_for_client, 1);
              if(status_sc!= SL_STATUS_OK)
               {
                 LOG_ERROR("sl_bt_sm_passkey_confirm() returned != 0 status=0x%04x", (unsigned int) status_sc);
               }
            }

             if((ble_data.ok_to_send_htm_notif==true)&&((evt->data.evt_system_external_signal.extsignals==evt_button_even_pressed)
                                                          ||(evt->data.evt_system_external_signal.extsignals==evt_button_odd_pressed)))
              {
                if(GPIO_PinInGet(GPIO_PORT2,KEYPAD_GPIO_PIN3)==0)//button odd pressed
                  {
                    button_state_to_gatt(1);
                    displayPrintf(DISPLAY_ROW_9, "Button ODD");
                  }
                else if (GPIO_PinInGet(GPIO_PORT2,KEYPAD_GPIO_PIN4)==0)
                  {
                    button_state_to_gatt(2);
                    displayPrintf(DISPLAY_ROW_9, "Button EVEN");
                  }
                else
                  {
                    button_state_to_gatt(0);
                    displayPrintf(DISPLAY_ROW_9, "No Button");
                  }
              }
          break;

          case sl_bt_evt_gatt_server_characteristic_status_id:
            //LOG_INFO("characStatus EVENT\n\r");
            //Track whether indications are enabled/disabled for each and every characteristic. Indications for each characteristic value is independently controllable from the client.

            if (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_Button_input)
              {
                ble_data.connection_handle= evt->data.evt_gatt_server_characteristic_status.connection;

                if(evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config )
                  {
                    // do some cool stuff
                         if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_disable)
                           {
                             ble_data.ok_to_send_htm_notif = false;
                             //LOG_INFO("htm indic disabled\n\r");
                             displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
                             //gpioLed0SetOff();
                           }
                          else if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_notification)
                          {
                            ble_data.ok_to_send_htm_notif = true;
                           // LOG_INFO("htm indic enabled\n\r");
                          }
                  }
              }

            //Track whether an indication is in flight or not
            if(evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_confirmation)//Characteristic confirmation has been received which means confirmation was sent
              {
                ble_data.notif_in_flight = false;
              }
          break;

          case sl_bt_evt_system_soft_timer_id:
            //handle timer expire
            displayUpdate();
            break;

          case sl_bt_evt_sm_bonded_id:

            ble_data.bonding_flag = true;

            displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
            displayPrintf(DISPLAY_ROW_PASSKEY, " ");
            displayPrintf(DISPLAY_ROW_ACTION, " ");

            break;

          case sl_bt_evt_sm_confirm_bonding_id:

            ble_data.passkey_flag = false;

            status_sc = sl_bt_sm_bonding_confirm(ble_data.handle_for_client,1);
            if(status_sc!= SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_sm_bonding_confirm() returned != 0 status=0x%04x", (unsigned int) status_sc);
            }
            break;

          case sl_bt_evt_sm_confirm_passkey_id:

            ble_data.passkey_flag = true;
            displayPrintf(DISPLAY_ROW_PASSKEY, " %d", evt->data.evt_sm_confirm_passkey.passkey);
            displayPrintf(DISPLAY_ROW_ACTION, "Confirm with PB0");
            break;

          case  sl_bt_evt_sm_bonding_failed_id:

            ble_data.bonding_flag = false;
            ble_data.passkey_flag = false;
            displayPrintf(DISPLAY_ROW_CONNECTION, "Failed");
            displayPrintf(DISPLAY_ROW_PASSKEY, " ");
            displayPrintf(DISPLAY_ROW_ACTION, " ");
            LOG_ERROR("sl_bt_evt_sm_bonding_failed_id \n\r");
            break;

          default:break;
          } // end - switch
} // handle_ble_event()

#endif // Server


#if (DEVICE_IS_BLE_SERVER == 0)

/**
 * @brief  function to handle ble client events
 * @param  event of type sl_bt_msg_t
 * @return none
 **/
void handle_ble_event_client(sl_bt_msg_t *evt)
{
  sl_status_t status_sc;

  switch (SL_BT_MSG_ID(evt->header))
  {
    case sl_bt_evt_system_boot_id:
      //LOG_INFO("boot\n\r");

      ble_data.bonding_flag=false; // DOS
      ble_data.passkey_flag=false; // DOS
      ble_data.button_notification_flag=false; // DOS, set false, discovery will set this

      status_sc = sl_bt_system_get_identity_address(&ble_data.myAddress, &ble_data.myAddressType);
      if(status_sc!= SL_STATUS_OK)
       {
         LOG_ERROR("sl_bt_system_get_identity_address() returned != 0 status=0x%04x", (unsigned int) status_sc);
       }

      status_sc = sl_bt_scanner_set_mode(sl_bt_gap_phy_1m,0);//Set phy to 1M and mode to passive scanning
      if(status_sc!= SL_STATUS_OK)
       {
         LOG_ERROR("sl_bt_scanner_set_mode() returned != 0 status=0x%04x", (unsigned int) status_sc);
       }


      status_sc = sl_bt_scanner_set_timing(sl_bt_gap_phy_1m,
                                           SCAN_INTERVAL,//duration between 2 scans = 50/0.625
                                           SCAN_WINDOW);//duration of 1 scan = 25 /0.625
      if(status_sc!= SL_STATUS_OK)
       {
         LOG_ERROR("sl_bt_scanner_set_timing() returned != 0 status=0x%04x", (unsigned int) status_sc);
       }

      status_sc = sl_bt_connection_set_default_parameters( MIN_INTERVAL,
                                               MAX_INTERVAL,
                                               SLAVE_LATENCY,
                                               TIMEOUT,
                                               MIN_CE_LENGTH,
                                               MAX_CE_LENGTH);
      if(status_sc!= SL_STATUS_OK)
       {
         LOG_ERROR("sl_bt_connection_set_default_parameters() returned != 0 status=0x%04x", (unsigned int) status_sc);
       }


      status_sc =sl_bt_scanner_start(sl_bt_gap_phy_1m, sl_bt_scanner_discover_generic);
      if(status_sc!= SL_STATUS_OK)
       {
         LOG_ERROR("sl_bt_scanner_start() returned != 0 status=0x%04x", (unsigned int) status_sc);
       }

      //add bonding functions
      status_sc= sl_bt_sm_delete_bondings();
      if(status_sc!= SL_STATUS_OK)
       {
         LOG_ERROR("sl_bt_sm_delete_bondings() returned != 0 status=0x%04x", (unsigned int) status_sc);
       }

      status_sc= sl_bt_sm_configure(0x0f, sm_io_capability_displayyesno);
      if(status_sc!= SL_STATUS_OK)
      {
        LOG_ERROR("sl_bt_sm_configure() returned != 0 status=0x%04x", (unsigned int) status_sc);
      }

      //LCD actions for boot event
      displayInit();
      displayPrintf(DISPLAY_ROW_NAME, BLE_DEVICE_TYPE_STRING);
      displayPrintf(DISPLAY_ROW_ASSIGNMENT, "PROJECT");
      displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
      displayPrintf(DISPLAY_ROW_BTADDR, "%02x%02x%02x%02x%02x%02x", ble_data.myAddress.addr[5],ble_data.myAddress.addr[4],ble_data.myAddress.addr[3],ble_data.myAddress.addr[2],ble_data.myAddress.addr[1],ble_data.myAddress.addr[0]);
      displayPrintf(DISPLAY_ROW_TEMPVALUE, " ");
      displayPrintf(DISPLAY_ROW_BTADDR2, " ");
      break;

    case sl_bt_evt_connection_opened_id:
      //LOG_INFO("open\n\r");
      ble_data.connection_open = true;
      displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
      break;

    case sl_bt_evt_connection_closed_id:
     // LOG_INFO("close \n\r");
      ble_data.connection_open = false;
      ble_data.bonding_flag=false; // DOS
      ble_data.passkey_flag=false; // DOS

      //start scanning here
      status_sc = sl_bt_scanner_start(sl_bt_gap_phy_1m, sl_bt_scanner_discover_generic);
      if(status_sc!= SL_STATUS_OK)
             {
               LOG_ERROR("sl_bt_scanner_start() returned != 0 status=0x%04x", (unsigned int) status_sc);
             }
      status_sc = sl_bt_sm_delete_bondings();
      if(status_sc!= SL_STATUS_OK)
                   {
                     LOG_ERROR("sl_bt_sm_delete_bondings() returned != 0 status=0x%04x", (unsigned int) status_sc);
                   }


      displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
      displayPrintf(DISPLAY_ROW_TEMPVALUE, " ");
      displayPrintf(DISPLAY_ROW_BTADDR2, " ");
      displayPrintf(DISPLAY_ROW_9, " ");
      break;

    case sl_bt_evt_system_soft_timer_id :
      displayUpdate();
      break;

    case sl_bt_evt_scanner_scan_report_id :
      //Received for advertising or scan response packets generated by: sl_bt_scanner_start().
      //Is the bd_addr, packet_type(0) and address_type (SERVER what we expect for our Server? If yes:
      //sl_bt_scanner_stop()
      //sl_bt_connection_open()
      //variable to check if all the address bytes match
      //LOG_INFO("rep id\n\r");
      for(int i=0;i<6;i++)
        {
          if(evt->data.evt_scanner_scan_report.address.addr[i] == SERVER_BT_ADDRESS.addr[i])
            {
              address_check++;
            }
        }
      if(address_check==6)
        {
          sl_bt_scanner_stop();
          status_sc = sl_bt_connection_open(evt->data.evt_scanner_scan_report.address,
                                            sl_bt_gap_public_address,//sl_bt_gap_public_address (0x0):</b> Public device address
                                            sl_bt_gap_phy_1m,
                                            &ble_data.connection_handle); //connection Handle that will be assigned to the connection after the connection is established. This handle is valid only if the result codeof this response is 0 (zero).
          if(status_sc!= SL_STATUS_OK)
           {
             LOG_ERROR("sl_bt_connection_open() returned != 0 status=0x%04x", (unsigned int) status_sc);
           }
          //displayPrintf(DISPLAY_ROW_CONNECTION, "Connecting");
          displayPrintf(DISPLAY_ROW_BTADDR2, "%02x%02x%02x%02x%02x%02x",SERVER_BT_ADDRESS.addr[0],
                                                                        SERVER_BT_ADDRESS.addr[1],
                                                                        SERVER_BT_ADDRESS.addr[2],
                                                                        SERVER_BT_ADDRESS.addr[3],
                                                                        SERVER_BT_ADDRESS.addr[4],
                                                                        SERVER_BT_ADDRESS.addr[5] );
          address_check=0;
        }

      break;

    case sl_bt_evt_gatt_procedure_completed_id :
      //LOG_INFO("gattcomp\n\r");

      if(evt->data.evt_gatt_procedure_completed.result == 0x110F)
        {
          status_sc= sl_bt_sm_increase_security(ble_data.connection_handle);
          if(status_sc!= SL_STATUS_OK)
         {
           LOG_ERROR("sl_bt_sm_increase_security() returned != 0 status=0x%04x", (unsigned int) status_sc);
         }
        }

      break;

    case sl_bt_evt_gatt_service_id :
      //Save GATT HTM service handle
     // LOG_INFO("servh\n\r");
      //check service handle for temp
      if((evt->data.evt_gatt_service.uuid.data[0] == Keypad_service_uuid[0]) && (evt->data.evt_gatt_service.uuid.data[1] == Keypad_service_uuid[1]))
          {
          ble_data.Keypad_service_handle = evt->data.evt_gatt_service.service;
          LOG_INFO("Keypad Servc found=%d", (int) ble_data.Keypad_service_handle); // DOS
          }

      //check service handle for button
      if( evt->data.evt_gatt_service.uuid.data[0] == LED_service_uuid[0] &&
          evt->data.evt_gatt_service.uuid.data[1] == LED_service_uuid[1] &&
          evt->data.evt_gatt_service.uuid.data[2] == LED_service_uuid[2] &&
          evt->data.evt_gatt_service.uuid.data[3] == LED_service_uuid[3] &&
          evt->data.evt_gatt_service.uuid.data[4] == LED_service_uuid[4] &&
          evt->data.evt_gatt_service.uuid.data[5] == LED_service_uuid[5] &&
          evt->data.evt_gatt_service.uuid.data[6] == LED_service_uuid[6] &&
          evt->data.evt_gatt_service.uuid.data[7] == LED_service_uuid[7] &&
          evt->data.evt_gatt_service.uuid.data[8] == LED_service_uuid[8] &&
          evt->data.evt_gatt_service.uuid.data[9] == LED_service_uuid[9] &&
          evt->data.evt_gatt_service.uuid.data[10] == LED_service_uuid[10] &&
          evt->data.evt_gatt_service.uuid.data[11] == LED_service_uuid[11] &&
          evt->data.evt_gatt_service.uuid.data[12] == LED_service_uuid[12] &&
          evt->data.evt_gatt_service.uuid.data[13] == LED_service_uuid[13] &&
          evt->data.evt_gatt_service.uuid.data[14] == LED_service_uuid[14] &&
          evt->data.evt_gatt_service.uuid.data[15] == LED_service_uuid[15]
        )
        {
          ble_data.LED_service_handle = evt->data.evt_gatt_service.service;
          LOG_INFO("LED Servc found=%d", (int) ble_data.LED_service_handle); // DOS
        }

      break;

    case sl_bt_evt_gatt_characteristic_id :
      LOG_INFO("Char evt, h=%d, u=%x,%x", evt->data.evt_gatt_characteristic.characteristic,
                                          evt->data.evt_gatt_characteristic.uuid.data[0],
                                          evt->data.evt_gatt_characteristic.uuid.data[1]); // DOS
      //Save GATT HTM charac handle
      //DOS: Unconditional assignment -> ble_data.GATT_HTM_characteristics_handle = evt->data.evt_gatt_characteristic.characteristic;

      //DOS wrong data struct in the union -> if((evt->data.evt_gatt_service.uuid.data[0] == thermo_charac_uuid[0]) && (evt->data.evt_gatt_service.uuid.data[1] == thermo_charac_uuid[1]))
      if((evt->data.evt_gatt_characteristic.uuid.data[0] == Keypad_charac_uuid[0]) && (evt->data.evt_gatt_characteristic.uuid.data[1] == Keypad_charac_uuid[1]))
              {
              ble_data.Keypad_characteristics_handle = evt->data.evt_gatt_characteristic.characteristic;
              LOG_INFO("Keypad Char found=%d", (int) ble_data.Keypad_characteristics_handle); // DOS
              }

          //check charac handle for button
            if( evt->data.evt_gatt_characteristic.uuid.data[0] == LED_charac_uuid[0] &&
                evt->data.evt_gatt_characteristic.uuid.data[1] == LED_charac_uuid[1] &&
                evt->data.evt_gatt_characteristic.uuid.data[2] == LED_charac_uuid[2] &&
                evt->data.evt_gatt_characteristic.uuid.data[3] == LED_charac_uuid[3] &&
                evt->data.evt_gatt_characteristic.uuid.data[4] == LED_charac_uuid[4] &&
                evt->data.evt_gatt_characteristic.uuid.data[5] == LED_charac_uuid[5] &&
                evt->data.evt_gatt_characteristic.uuid.data[6] == LED_charac_uuid[6] &&
                evt->data.evt_gatt_characteristic.uuid.data[7] == LED_charac_uuid[7] &&
                evt->data.evt_gatt_characteristic.uuid.data[8] == LED_charac_uuid[8] &&
                evt->data.evt_gatt_characteristic.uuid.data[9] == LED_charac_uuid[9] &&
                evt->data.evt_gatt_characteristic.uuid.data[10] == LED_charac_uuid[10] &&
                evt->data.evt_gatt_characteristic.uuid.data[11] == LED_charac_uuid[11] &&
                evt->data.evt_gatt_characteristic.uuid.data[12] == LED_charac_uuid[12] &&
                evt->data.evt_gatt_characteristic.uuid.data[13] == LED_charac_uuid[13] &&
                evt->data.evt_gatt_characteristic.uuid.data[14] == LED_charac_uuid[14] &&
                evt->data.evt_gatt_characteristic.uuid.data[15] == LED_charac_uuid[15]
              )
            {
              ble_data.LED_characteristics_handle = evt->data.evt_gatt_characteristic.characteristic;
              LOG_INFO("LED Char found=%d", (int) ble_data.LED_characteristics_handle); // DOS
            }

      break;

    case sl_bt_evt_gatt_characteristic_value_id:
      //call the send confirmation here
     // LOG_INFO("valueid\n\r");

      // Is it the button char handle AND att_opcode == notif?
      if ( (evt->data.evt_gatt_characteristic_value.characteristic == ble_data.Keypad_characteristics_handle) &&
           ((evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_notification)||(evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_read_response)))
        {
          sl_bt_gatt_read_characteristic_value(ble_data.connection_handle,ble_data.Keypad_characteristics_handle);

          if (evt->data.evt_gatt_characteristic_value.value.data[1])
          { // offset 1 for the indications cuz the flag byte
              displayPrintf(DISPLAY_ROW_9, "Button ODD");
              LED_pattern_odd();
              led_state_to_gatt (1);
          }
           if (evt->data.evt_gatt_characteristic_value.value.data[2])
          {
              displayPrintf(DISPLAY_ROW_9, "Button EVEN");
              LED_pattern_even();
              led_state_to_gatt (2);
          }
            if (evt->data.evt_gatt_characteristic_value.value.data[0])
          {
            displayPrintf(DISPLAY_ROW_9, "No Button");
            led_state_to_gatt(0);
          }

      } // if

      break;

    case sl_bt_evt_sm_confirm_bonding_id:

      ble_data.passkey_flag = false;

      status_sc = sl_bt_sm_bonding_confirm(ble_data.handle_for_client,1);
      if(status_sc!= SL_STATUS_OK)
      {
        LOG_ERROR("sl_bt_sm_bonding_confirm() returned != 0 status=0x%04x", (unsigned int) status_sc);
      }
    break;

    case sl_bt_evt_sm_confirm_passkey_id:

            ble_data.passkey_flag = true;

            displayPrintf(DISPLAY_ROW_PASSKEY, "Passkey %d", evt->data.evt_sm_confirm_passkey.passkey);
            displayPrintf(DISPLAY_ROW_ACTION, "Confirm with PB0");
            //DOS No the device is not yet bonded. ble_data.bonding_flag = true;
            break;

    case sl_bt_evt_sm_bonded_id:
          //LOG_INFO("Bonded");
          ble_data.bonding_flag = true; // DOS
          displayPrintf(DISPLAY_ROW_PASSKEY, " ");
          displayPrintf(DISPLAY_ROW_ACTION, " ");
          displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");

          break;

    case sl_bt_evt_sm_bonding_failed_id:

          ble_data.bonding_flag = false;
          ble_data.passkey_flag = false;
          displayPrintf(DISPLAY_ROW_PASSKEY, " ");
          displayPrintf(DISPLAY_ROW_ACTION, " ");
          displayPrintf(DISPLAY_ROW_CONNECTION, "Failed");
          LOG_ERROR("Bonding Failed");
          break;

    case sl_bt_evt_system_external_signal_id:

      //DOS Handle security
      if ( (evt->data.evt_system_external_signal.extsignals == evt_pb0_pressed) && // PB0 event
           (GPIO_PinInGet(PB0_BUTTON_PORT, PB0_BUTTON_PIN) == 0) && // PB0 is pressed
           ( ble_data.passkey_flag) && // We are displaying the passkey to the user
           (!ble_data.bonding_flag) )  // We are not yet bonded
        {
                status_sc = sl_bt_sm_passkey_confirm(ble_data.connection_handle, 1);
                if(status_sc!= SL_STATUS_OK)
               {
                 LOG_ERROR("sl_bt_sm_passkey_confirm_1() returned != 0 status=0x%04x", (unsigned int) status_sc);
               }

        }


    if(ble_data.button_notification_flag== false) // indications are off, turn it on
    {
        status_sc = sl_bt_gatt_set_characteristic_notification(ble_data.connection_handle,
                                                               ble_data.Keypad_characteristics_handle,
                                                               sl_bt_gatt_notification
                                                              );
        if(status_sc!= SL_STATUS_OK)
       {
         LOG_ERROR("sl_bt_gatt_set_characteristic_notification_1() returned != 0 status=0x%04x", (unsigned int) status_sc);
       }

        ble_data.button_notification_flag=true; // DOS toggle the value for next time
    }


    else // notif are on, turn it off

      {
        status_sc = sl_bt_gatt_set_characteristic_notification(ble_data.connection_handle,
                                                              ble_data.Keypad_characteristics_handle,
                                                              sl_bt_gatt_disable
                                                             );
        if(status_sc!= SL_STATUS_OK)
       {
         LOG_ERROR("sl_bt_gatt_set_characteristic_notification_2() returned != 0 status=0x%04x", (unsigned int) status_sc);
       }
        ble_data.button_notification_flag=false; // toggle the value for next time
      }

      break;

  } // switch

}

#endif // Client

void LED_pattern_odd(void)
{
  for(int j=0;j<2000;j++)
      {
        gpioLed0SetOn();
        for (int i=0;i<1000;i++){;}
        gpioLed0SetOff();
        gpioLed1SetOn();
        for (int i=0;i<1000;i++){;}
        gpioLed1SetOff();
        gpioLed0SetOn();
        for (int i=0;i<1000;i++){;}
        gpioLed0SetOff();
        gpioLed1SetOn();
        for (int i=0;i<1000;i++){;}
        gpioLed1SetOff();
      }
}

void LED_pattern_even (void)
{
  for(int j=0;j<2000;j++)
       {
         gpioLed0SetOn();
         gpioLed1SetOn();
         for (int i=0;i<1000;i++){;}
         gpioLed0SetOff();
         for (int i=0;i<1000;i++){;}
         gpioLed0SetOn();
         for (int i=0;i<1000;i++){;}
         gpioLed1SetOff();
         for (int i=0;i<1000;i++){;}
         gpioLed0SetOff();
       }
}

/**
 * @brief  function to write temp to gatt database
 * @param  temperature value in degrees
 * @return none
 **/
void led_state_to_gatt(uint8_t led_state)
{
  uint32_t status_sc;
    status_sc = sl_bt_gatt_server_write_attribute_value(
                                                    gattdb_LED_output, // handle from gatt_db.h
                                                    0, // offset
                                                    sizeof(led_state), // length
                                                    &led_state // pointer to buffer where data is//
                                                    );
    if(status_sc!= SL_STATUS_OK)
       {
         LOG_ERROR("sl_bt_gatt_server_write_attribute_value() returned != 0 status=0x%04x", (unsigned int) status_sc);
       }

}

void button_state_to_gatt(uint8_t button)
{
  uint32_t status_sc;
  status_sc = sl_bt_gatt_server_write_attribute_value(
                                                  gattdb_Button_input, // handle from gatt_db.h
                                                  0, // offset
                                                  sizeof(button), // length
                                                  &button // pointer to buffer where data is//
                                                  );
  if(status_sc!= SL_STATUS_OK)
     {
       LOG_ERROR("sl_bt_gatt_server_write_attribute_value() returned != 0 status=0x%04x", (unsigned int) status_sc);
     }

  if(!ble_data.notif_in_flight)
       {

         status_sc = sl_bt_gatt_server_send_notification(ble_data.connection_handle,
                                                         gattdb_Button_input, // handle from gatt_db.h
                                                         1,
                                                         &button // in IEEE-11073 format
                                                         );
         if(status_sc!= SL_STATUS_OK)
          {
            LOG_ERROR("sl_bt_gatt_server_send_notification() returned != 0 status=0x%04x", (unsigned int) status_sc);
          }
         else
           {
             ble_data.notif_in_flight= true;
           }
       }
}
