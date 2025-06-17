#include "mesh.h"

// Global variables initialization
std::set<uint32_t> receivedMessages;    // Stores IDs of already received messages
uint32_t messageCounter = 0;            // Counter for outgoing messages
uint8_t currentColor = 0;               // Current color value of the node
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Broadcast address
uint8_t myMac[6];                       // MAC address of this device
Task syncLEDsTask(SYNC_INTERVAL + random(0, 250), TASK_FOREVER, &sendColorSyncMessage);
unsigned long lastSetTime;

// Initialize the mesh network
void initMeshNetwork(Scheduler &runner) {
    // Initialize WiFi in station mode
    WiFiClass::mode(WIFI_STA);

    // Store MAC address
    WiFi.macAddress(myMac);
    Serial.print("My MAC address: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(myMac[i], HEX);
        if (i < 5) Serial.print(":");
    }
    Serial.println();

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register callbacks
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // Add broadcast peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add broadcast peer");
        return;
    }

    Serial.println("ESP-NOW Mesh Network started");

    // setup periodic message send task
    runner.addTask(syncLEDsTask);
    lastSetTime = 0;
    syncLEDsTask.enable();
}

// callback for sent data
void OnDataSent(const uint8_t *mac_addr, const esp_now_send_status_t status) {
    if (status != ESP_NOW_SEND_SUCCESS) {
        Serial.println("Error sending message");
    }
}

// Check if the message has already been received
bool isMessageDuplicate(const uint32_t messageId) {
    if (receivedMessages.find(messageId) != receivedMessages.end()) {
        return true;
    }
    receivedMessages.insert(messageId);
    if (receivedMessages.size() > 64) {
        receivedMessages.erase(receivedMessages.begin());
    }
    return false;
}

// Process a color message based on its type
void processColorMessage(const mesh_message* msg) {
    if (msg->msgType == MSG_COLOR_SET) {
        MeshColorChange(msg->colorValue);
        lastSetTime = millis();
        Serial.print("Color set received: ");
        Serial.println(msg->colorValue);
    } else if (msg->msgType == MSG_COLOR_SYNC) {
        if (lastSetTime + 500 < millis())
        {
            MeshColorChange(msg->colorValue);
            Serial.print("Color sync received: ");
            Serial.println(msg->colorValue);
        }
    } else {
        Serial.printf("Unknown message type: %d\n", msg->msgType);
    }
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, const int data_len) {
    if (data_len != sizeof(mesh_message)) {
        Serial.println("Invalid message size");
        return;
    }

    const mesh_message* msg = (mesh_message*)data;

    // Duplicate check
    if (isMessageDuplicate(msg->messageId)) {
        return; // Message has already been processed
    }

    // Process the message
    processColorMessage(msg);

    // Rebroadcast the message if Set Packet
    if (msg->msgType == MSG_COLOR_SET) esp_now_send(broadcastAddress, data, data_len);

}

void sendColorSetMessage(const uint8_t colorValue) {
    mesh_message msg{};
    msg.msgType = MSG_COLOR_SET;
    msg.colorValue = colorValue;
    memcpy(msg.senderMac, myMac, 6);
    msg.messageId = esp_random();
    Serial.println("sending color set message...");
    esp_now_send(broadcastAddress, reinterpret_cast<uint8_t*>(&msg), sizeof(mesh_message));
    //delay(50); //rebroadcast message 2 times
    //esp_now_send(broadcastAddress, reinterpret_cast<uint8_t*>(&msg), sizeof(mesh_message));
    lastSetTime = millis();
}

void sendColorSyncMessage() {
    if (isMovingToTargetHue || millis() < 2000) return; // skip sync if moving to Target Hue || directly after init
    mesh_message msg{};
    msg.msgType = MSG_COLOR_SYNC;
    msg.colorValue = getCurrentHue();
    memcpy(msg.senderMac, myMac, 6);
    msg.messageId = esp_random();;
    Serial.println("sending color update message...");

    esp_now_send(broadcastAddress, reinterpret_cast<uint8_t*>(&msg), sizeof(mesh_message));
}