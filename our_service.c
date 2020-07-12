
#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "our_service.h"
#include "ble_srv_common.h"
#include "app_error.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

void ble_our_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
  	ble_os_t * p_our_service =(ble_os_t *) p_context;  

	switch (p_ble_evt->header.evt_id)
        {
            case BLE_GAP_EVT_CONNECTED:
                p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
                break;
            case BLE_GAP_EVT_DISCONNECTED:
                p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
                break;
            default:
                // No implementation needed.
                break;
        }
	
}

/**@brief Function for adding our new characterstic to "Our service" that we initiated in the previous tutorial. 
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
static uint32_t our_char_add(ble_os_t * p_our_service)
{
    uint32_t            err_code;
    ble_uuid_t          char_uuid,char_uuid2;
    ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID;
    char_uuid.uuid      = BLE_UUID_OUR_CHARACTERISTC_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code); 

    char_uuid2.uuid     =BLE_UUID_OUR_CHARACTERISTC_UUID2;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid2.type);
    APP_ERROR_CHECK(err_code); 

    
    ble_gatts_char_md_t char_md, char_md2;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
//    char_md.char_props.write = 1;

    memset(&char_md2, 0, sizeof(char_md2));
    char_md2.char_props.read = 1;
//    char_md2.char_props.write = 1;




    ble_gatts_attr_md_t cccd_md,cccd_md2;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;

    memset(&cccd_md2, 0, sizeof(cccd_md2));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md2.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md2.write_perm);
    cccd_md2.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md2.p_cccd_md           = &cccd_md2;
    char_md2.char_props.notify   = 1;


   
    ble_gatts_attr_md_t attr_md, attr_md2;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc        = BLE_GATTS_VLOC_STACK; 

    memset(&attr_md2, 0, sizeof(attr_md2));
    attr_md2.vloc        = BLE_GATTS_VLOC_STACK;  

	
    
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md2.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md2.write_perm);

    
    ble_gatts_attr_t    attr_char_value, attr_char_value2;
    memset(&attr_char_value, 0, sizeof(attr_char_value));    
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;

    memset(&attr_char_value2, 0, sizeof(attr_char_value2));    
    attr_char_value2.p_uuid      = &char_uuid2;
    attr_char_value2.p_attr_md   = &attr_md2;


    
    attr_char_value.max_len     = 1;
    attr_char_value.init_len    = 1;
    uint8_t value[1]            = {0};
    attr_char_value.p_value     = value;
    
    uint8_t value2[2]            = {0,0};
    attr_char_value2.max_len     = 2;
    attr_char_value2.init_len    = 2;
    attr_char_value2.p_value     = value;


    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_our_service->char_handles);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                       &char_md2,
                                       &attr_char_value2,
                                       &p_our_service->char_handles2);
    APP_ERROR_CHECK(err_code);

    return NRF_SUCCESS;
}


/**@brief Function for initiating our new service.
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
void our_service_init(ble_os_t * p_our_service)
{
    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions

    // FROM_SERVICE_TUTORIAL: Declare 16-bit service and 128-bit base UUIDs and add them to the BLE stack
    ble_uuid_t        service_uuid;
    ble_uuid128_t     base_uuid = BLE_UUID_OUR_BASE_UUID;
    service_uuid.uuid = BLE_UUID_OUR_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);    
    
    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    // FROM_SERVICE_TUTORIAL: Add our service
		err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_our_service->service_handle);
    
    APP_ERROR_CHECK(err_code);
    
    our_char_add(p_our_service);
}

void our_temperature_characteristic_update(ble_os_t *p_our_service, int32_t *temperature_value)
{
    if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t               len = 1;
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_our_service->char_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)temperature_value;
        

        sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);
    }


}


void our_saadc_characteristic_update(ble_os_t *p_our_service, int16_t *adc_value)
{
    if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t               len = 2;
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_our_service->char_handles2.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)adc_value;  

//        NRF_LOG_INFO("adc val: %x", *hvx_params.p_data);

        sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);
    }


}
