/**
 * @file ble.h
 * @author Isha Sharma
 * @date 08-Oct-2023
 * @brief header file for ble.c
 **/

#ifndef SRC_BLE_H_
#define SRC_BLE_H_

#include "sl_bt_api.h"
#include "gatt_db.h"
#include "sl_bluetooth.h"

// BLE Data Structure, save all of our private BT data in here.
// Modern C (circa 2021 does it this way)
// typedef ble_data_struct_t is referred to as an anonymous struct definition

//uuids as found in the gatt configurator in little endian format
static const uint8_t Keypad_service_uuid[2]= {0x09,0x18};
//static const uint16_t service_uuid = 0x1809;
static const uint8_t Keypad_charac_uuid[2] = {0x1C, 0x2A};
//static const uint16_t charac_uuid = 0x2A1C;

//00000001-38c8-433e-87ec-652a2d136289
static const uint8_t LED_service_uuid[16]= { 0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x01, 0x00, 0x00, 0x00 };
//00000002-38c8-433e-87ec-652a2d136289
static const uint8_t LED_charac_uuid[16] = { 0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x02, 0x00, 0x00, 0x00 };

typedef struct
{
    // values that are common to servers and clients
    bd_addr myAddress;
    uint8_t myAddressType;
    // values unique for server
    uint8_t advertisingSetHandle;
    bool connection_open; // true when in an open connection
    bool ok_to_send_htm_notif; // true when client enabled indications
    bool notif_in_flight; // true when an indication is in-flight
    bool button_notification_flag;
    // values unique for client
    uint16_t handle_for_client;
    uint8_t connection_handle;

    uint32_t Keypad_service_handle;
    uint16_t Keypad_characteristics_handle;

    uint32_t LED_service_handle;
    uint16_t LED_characteristics_handle;

    bool bonding_flag;
    bool passkey_flag;

} ble_data_struct_t;

//function prototypes
void handle_ble_event_server(sl_bt_msg_t *evt);
void handle_ble_event_client(sl_bt_msg_t *evt);
ble_data_struct_t* getBleDataPtr();
void led_state_to_gatt(uint8_t led_state);
void button_state_to_gatt(uint8_t button);
void LED_pattern_odd(void);
void LED_pattern_even (void);


#endif /* SRC_BLE_H_ */
