/**
 * @brief Global includes, shared macros, and task declarations for Zaire
 *
 * This file pulls in the main system headers, ESP-IDF libraries,
 * driver headers, and shared project-wide definitions used across
 * the codebase.
 *
 * It also exposes:
 *  - buzzer config + function declarations
 *  - shared task timing macros
 *  - task handles
 *  - task function prototypes
 *
 * Basically: this is the common header that other files include
 * so they all have access to the same core stuff.
 *
 * @note
 * Since a lot of files depend on this header, changes here can ripple
 * across the whole project.
 *
 * @warning
 * Be careful not to overload this file too much — global headers can
 * get messy fast and increase compile dependencies.
 *
 * @author Yann Kabambi
 * @project ZAIRE SYSTEMS
 */

#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>

#include <nvs.h>
#include <nvs_flash.h>

#include <sys/unistd.h>
#include <sys/stat.h>

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>
#include <freertos/ringbuf.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

#include <esp_flash.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include <esp_timer.h>
#include <esp_now.h>
#include <esp_mac.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_spiffs.h>
#include <esp_event.h>
#include <esp_vfs.h>
#include <esp_vfs_fat.h>
#include <esp_http_server.h>
#include <esp_netif_ip_addr.h>
#include <esp_system.h>

#include <lwip/sockets.h>
#include <lwip/inet.h>

#include <driver/gpio.h>
#include <driver/i2c.h>
#include <driver/i2s.h>
#include <driver/uart.h>
#include <driver/ledc.h>


// Include ZAIRE SYSTEMS
#include "zaire_systems.h"


//BUZZER
#define BUZZER_PIN          GPIO_NUM_23
#define BUZZER_CHANNEL      LEDC_CHANNEL_0
#define BUZZER_TIMER        LEDC_TIMER_0
#define BUZZER_SPEED_MODE   LEDC_LOW_SPEED_MODE
zaire_err_t buzzer_init(void);
void init_beep(void);
void shutdown_beep(void);
void single_beep(void);
void double_beep(void);
void error_beep(void);

//Task
#define TASK_HOLD_DELAY             (10)
#define TASK_HOLD_DELAY_SEC         (1000)
#define TASK_HOLD_DELAY_FIVE_SEC    (1000 * 5)
extern TaskHandle_t purpose_btn_task_handler;
extern TaskHandle_t walkie_btn_task_handler;
extern TaskHandle_t nxt_up_btn_task_handler;
extern TaskHandle_t prv_dn_btn_task_handler;
extern TaskHandle_t zaire_systems_red_light_ind_task_handler;
extern TaskHandle_t zaire_systems_green_light_ind_task_handler;
extern TaskHandle_t zaire_systems_walkie_task_handler;

void purpose_btn_task_run(void *vpParam);
void walkie_btn_task_run(void *vpParam);
void nxt_up_btn_task_run(void *vpParam);
void prv_dn_btn_task_run(void *vpParam);
void bt_device_task_run(void *vpParam);
void walkie_task_run(void *vpParm);
void green_task_run(void *vpParm);
void red_task_run(void *vpParm);
void zaire_systems_walkie_task_run(void *vpParam);

#endif //GLOBAL_VAR_H