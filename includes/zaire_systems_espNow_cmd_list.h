#ifndef ZAIRE_SYSTEMS_ESP_NOW_CMD_LIST_H
#define ZAIRE_SYSTEMS_ESP_NOW_CMD_LIST_H


#define EMPTY                           0xFF
#define REQUEST_STA_ADDR                0x0A
#define WALKIE_PAIRING_START            0xB0 //Alert pairing session has begun
#define WALKIE_PAIRING_DONE             0xB1 //Alert pairing session done
#define WALKIE_PAIRING_CANCEL           0xB3 //Alert pairing session being canceled
#define WALKIE_REMOVE_PEER_P0           0xC0 //Alert other devices to remove us from mesh - POSITION 0 [peer_mesh_position]
#define WALKIE_REMOVE_PEER_P1           0xC1 //Alert other devices to remove us from mesh - POSITION 1 [peer_mesh_position]
#define WALKIE_REMOVE_PEER_P2           0xC2 //Alert other devices to remove us from mesh - POSITION 2 [peer_mesh_position]
#define WALKIE_REMOVE_PEER_P3           0xC3 //Alert other devices to remove us from mesh - POSITION 3 [peer_mesh_position]
#define WALKIE_REMOVE_PEER_P4           0xC4 //Alert other devices to remove us from mesh - POSITION 4 [peer_mesh_position]
#define WALKIE_REMOVE_PEER_P5           0xC5 //Alert other devices to remove us from mesh - POSITION 5 [peer_mesh_position]
#define WALKIE_REMOVE_PEER_P6           0xC6 //Alert other devices to remove us from mesh - POSITION 6 [peer_mesh_position]
#define WALKIE_REMOVE_PEER_P7           0xC7 //Alert other devices to remove us from mesh - POSITION 7 [peer_mesh_position]
#define WALKIE_REMOVE_PEER_P8           0xC8 //Alert other devices to remove us from mesh - POSITION 8 [peer_mesh_position]

#endif //ZAIRE_SYSTEMS_ESP_NOW_CMD_LIST_H