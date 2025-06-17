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
#define MSG_COLOR_SYNC 2

// Maximum number of peers in ESP-NOW
#define MAX_PEERS 10

// sync interval
#define SYNC_INTERVAL 1500

// Structure to hold message data
struct mesh_message {
    uint8_t msgType;       // Type of message
    uint8_t colorValue;    // Color value being sent
    uint8_t senderMac[6];  // MAC address of sender
    uint32_t messageId;    // Unique message ID
};

// Function declarations
void initMeshNetwork(Scheduler &runner);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
bool isMessageDuplicate(uint32_t messageId);
void processColorMessage(const mesh_message* msg);

// Message sending functions
void sendColorSetMessage(uint8_t colorValue);
void sendColorSyncMessage();

#endif //MESH_H