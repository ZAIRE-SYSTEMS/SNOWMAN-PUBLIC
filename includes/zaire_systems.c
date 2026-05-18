/**
 * @brief Core system init + global state for Zaire
 *
 * This file is basically the main setup / brain of the system.
 * It initializes everything needed for the helmet to run and
 * holds global state used across the project.
 *
 * Handles:
 *  - First boot setup (default settings)
 *  - NVS init + loading/saving system preferences
 *  - SPIFFS init (for web files, storage, etc.)
 *  - GPIO button setup
 *  - Network init
 *  - Buzzer init
 *
 * Also defines global system structs like:
 *  - user prefs
 *  - system prefs
 *  - device state
 *  - device info
 *  - walkie peer info
 *
 * Basically: this is where everything gets brought together on startup.
 *
 * @note
 * If you change system_prefs_t, make sure to update the first boot
 * function so all fields are initialized properly.
 *
 * @warning
 * Missing init steps here can break the whole system, so keep this
 * clean and in order.
 *
 * @author Yann Kabambi
 * @project ZAIRE SYSTEMS
 */

#include "globalVar.h"
#include "zaire_systems.h"


#if DEV_BOARD == PROTOTYPE
    uint8_t top_btn = GPIO_NUM_36;   // WALKIE
    uint8_t bottom_btn = GPIO_NUM_39;// PURPOSE 
    uint8_t front_btn =  GPIO_NUM_4;//TBD (maybe next/up)
    uint8_t back_btn =  GPIO_NUM_15;//TBD  (maybe prev/down)
    uint8_t rcv_test_board_mac_addr[6] = {0xC8, 0x2E, 0x18, 0xF1, 0x77, 0xD0}; //BREAD BOARD'S

#elif DEV_BOARD == BREAD
   uint8_t top_btn = GPIO_NUM_32;
    uint8_t bottom_btn = GPIO_NUM_33;//TBD;
    uint8_t front_btn =  GPIO_NUM_25;
    uint8_t back_btn =  GPIO_NUM_26;//TBD;
    uint8_t rcv_test_board_mac_addr[6] = {0x8, 0xB6, 0x1F, 0xB9, 0xD2, 0xF8}; //ROTOTYPE'S

#endif

uint8_t btn_array[4];

bool is_esp_now_active = false;
int is_setting_on = false;
uint8_t current_BT_vol = 11;
int top_speed_gps = 0;

user_prefs_t    g_user_prefs;
system_prefs_t  g_system_prefs;
device_state_t  g_device_state;
walkie_peer_systemSettings_t g_walkie_peer_info[8];
device_info_t   g_device_info;
usr_secure_info_t g_usr_secure_info;
sys_secure_info_t g_sys_secure_info;

/**
 * @brief Helper function: Initializes default settings on first boot. Called by zaire_systems_init() if NVS doesn't have valid prefs.
 *       Important: If you change the structure of system_prefs_t, update this function to set all fields and avoid uninitialized data.
 * @return ZAIRE_OK if defaults are set successfully, otherwise ZAIRE_ERR.
 */
static zaire_err_t zaire_systems_first_boot(){

    uint8_t _arr[4] = {bottom_btn, top_btn, front_btn, back_btn};

    // g_system_prefs.first_boot = 0;
    // memcpy(g_system_prefs.device_lang, "en", sizeof("en"));
    // g_system_prefs.units = 1;
    // g_system_prefs.theme = 1;
    // g_system_prefs.lower_bt_volume_on_walkie = 1;
    // g_system_prefs.auto_bt_on_powerup = 1;
    // g_system_prefs.device_auto_record = 0;
    // g_system_prefs.device_mute_record_audio = 0;
    

    if(zaire_systems_nvs_write_blob(sys_handle, ZAIRE_SYSTEMS_CFG_USR_NAMESPACE_KEY, &g_system_prefs, sizeof(g_system_prefs)) != ZAIRE_OK)return ZAIRE_ERR;

    return ZAIRE_OK;
}


// Compile-time date helpers
static int _get_compile_month(void)
{
    const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char mon[4] = {0};
    // __DATE__ format is "Mmm DD YYYY"
    mon[0] = __DATE__[0];
    mon[1] = __DATE__[1];
    mon[2] = __DATE__[2];
    const char *p = strstr(months, mon);
    return p ? (int)((p - months) / 3) + 1 : 1;
}

static int _get_compile_year(void)
{
    // __DATE__[7..10] is "YYYY"
    return (__DATE__[7] - '0') * 1000 +
           (__DATE__[8] - '0') * 100  +
           (__DATE__[9] - '0') * 10   +
           (__DATE__[10] - '0');
}

static int _get_compile_day(void)
{
    // __DATE__ format is "Mmm DD YYYY" — DD is at index 4..5, space-padded for single digits
    int d = (__DATE__[4] == ' ' ? 0 : __DATE__[4] - '0') * 10
          + (__DATE__[5] - '0');
    return d;
}

/**
 * @brief Default Settings Bootstrap: Initializes system settings if NVS is uninitialized.
 *        Detects erased flash state (0xFF), applies default values, and restarts device.
 *
 * @return ZAIRE_OK if bootstrap succeeds or not required.
 * @return ZAIRE_ERR if default settings fail to write.
 */
static zaire_err_t zaire_systems_default_settings_bootstrap(void){

    uint8_t _btn_map[4] = {bottom_btn, top_btn, front_btn, back_btn};
    if(settings_update_device_button_map(_btn_map) != ZAIRE_OK) return ZAIRE_ERR;
    if(settings_update_first_boot("0") != ZAIRE_OK) return ZAIRE_ERR;
    if(settings_update_user_name("ZAIRE") != ZAIRE_OK) return ZAIRE_ERR;
    if(settings_update_device_name("ZAIRE VISION") != ZAIRE_OK) return ZAIRE_ERR;

    strlcpy(g_device_info.manufacturer, "ZAIRE SYSTEMS", sizeof(g_device_info.manufacturer));
    strlcpy(g_device_info.model_name, "snowman", sizeof(g_device_info.model_name));
    strlcpy(g_device_info.firmware_version, "1.0.0", sizeof("1.0.0"));
    strlcpy(g_device_info.build_date, "2026-5-4", sizeof("2026-5-4"));
    strlcpy(g_device_info.build_type, "release", sizeof("release"));

    uint8_t mac[6];
    if(zaire_systems_network_init() != ZAIRE_OK)return ZAIRE_ERR;
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(g_device_info.device_id, sizeof(g_device_info.device_id),
    "ZS%02X%02d%02X%02d%02X%02d", //ZS [mac0] [day] [mac2] [month] [mac5] [year]
    mac[0],
    _get_compile_day(),
    mac[2],
    _get_compile_month(),
    mac[5],
    _get_compile_year() % 100);
    printf("%s\n", g_device_info.device_id);
    if(settings_update_system_info(&g_device_info) != ZAIRE_OK) return ZAIRE_ERR;
    
    return ZAIRE_OK;
}


static zaire_err_t zaire_systems_nvs_load_settings(void){

    if (zaire_systems_nvs_read_blob(sys_handle,
        ZAIRE_NVS_KEY_SYSTEM_PREFS,
        &g_system_prefs,
        sizeof(g_system_prefs)) != ZAIRE_OK) return ZAIRE_ERR;

    if (zaire_systems_nvs_read_blob(usr_handle,
        ZAIRE_NVS_KEY_USER_PREFS,
        &g_user_prefs,
        sizeof(g_user_prefs)) != ZAIRE_OK) return ZAIRE_ERR;

    if (zaire_systems_nvs_read_blob(sys_handle,
        ZAIRE_NVS_KEY_SYSTEM_SECURE,
        &g_sys_secure_info,
        sizeof(g_sys_secure_info)) != ZAIRE_OK) return ZAIRE_ERR;

    if (zaire_systems_nvs_read_blob(info_handle,
        ZAIRE_NVS_KEY_DEVICE_INFO,
        &g_device_info,
        sizeof(g_device_info)) != ZAIRE_OK) return ZAIRE_ERR;


    // not this yet since it contains the passcode, but will need to load this too eventually
    // if (zaire_systems_nvs_read_blob(sys_handle,
    //     ZAIRE_NVS_KEY_USER_SECURE,
    //     &g_usr_secure_info,
    //     sizeof(g_usr_secure_info)) != ZAIRE_OK) return ZAIRE_ERR;

    return ZAIRE_OK;
}


/**
 * @brief Init Function: Sets up NVS, SPIFFS, GPIOs, Camera, etc. Called once at startup from main.c.
 *        If any step fails, returns error and main.c will restart the device.
    * @return ZAIRE_OK if all init steps succeed, otherwise ZAIRE_ERR.
    * @return ZAIRE_ERR if any init step fails.
 */
zaire_err_t zaire_systems_init(void){

    if(buzzer_init() != ZAIRE_OK)return ZAIRE_ERR; //init alert system early so we can beep on errors during startup

    //Load and set settings from NVS
    if(zaire_systems_nvs_init() != ZAIRE_OK)return ZAIRE_ERR;
    zaire_err_t err = zaire_systems_nvs_read_blob(sys_handle, ZAIRE_NVS_KEY_SYSTEM_SECURE, &g_sys_secure_info, sizeof(g_sys_secure_info));
    if (err != ZAIRE_OK) {
        printf("No system config found - running default settings bootstrap...\n");
        vTaskDelay(pdMS_TO_TICKS(1500));

        if (zaire_systems_default_settings_bootstrap() != ZAIRE_OK) {
            printf("Error during default settings bootstrap!\n");

            while (1) {
                printf("FATAL: default settings bootstrap failed\n");
                error_beep();
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }

        printf("Bootstrap complete - restarting device...\n");
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();

        // Catch if restart returns unexpectedly
        while (1) {
            printf("FATAL: esp_restart() returned unexpectedly\n");
            error_beep();
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    if(zaire_systems_nvs_load_settings() != ZAIRE_OK)return ZAIRE_ERR;

    printf("btn: %d %d %d %d\n", PURPOSE_BTN, WALKIE_BTN, NXT_UP_BTN, PRV_DN_BTN);


    const char x[32];

    snprintf(x, sizeof(x), "%s", settings_get_device_id());

    printf("device id: %s\n", x);

    //init secondary devices (SPIFFS, GPIOs, Camera, etc...)
    if(zaire_systems_spiffs_init() != ZAIRE_OK)return ZAIRE_ERR;
    if(zaire_systems_gpio_btn_init() != ZAIRE_OK)return ZAIRE_ERR;
    if(zaire_systems_network_init() != ZAIRE_OK)return ZAIRE_ERR;
    if(zaire_systems_i2s_init() != ZAIRE_OK)return ZAIRE_ERR;
    //if(zaire_systems_uart_init() != ZAIRE_OK)return ZAIRE_ERR;
    return ZAIRE_OK;
}

zaire_err_t zaire_systems_spiffs_init(void){
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 20,                    // Increase if you have many files
        .format_if_mount_failed = true      // Keep true during development
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE("SPIFFS", "Partition 'storage' not found in partition table!");
        } else if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGE("SPIFFS", "SPIFFS mount failed - possibly corrupted");
        } else {
            ESP_LOGE("SPIFFS", "Failed to register SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ZAIRE_ERR;
    }

    //INFO
    // size_t total = 0, used = 0;
    // ret = esp_spiffs_info("storage", &total, &used);
    // if (ret != ESP_OK) {
    //     ESP_LOGE("SPIFFS", "Failed to get SPIFFS info");
    //     return ZAIRE_ERR;
    // }

    // printf("SPIFFS mounted successfully!\n");
    // printf("Total: %zu bytes (%.2f KB)\n", total, total / 1024.0);
    // printf("Used : %zu bytes (%.2f KB)\n", used, used / 1024.0);
    // printf("Free : %zu bytes\n", total - used);

    // List root directory for debugging
    DIR *d = opendir("/spiffs");
    if (d) {
        struct dirent *e;
        printf("Files in /spiffs:\n");
        while ((e = readdir(d)) != NULL) {
            printf("  - %s\n", e->d_name);
        }
        closedir(d);
    } else {
        printf("Warning: Could not open /spiffs directory\n");
    }

    return ZAIRE_OK;
}


zaire_err_t zaire_systems_gpio_btn_init(void){

    gpio_config_t i_p1_btn_conf = {
        .pin_bit_mask = (1ULL << PURPOSE_BTN) | (1ULL << WALKIE_BTN) | 
                        (1ULL << NXT_UP_BTN ) | (1ULL << PRV_DN_BTN) ,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };

    if(gpio_config(&i_p1_btn_conf) != ESP_OK) return ZAIRE_ERR;

    gpio_config_t o_trig_conf = {
        .pin_bit_mask = (1ULL << ZAIRE_BT_PP_GPIO) | (1ULL << RED_PAIRING_IND) | (1ULL << GREEN_PAIRING_IND),

          .mode       = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };

    if(gpio_config(&o_trig_conf) != ESP_OK) return ZAIRE_ERR;
    gpio_set_level(ZAIRE_BT_PP_GPIO, LOW); 

    return ZAIRE_OK;
}