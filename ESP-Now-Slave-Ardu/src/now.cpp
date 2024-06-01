#include "now.h"

bool adressCompare(const uint8_t *addr1, const uint8_t *addr2);

/**********************************************************************************************/

bool nowInit() {

    WiFi.mode(WIFI_STA);
    if (esp_now_init())
        return false;

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    Serial.println(WiFi.macAddress());

    return true;
}

/**********************************************************************************************/

bool nowAddPeer(const uint8_t *address, uint8_t channel) {

    esp_now_peer_info_t peerInfo = {};

    memcpy(peerInfo.peer_addr, address, 6);
    peerInfo.channel = channel;

    if (esp_now_add_peer(&peerInfo))
        return false;
    return true;
}

/**********************************************************************************************/

// This callback is not used in Broadcast concept. It is left for future ideas.
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

/**********************************************************************************************/

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

    if (adressCompare(mac, adressObc)) {

        // if nextSendTime:
        if (len == sizeof(moduleData.obcState)) {

            memcpy((void*) &moduleData.obcState, (uint16_t *)incomingData, sizeof(moduleData.obcState));

            // if long time - go to sleep:
            if (fcn.stateInWakenUp()) moduleData.dataToObc.wakenUp = false;
            else moduleData.dataToObc.wakenUp = true;
        }

        // if different command:
        else fcn.rxNowHandler(incomingData, len);
    }
}

/**********************************************************************************************/

bool adressCompare(const uint8_t *addr1, const uint8_t *addr2) {

    for (int8_t i = 0; i < 6; i++) {

        if (addr1[i] != addr2[i])
            return false;
    }

    return true;
}