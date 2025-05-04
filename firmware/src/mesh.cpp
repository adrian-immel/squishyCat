//
// Created by adrian on 03.05.25.
//
#include "mesh.h"

// Global variables initialization
std::set<uint32_t> receivedMessages;    // Stores IDs of already received messages
uint8_t knownPeers[MAX_PEERS][6]; // Stores MAC addresses of known peers
int peerCount = 0;                      // Number of known peers
uint32_t messageCounter = 0;            // Counter for outgoing messages
uint8_t currentColor = 0;               // Current color value of the node
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Broadcast address
uint8_t myMac[6];                       // MAC address of this device
Task syncLEDsTask(SYNC_INTERVAL + random(0, 2000), TASK_FOREVER, &sendColorUpdateMessage);


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

  // Add broadcast peer for initial discovery
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add broadcast peer");
    return;
  }

  // Initial announcement in the network
  Serial.println("ESP-NOW Mesh Network started");

  // Send an initial broadcast message to announce presence
  mesh_message announceMsg;
  announceMsg.msgType = MSG_Hello;
  announceMsg.colorValue = 0;
  memcpy(announceMsg.senderMac, myMac, 6);
  announceMsg.messageId = messageCounter++;

  esp_now_send(broadcastAddress, reinterpret_cast<uint8_t*>(&announceMsg), sizeof(mesh_message));

  // setup periodic message send task
  runner.addTask(syncLEDsTask);
  syncLEDsTask.enable();
}

// Callback function for sent data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("Error sending message");
    // Here you could mark the peer as unreliable or remove it
  }
}

// Check if the message has already been received
bool isMessageDuplicate(uint32_t messageId) {
  if (receivedMessages.find(messageId) != receivedMessages.end()) {
    return true;
  }
  // Add a message to the list
  receivedMessages.insert(messageId);
  // Limit list size (optional) to control memory usage
  if (receivedMessages.size() > 100) {
    receivedMessages.erase(receivedMessages.begin());
  }
  return false;
}

// Add peer to the network
bool addPeer(const uint8_t *mac_addr) {
  // Check if the peer is already known
  for (int i = 0; i < peerCount; i++) {
    bool match = true;
    for (int j = 0; j < 6; j++) {
      if (knownPeers[i][j] != mac_addr[j]) {
        match = false;
        break;
      }
    }
    if (match) return true; // Peer already known
  }

  // Maximum reached?
  if (peerCount >= MAX_PEERS) {
    Serial.println("Maximum number of peers reached");
    return false;
  }

  // Register new peer in ESP-NOW
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, mac_addr, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return false;
  }

  // Add peer to local list
  for (int i = 0; i < 6; i++) {
    knownPeers[peerCount][i] = mac_addr[i];
  }
  peerCount++;

  Serial.print("New peer added: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac_addr[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  return true;
}

// Process a color message based on its type
void processColorMessage(const mesh_message* msg) {
  if (msg->msgType == MSG_COLOR_SET || msg->msgType == MSG_COLOR_UPDATE) {
    updateLed(msg->colorValue, false);
    Serial.print("Color updated received: ");
    Serial.println(msg->colorValue);
  }
  else if (msg->msgType == MSG_Hello)
  {
    Serial.printf("Hello from:%02X%02X%02X%02X%02X%02X\n", msg->senderMac[0], msg->senderMac[1], msg->senderMac[2],
              msg->senderMac[3], msg->senderMac[4], msg->senderMac[5]);
  }else
  {
    Serial.printf("Unknown message type: %d\n", msg->msgType);
  }

}

// Callback function for received data
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, const int data_len) {
  if (data_len != sizeof(mesh_message)) {
    Serial.println("Invalid message size");
    return;
  }

  auto *msg = (mesh_message*)data;

  // Duplicate check
  if (isMessageDuplicate(msg->messageId)) {
    return; // Message has already been processed
  }

  // Add new peer to the mesh
  addPeer(mac_addr);

  // Process the message
  processColorMessage(msg);

  // Forward the message to all other peers (except sender)
  Serial.println("Forwarding message...");
  for (int i = 0; i < peerCount; i++) {
    bool isSender = true;
    for (int j = 0; j < 6; j++) {
      if (knownPeers[i][j] != mac_addr[j]) {
        isSender = false;
        break;
      }
    }

    if (!isSender) {
      esp_now_send(knownPeers[i], reinterpret_cast<uint8_t*>(msg), sizeof(mesh_message));
    }
  }
}

// Send a color set message to all peers
void sendColorSetMessage(uint8_t colorValue) {
  mesh_message msg;
  msg.msgType = MSG_COLOR_SET;
  msg.colorValue = colorValue;
  memcpy(msg.senderMac, myMac, 6);
  msg.messageId = messageCounter++;
  Serial.println("sending color set message...");

  // Send the message to all known peers
  for (int i = 0; i < peerCount; i++) {
    esp_now_send(knownPeers[i], reinterpret_cast<uint8_t*>(&msg), sizeof(mesh_message));
  }
}

// Send a color update message to all peers
void sendColorUpdateMessage() {
  mesh_message msg;
  msg.msgType = MSG_COLOR_UPDATE;
  msg.colorValue = getCurrentHue();
  memcpy(msg.senderMac, myMac, 6);
  msg.messageId = messageCounter++;
  Serial.println("sending color update message...");


  // Send the message to all known peers
  for (int i = 0; i < peerCount; i++) {
    esp_now_send(knownPeers[i], reinterpret_cast<uint8_t*>(&msg), sizeof(mesh_message));
  }
}
