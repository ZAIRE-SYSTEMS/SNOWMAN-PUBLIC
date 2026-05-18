/**
 * @brief Main header for Zaire Systems
 *
 * This is the main "contract" for the whole project.
 * It exposes all the core definitions, configs, structs,
 * and function prototypes used across the system.
 *
 * Includes:
 *  - System init + core functions
 *  - Network + ESP-NOW (walkie) functions
 *  - DNS / captive portal functions
 *  - UART, I2S, camera configs
 *  - Button mappings
 *  - NVS read/write helpers
 *  - Global system structs (prefs, state, device info)
 *
 * Also defines:
 *  - Device modes (BREAD / PROTOTYPE)
 *  - Pin configs
 *  - Communication settings
 *  - Walkie pairing + mesh system
 *
 * Basically: if something is shared across files, it’s probably here.
 *
 * @note
 * Changing anything here can affect the entire system, so be careful.
 *
 * @warning
 * Keep this file organized — it can get messy fast since everything depends on it.
 *
 * @author Yann Kabambi
 * @project ZAIRE SYSTEMS
 */


#ifndef ZAIRE_SYSTEMS_H
#define ZAIRE_SYSTEMS_H

#include "globalVar.h"
#include "zaire_systems_settings_codex.h"
#include "zaire_systems_bt1026_command_list.h"
#include "zaire_systems_espNow_cmd_list.h"


#define BREAD           0x00
#define PROTOTYPE       0x01
#define DEV_BOARD       PROTOTYPE

extern uint8_t rcv_test_board_mac_addr[6];


//initialize error types
typedef enum{
    ZAIRE_OK = 0,
    ZAIRE_ERR = !ZAIRE_OK
} zaire_err_t;

// ======================
// 1. USER SETTINGS (stored in NVS, survives reboot)
// ======================
typedef struct {
    char user_name[64];      // "Alex", "ZAIRE Rider", etc.
    char device_name[64];    // "ZAIRE-VISION-001", "SnowBeast"
    uint8_t passcode_enabled;       // 0 = no passcode set, 1 = passcode set
} user_prefs_t;

// ======================
// 2. SYSTEM SETTINGS (user preferences, stored in NVS)
// ======================
typedef struct {
    char device_lang[8];            // "en", "es", "fr", etc.
    uint8_t units;                    // 0 = metric (km/h, m), 1 = imperial (mph, ft)
    uint8_t theme;                   // 0 = light, 1 = dark
    uint8_t walkie_volume;            // 0 = low, 1 = medium, 2 = high
    uint8_t walkie_mic_sensitivity;   // 0 = low, 1 = medium, 2 = high
    uint8_t walkie_bt_behavior;     // 0 = lower volume, 1 = pause music, 2 = mute music,
                                    // 3 = leave music on
    bool btAutoConnect;              // should the device auto-connect to previously paired Bluetooth devices?
    uint8_t bt_init_volume;         // initial Bluetooth volume (0-100)
    bool btAutoPlay;             // should the device auto-play audio when a Bluetooth connection is established?
    bool btAutoDecline;           // should the device auto-answer calls when a Bluetooth connection is established?
    uint8_t btn_array[4];
    // Add more user prefs here later (vibration, HUD brightness, etc.)
} system_prefs_t;

// ======================
// 3. DEVICE STATE (live/runtime data, NOT saved to flash)
// ======================
typedef struct {
    uint16_t photos_count;            // current number of photos in /media/photos/
    uint16_t videos_count;
    bool     bt_connected;            // is Bluetooth currently linked?
    bool     bt_audio_active;         // is audio streaming?
    char     bt_device_name[64];      // "Alex’s iPhone",
    uint8_t  walkie_peers_count;       // how many walkie devices are connected
    bool     walkie_active;           // is walkie-talkie session running?
    bool     walkie_paired;           // has at least one peer been paired ever?
    float    top_speed;               // current session best (in current units)
    uint16_t top_height;              // meters or feet
    float    top_accel;               // ft/s or m/s²
    // Add live sensors here: battery %, temperature, GPS fix, etc.
} device_state_t;

// ======================
// 4. WALKIE PEER INFO STRUCTURE AND GLOBAL ARRAY
// ======================

typedef struct {
    uint8_t  id;                        // 0–8 → matches array index
    char     name[32];                  // "Alex", "Helmet #5"
    char     last_position[64];         // "Ridge lookout", "GPS: 46.8123,-121.7456"
    uint32_t last_comm_timestamp;       // Unix timestamp (seconds since 1970)
    int16_t  rssi;                      // signed! RSSI is always negative (-30 to -100)
    uint8_t  battery_level;             // 0–100%
    uint8_t  __padding[1];              // align to 128 bytes (nice for flash/JSON)
} walkie_peer_systemSettings_t;


// ======================
// 5 - A. DEVICE ENCODED INFO
// ======================
typedef struct {
    uint8_t first_boot;              // 0 = new device, 1 = already accepted
    uint32_t encoded_passcode;
    uint8_t recovery_code;
} usr_secure_info_t;

// ======================
// 5 - B. DEVICE ENCODED INFO
// ======================
typedef struct {
    uint8_t first_boot;              // 0 = new device, 1 = already accepted
} sys_secure_info_t;


// ======================
// 6. DEVICE ENCODED INFO
// ======================

typedef struct {
    /* =========================
       DEVICE IDENTITY
       ========================= */
    char manufacturer[16];        // "ZAIRE SYSTEMS"
    char model_name[20];           // "ZAIRE-HELMET" - "ZAIRE VISION"
    char device_id[24];        // Unique per unit

    /* =========================
       HARDWARE
       ========================= */
    char hardware_revision[16];   // "1.0", "1.1", etc.
    char bluetooth_version[16];   // "5.0", "5.2", etc.

    /* =========================
       FIRMWARE
       ========================= */
    char firmware_version[16];     // "1.0.0"

    /* =========================
       BUILD INFO
       ========================= */
    char build_date[16];          // "2024-06-01"
    char build_type[16];          // "debug" or "release"

} device_info_t;


// ======================
// Global instances
// ======================
extern user_prefs_t    g_user_prefs;
extern system_prefs_t  g_system_prefs;
extern device_state_t  g_device_state;
extern walkie_peer_systemSettings_t g_walkie_peer_info[8];
extern device_info_t   g_device_info;
extern usr_secure_info_t g_usr_secure_info;
extern sys_secure_info_t g_sys_secure_info;


//initialize ZAIRE SYSTEMS 
zaire_err_t zaire_systems_init(void);
zaire_err_t zaire_systems_rst_all_pins(void);
zaire_err_t zaire_systems_spiffs_init(void);
zaire_err_t zaire_systems_gpio_btn_init();

//NETWORK
#define NETWORK_MAX_CONN            9
#define NETWORK_SETTINGS_SSID       "ZAIRE VIEW PORTAL"
extern bool is_esp_now_active;
zaire_err_t zaire_systems_network_init();
zaire_err_t zaire_systems_network_settings_mode();
zaire_err_t zaire_systems_network_standard_mode();
zaire_err_t zaire_systems_network_pairing_mode_master();
zaire_err_t zaire_systems_network_pairing_mode_slave();
void wifi_send_ap_bssid();
zaire_err_t esp_now_mesh_init(void);
void send_data_to_mesh(uint8_t data);
void detach_peer_from_mesh(uint8_t position);
void peer_leave_mesh(void);
void esp_now_sent_cb(const uint8_t *mac_addr, esp_now_send_status_t status);
void esp_now_recv_cb(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len);


//SYSTEM BUTTONS CONFIG
//=========== Button Handler ===========
typedef struct {
    gpio_num_t  pin;
    const char *name;
} btn_task_params_t;
#define HIGH                        0x01
#define LOW                         0x00
#define BTN_DEBOUNCE_MS             50
#define BTN_LONG_PRESS_MS           800
#define BTN_DOUBLE_WINDOW_MS        400
#define PURPOSE_BTN                 g_system_prefs.btn_array[0] //init - 1 -- PWR && PLAY/PAUSE && ANSWER CALL
#define WALKIE_BTN                  g_system_prefs.btn_array[1] //init - 0 -- WALKIE && CAMERA CAPTURE
#define NXT_UP_BTN                  g_system_prefs.btn_array[2] //init - 0 -- NEXT && VOLUME UP
#define PRV_DN_BTN                  g_system_prefs.btn_array[3] //init - 1 -- PREVIOUS && VOLUME DOWN
extern uint8_t top_btn;     // = WALKIE_BTN
extern uint8_t bottom_btn; //  = PURPOSE_BTN
extern uint8_t front_btn; //   = NXT_UP_BTN || PRV_DN_BTN -- DEPENDS ON USER PREFS
extern uint8_t back_btn; //    = PRV_DN_BTN || NXT_UP_BTN -- DEPENDS ON USER PREFS


//SYSTEM INDICATOR
#define GREEN_PAIRING_IND      GPIO_NUM_18
#define RED_PAIRING_IND         GPIO_NUM_19

typedef enum{
    OFF = 0,
    STANDBY = 1,
    CYCLE_1 = 2,
    CYCLE_2 = 3, 
    CYCLE_3 = 4

}_ind_time_t;

extern _ind_time_t green_ind_time;
extern _ind_time_t red_ind_time;


//DNS SERVER - CAPTIVE PORTAL 
extern char dns_server_ssid[128];
extern char dns_server_bssid[128];
extern bool is_settings_portal_on;
#define DNS_PORT                53
#define DNS_TASK_STACK_SIZE     8192
#define DNS_TASK_PRIORITY       3
#define MAX_HTTP_RECV_BUFFER    512

#ifdef __cplusplus
extern "C" {
#endif
void start_dns_server(void);
void stop_dns_server(void);
esp_err_t handler(httpd_req_t *req);
void register_dns_catch_all(httpd_handle_t server);
#ifdef __cplusplus
}
#endif


extern TaskHandle_t dns_task_handle;
extern httpd_handle_t web_server;
httpd_handle_t start_webserver(void);
esp_err_t main_page_get_handler(httpd_req_t *req);

zaire_err_t settings_update_system_info(const device_info_t *info);
zaire_err_t settings_update_first_boot(const char *first_boot_values);
zaire_err_t settings_update_user_name(const char *user_name);
zaire_err_t settings_update_device_name(const char *device_name);
zaire_err_t settings_update_device_lang(const char *device_lang);
zaire_err_t settings_update_device_units(const char *device_units);
zaire_err_t settings_update_device_theme(const char *device_theme);
zaire_err_t settings_update_passcode(const char *passcode);
zaire_err_t settings_update_passcode_enabled(const char *passcode_enabled);
zaire_err_t settings_update_recovery_code(const char *recovery_code);
zaire_err_t settings_update_device_button_map(const uint8_t *map);
zaire_err_t settings_update_walkie_volume(const char *walkie_volume);
zaire_err_t settings_update_walkie_mic_sensitivity(const char *walkie_mic_sensitivity);
zaire_err_t settings_update_walkie_bt_behavior(const char *walkie_bt_behavior);
zaire_err_t settings_update_bt_auto_connect(const char *bt_auto_connect);
zaire_err_t settings_update_bt_init_volume(const char *bt_init_volume);
zaire_err_t settings_update_bt_auto_play(const char *bt_auto_play);
zaire_err_t settings_update_bt_auto_decline(const char *bt_auto_decline);

zaire_err_t settings_get_first_boot(void);
const char *settings_get_user_name(void);
const char *settings_get_device_name(void);
const char *settings_get_device_lang(void);
uint8_t settings_get_device_units(void);
uint8_t settings_get_device_theme(void);
uint8_t settings_get_passcode_enabled(void);
const uint8_t *settings_get_device_button_map(void);
uint8_t settings_get_walkie_volume(void);
uint8_t settings_get_walkie_mic_sensitivity(void);
uint8_t settings_get_walkie_bt_behavior(void);
uint8_t settings_get_bt_auto_connect(void);
uint8_t settings_get_bt_init_volume(void);
uint8_t settings_get_bt_auto_play(void);
uint8_t settings_get_bt_auto_decline(void);
const char *settings_get_manufacturer(void);
const char *settings_get_model_name(void);
const char *settings_get_firmware_version(void);
const char *settings_get_build_date(void);
const char *settings_get_device_id(void);
const char *settings_get_hardware_rev(void);
const char *settings_get_bluetooth_version(void);

//I2S PINS CONFIG
#define WALKIE_I2S_BCK_MIC              GPIO_NUM_32
#define WALKIE_I2S_BCK_SPK              GPIO_NUM_33
#define WALKIE_I2S_WS_MIC               GPIO_NUM_26
#define WALKIE_I2S_WS_SPK               GPIO_NUM_25
#define WALKIE_I2S_MIC                  GPIO_NUM_27
#define WALKIE_I2S_SPK                  GPIO_NUM_13
#define WALKIE_I2S_SAMPLE_RATE          15500//16000
extern const i2s_port_t REC_I2S_PORT;
extern const i2s_port_t SPK_I2S_PORT;
#define WALKIE_I2S_SAMPLE_SIZE_128      128
#define WALKIE_I2S_SAMPLE_SIZE_256      256
extern int32_t samples_32bit [WALKIE_I2S_SAMPLE_SIZE_128];
extern int16_t samples_16bit [WALKIE_I2S_SAMPLE_SIZE_128];
extern uint8_t samples_8bit  [WALKIE_I2S_SAMPLE_SIZE_128];
extern int16_t receive_16bit [WALKIE_I2S_SAMPLE_SIZE_128];
extern int16_t output_16bit  [WALKIE_I2S_SAMPLE_SIZE_128];
zaire_err_t zaire_systems_i2s_init(void);
int  zaire_systems_i2s_read(int16_t *samples, int count);
void zaire_systems_i2s_write(int16_t *samples, int count);
void zaire_systems_i2s_covert_bit(int16_t *temp_samples_16bit, uint8_t *temp_samples_8bit, uint8_t len);
void _i2s_spk_buff_out(void);

//NVS
#define ZAIRE_SYSTEMS_CFG_USR_NAMESPACE_KEY   "zaire_usr_cfg"   // User preferences (name, device name, language, etc.)
#define ZAIRE_SYSTEMS_CFG_SYS_NAMESPACE_KEY   "zaire_sys_cfg"   // System settings (button mapping, audio, behavior, etc.)
#define ZAIRE_SYSTEMS_SYS_INFO_NAMESPACE_KEY  "zaire_sys_info"  // System info (device ID, firmware version, hardware data)

#define ZAIRE_NVS_KEY_USER_PREFS      "user_prefs"
#define ZAIRE_NVS_KEY_SYSTEM_PREFS    "system_prefs"
#define ZAIRE_NVS_KEY_USER_SECURE     "user_secure"
#define ZAIRE_NVS_KEY_SYSTEM_SECURE   "system_secure"
#define ZAIRE_NVS_KEY_DEVICE_INFO     "device_info"
#define ZAIRE_NVS_KEY_WALKIE_PEERS    "walkie_peers"


extern nvs_handle_t usr_handle;
extern nvs_handle_t sys_handle;
extern nvs_handle_t info_handle;

zaire_err_t zaire_systems_nvs_init(void);
zaire_err_t zaire_systems_nvs_write_char(nvs_handle_t handle, const char *key, const char *value);
zaire_err_t zaire_systems_nvs_write_blob(nvs_handle_t handle, const char *key, const void *value, size_t size);
zaire_err_t zaire_systems_nvs_read_char(nvs_handle_t handle, const char *key, char *out, size_t size);
zaire_err_t zaire_systems_nvs_read_blob(nvs_handle_t handle, const char *key, void *out, size_t size);


//ZAIRE I2C
#define ZAIRE_I2C_NUM           I2S_NUM_0
#define ZAIRE_I2C_SDA           //TBD
#define ZAIRE_I2C_SCL           //TBD
zaire_err_t zaire_systems_i2c_init(void);


//ZAIRE UART
#define ZAIRE_UART_BUF_SIZE     (1024)
#define ZAIRE_BT_UART_BAUD      (9600)
zaire_err_t zaire_systems_uart_init(void);

//======================= BLUETOOTH =============================

#define ZAIRE_BT_UART_RX        GPIO_NUM_16
#define ZAIRE_BT_UART_TX        GPIO_NUM_17
#define ZAIRE_BT_PP_GPIO        GPIO_NUM_5

extern bool is_bluetooth_discoverable;
extern bool is_bluetooth_paired;


void bluetooth_discoverable_on(void);
void bluetooth_discoverable_off(void);

void bluetooth_set_device_name(const char *name);
void bluetooth_set_device_vol(uint8_t vol);

void bluetooth_pp_action(void);
void bluetooth_nxt_track(void);
void bluetooth_prv_track(void);
void bluetooth_vol_up(void);
void bluetooth_vol_dn(void);



//======================== WALKIE ==============================

//PEER MESH SYSTEM
#define DEVICE_PAIRING_NAME         "zairesystemsPairing_0" 
#define ESP_NOW_CHANNEL             3
#define DEFAULT_SCAN_LIST_SIZE      10


extern uint8_t new_peers;
extern uint8_t peer_addresses[NETWORK_MAX_CONN][6];
extern uint8_t peers_in_mesh; // 1 is us
extern uint8_t peer_mesh_position; // our position in mesh -> important for removal 

void _add_peer_to_mesh(uint8_t *addr);
void _del_peer_from_mesh(uint8_t *addr);
void _sync_mesh_with_all_peers();
void _sync_mesh_pairing_mode(uint8_t *addr);
void _send_sta_addr(uint8_t *addr);
void _request_sta_addr(uint8_t *addr);


//ZAIRE WALKIE SYSTEM
#define MAX_WALKIE_PEERS 9
extern uint8_t _sta_mac_addr[6];
extern uint8_t _ap_mac_addr[6];
extern bool    walkie_out;
extern bool    sendOver;
extern bool    recOver;
extern bool    walkie_spk_en;
extern bool    is_walkie_mute;
extern bool    is_walkie_on; 
extern bool is_walkie_in_pairing_session;

typedef enum{
    NONE = 0,
    MASTER = 1,
    SLAVE = 2,
    SYNC = 3
}walkie_pairing_mode_t;
extern walkie_pairing_mode_t walkie_pairing_mode;

void walkie_pairing_sequence(void);
void walkie_pairing_master(void);
void walkie_pairing_slave(void);
void walkie_mute(void);
void walkie_sendData(const uint8_t *data, uint16_t len);
void _walkie_mic_out(void); //MIC -> I2S -> COMPRESS -> ESP_NOW -> PEER_MESH
void _walkie_spk_out(uint8_t *data, int len); //PEER_MESH -> ESP_NOW -> DECOMPRESS -> I2S -> SPEAKERS


#endif
