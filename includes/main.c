/**
 * @brief Main entry point for Zaire
 *
 * This is where the system boots up.
 * It initializes flash/NVS, starts the main Zaire system,
 * launches the button tasks, and plays the startup beep.
 *
 * Startup flow:
 *  - init NVS flash
 *  - init Zaire system
 *  - start button tasks
 *  - play startup sound
 *
 * Basically: this is the boot sequence of the system.
 *
 * @note
 * If init fails, the device restarts.
 *
 * @warning
 * Since this is the main startup path, any failure here can stop
 * the whole system from booting properly.
 *
 * @author Yann Kabambi
 * @project ZAIRE SYSTEMS
 */

#include "globalVar.h"


void app_main(void)
{

    zaire_err_t ret;
    // Initialize NVS -- ALWAYS FIRST
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if(ret != ZAIRE_OK)esp_restart();

    // Initialize ZAIRE Systems
    if(zaire_systems_init() != ZAIRE_OK){printf("INIT Failed!\n");vTaskDelay(4000 / portTICK_PERIOD_MS);esp_restart();}

    xTaskCreate(purpose_btn_task_run,"purpose_btn_task_run", 4096, NULL, 4, &purpose_btn_task_handler);
    xTaskCreate(walkie_btn_task_run, "walkie_btn_task_run",  4096, NULL, 4, &walkie_btn_task_handler);
    xTaskCreate(nxt_up_btn_task_run, "nxt_up_btn_task_run",  4096, NULL, 4, &nxt_up_btn_task_handler);
    xTaskCreate(prv_dn_btn_task_run, "prv_dn_btn_task_run",  4096, NULL, 4, &prv_dn_btn_task_handler);
    xTaskCreate(green_task_run, "green_task_run",  2048, NULL, 4, &zaire_systems_green_light_ind_task_handler);
    xTaskCreate(red_task_run, "red_task_run",  2048, NULL, 4, &zaire_systems_red_light_ind_task_handler);
    xTaskCreate(zaire_systems_walkie_task_run, "zaire_systems_walkie_task_run",  8192, NULL, 4, &zaire_systems_walkie_task_handler);
    
    

    vTaskDelay(pdMS_TO_TICKS(10)); // Short delay to ensure all systems are up before proceeding
    //ALERT PWR ON
    init_beep();
}
