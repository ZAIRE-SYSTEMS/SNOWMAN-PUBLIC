#ifndef ZAIRE_SYSTEMS_BT1026_COMMAND_LIST_H
#define ZAIRE_SYSTEMS_BT1026_COMMAND_LIST_H


//=========== FSC-BT1026C AT Commands ===========
#define BT_CMD_NEXT             "AT+FORWARD\r\n"
#define BT_CMD_PREV             "AT+BACKWARD\r\n"
#define BT_CMD_PLAY_PAUSE       "AT+PLAYPAUSE\r\n"
#define BT_CMD_PLAY             "AT+PLAY\r\n"
#define BT_CMD_PAUSE            "AT+PAUSE\r\n"
#define BT_CMD_STOP             "AT+STOP\r\n"
#define BT_CMD_MUTE_SPK_ON      "AT+SPKMUTE=1\r\n"
#define BT_CMD_MUTE_SPK_OFF     "AT+SPKMUTE=0\r\n"
#define BT_CMD_MUTE_MIC_ON      "AT+MICMUTE=1\r\n"
#define BT_CMD_MUTE_MIC_OFF     "AT+MICMUTE=0\r\n"
#define BT_CMD_VOICE_ASSIST_ON  "AT+HFPVR=1\r\n"
#define BT_CMD_VOICE_ASSIST_OFF "AT+HFPVR=0\r\n"
#define BT_CMD_DISCOVERABLE_ON  "AT+PAIR=1\r\n"
#define BT_CMD_DISCOVERABLE_OFF "AT+PAIR=0\r\n"
#define BT_CMD_BT_ON            "AT+BTEN=1\r\n"
#define BT_CMD_BT_OFF           "AT+BTEN=0\r\n"
#define BT_CMD_DISCONNECT_ALL   "AT+DSCA\r\n"
#define BT_CMD_GET_STATE        "AT+STAT\r\n"
#define BT_CMD_GET_VERSION      "AT+VER\r\n"
#define BT_CMD_REBOOT           "AT+REBOOT\r\n"
#define BT_CMD_FACTORY_RESET    "AT+RESTORE\r\n"


#endif