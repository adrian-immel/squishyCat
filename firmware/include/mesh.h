//
// Created by adrian on 03.05.25.
//

#ifndef MESH_H
#define MESH_H

#include <esp_now.h>
#include <WiFi.h>
#include <set>
#include <led.h>
#include <TaskSchedulerDeclarations.h>



// Message types
#define MSG_COLOR_SET 1
#define MSG_COLOR_UPDATE 2
#define MSG_Hello 3

// Maximum number of peers in ESP-NOW
#define MAX_PEERS 10

// sync interval
#define SYNC_INTERVAL 20000

// Structure for our messages
typedef struct mesh_message {
    uint8_t msgType;        // Message type: 1=COLOR_SET, 2=COLOR_UPDATE
    uint8_t colorValue;     // 8-bit color value
    uint8_t senderMac[6];   // MAC address of the original sender
    uint32_t messageId;     // Unique message ID to avoid duplicates
} mesh_message;

// External variable declarations
extern std::set<uint32_t> receivedMessages;
extern uint8_t knownPeers[MAX_PEERS][6];
extern int peerCount;
extern uint32_t messageCounter;
extern uint8_t currentColor;
extern uint8_t myMac[6];
extern uint8_t broadcastAddress[6];

// Network management functions
void initMeshNetwork(Scheduler &runner);
bool addPeer(const uint8_t *mac_addr);
bool isMessageDuplicate(uint32_t messageId);

// ESP-NOW callback functions
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);

// Message processing functions
void processColorMessage(const mesh_message* msg);

// Message sending functions
void sendColorSetMessage(uint8_t colorValue);
void sendColorUpdateMessage();

#endif //MESH_H
