#include <esp_log.h>
#include <webserver.h>

extern volatile int led1_state;
extern volatile int led2_state;

extern void Toggle_Led(uint8_t led);

void wifi_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("WiFi", "Connecting to WiFi...");
    ESP_ERROR_CHECK(esp_wifi_connect());
}

esp_err_t serve_index_html(httpd_req_t *req) {
    // Mở tệp "index.html" từ SPIFFS
    const char *path = "/spiffs/index.html";
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        // Xử lý lỗi khi mở tệp
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to open file");
        return ESP_FAIL;
    }

    // Gửi nội dung của tệp "index.html" về máy khách
    char buffer[1024];
    size_t read_bytes;
    do {
        read_bytes = fread(buffer, 1, sizeof(buffer), file);
        if (read_bytes > 0) {
            httpd_resp_send_chunk(req, buffer, read_bytes);
        }
    } while (read_bytes > 0);

    // Đóng tệp
    fclose(file);

    // Kết thúc gửi nội dung
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/* Xử lý yêu cầu GET cho endpoint "/led" */
esp_err_t led_get_handler(httpd_req_t *req)
{
    char response_data[40];
    snprintf(response_data, sizeof(response_data), "{\"led1\": %s, \"led2\": %s}", 
             led1_state ? "true" : "false", led2_state ? "true" : "false");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response_data, strlen(response_data));
    return ESP_OK;
}

/* Xử lý yêu cầu POST cho endpoint "/toggle-led1" */
esp_err_t toggle_led1_post_handler(httpd_req_t *req)
{
    // Đảo trạng thái của Relay 1
    Toggle_Led(1);

    // Gửi trạng thái mới của Relay 1 dưới dạng JSON
    char response_data[20];
    snprintf(response_data, sizeof(response_data), "{\"led1\": %s}", led1_state ? "true" : "false");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response_data, strlen(response_data));
    return ESP_OK;
}

/* Xử lý yêu cầu POST cho endpoint "/toggle-led2" */
esp_err_t toggle_led2_post_handler(httpd_req_t *req)
{
    // Đảo trạng thái của Relay 1
    Toggle_Led(2);

    // Gửi trạng thái mới của Relay 1 dưới dạng JSON
    char response_data[20];
    snprintf(response_data, sizeof(response_data), "{\"led2\": %s}", led1_state ? "true" : "false");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response_data, strlen(response_data));
    return ESP_OK;
}

void webserver_init(void){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Khởi tạo và bắt đầu web server
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    /* Đăng ký xử lý yêu cầu GET cho tệp "index.html" */
    httpd_uri_t index_html = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = serve_index_html,
        .user_ctx = NULL
    };

    // Đăng ký xử lý yêu cầu GET cho endpoint "/led1"
    httpd_uri_t led_uri = {
        .uri = "/led",
        .method = HTTP_GET,
        .handler = led_get_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &led_uri);

    /* Đăng ký xử lý yêu cầu POST cho các endpoint */
        httpd_uri_t toggle_relay1_uri = {
            .uri = "/toggle-led1",
            .method = HTTP_POST,
            .handler = toggle_led1_post_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &toggle_relay1_uri);

        httpd_uri_t toggle_relay2_uri = {
            .uri = "/toggle-led2",
            .method = HTTP_POST,
            .handler = toggle_led2_post_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &toggle_relay2_uri);

    
    httpd_register_uri_handler(server, &index_html);
}