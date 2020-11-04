#ifndef __BT_H__
#define __BT_H__

struct transmitInfo{
    int header;
    int size;
};

void btInit();
int btRegister();
void sendData(uint8_t* data,int len);
void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

#endif