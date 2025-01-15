#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_http_server.h>
#include <nvs_flash.h>

#define WIFI_SSID "minhhchienn and friends"
#define WIFI_PASS "71846584"

#define WIFI_CONNECTED       0
#define WIFI_DISCONNECTED    1
#define WIFI_CONNECTING      2

void wifi_init(void);

void webserver_init(void);

#endif // __WEBSERVER_H__