#include "globalVar.h"


bool is_bluetooth_discoverable = 0;
bool is_bluetooth_paired = 0;



void bluetooth_discoverable_on(void){
    //Adiscoverable and connectable
    single_beep();
    is_bluetooth_discoverable = 1;
    green_ind_time = CYCLE_2;
}

void bluetooth_discoverable_off(void){
    //connectable only
    double_beep();
    is_bluetooth_discoverable = 0;
    green_ind_time = OFF;
}

void bluetooth_set_device_name(const char *name){
    char cmd[48];
    snprintf(cmd, sizeof(cmd), "AT+NAME=%s\r\n", name);
}

void bluetooth_set_device_vol(uint8_t vol){
    if (vol > 15) vol = 15;
    char cmd[20];
    snprintf(cmd, sizeof(cmd), "AT+SPKVOL=%d\r\n", vol);
}


void bluetooth_pp_action(void){
    gpio_set_level(ZAIRE_BT_PP_GPIO, HIGH);
    vTaskDelay(pdMS_TO_TICKS(150));
    gpio_set_level(ZAIRE_BT_PP_GPIO, LOW);
    vTaskDelay(pdMS_TO_TICKS(150));
}
void bluetooth_nxt_track(void){
    printf("nxt track\n");
}

void bluetooth_prv_track(void){
    printf("prv track\n");

}

void bluetooth_vol_up(void){
    
    while (gpio_get_level(NXT_UP_BTN) == LOW)
    {
        printf("vol up\n");
        vTaskDelay(pdMS_TO_TICKS(1500));
    }

}

void bluetooth_vol_dn(void){
    while (gpio_get_level(PRV_DN_BTN) == LOW)
    {
        printf("vol dn\n");
        vTaskDelay(pdMS_TO_TICKS(1500));
    }

}