#include <WiFi.h>
#include "esp_wpa2.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "secrets.h"

// Freenove ESP32-WROVER CAM pin configuration
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

// Use credentials from secrets.h
const char* ssid = EAP_SSID;
const char* eap_identity = EAP_IDENTITY;
const char* eap_password = EAP_PASSWORD;

httpd_handle_t stream_httpd = NULL;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static esp_err_t stream_handler(httpd_req_t *req) {
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    char part_buf[64];

    res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
    if (res != ESP_OK) return res;

    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
        } else {
            size_t hlen = snprintf(part_buf, 64, STREAM_PART, fb->len);
            res = httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));
            if (res == ESP_OK) {
                res = httpd_resp_send_chunk(req, part_buf, hlen);
            }
            if (res == ESP_OK) {
                res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
            }
            esp_camera_fb_return(fb);
        }
        if (res != ESP_OK) break;
    }
    return res;
}

static esp_err_t index_handler(httpd_req_t *req) {
    const char* html = "<!DOCTYPE html><html><head><title>ESP32-CAM</title></head>"
                       "<body style='text-align:center;background:#1a1a1a;color:#fff;font-family:sans-serif;'>"
                       "<h1>ESP32 Camera Stream</h1>"
                       "<img src='/stream' style='max-width:100%;border:2px solid #444;'>"
                       "</body></html>";
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, html, strlen(html));
}

void startCameraServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    httpd_uri_t index_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = index_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t stream_uri = {
        .uri       = "/stream",
        .method    = HTTP_GET,
        .handler   = stream_handler,
        .user_ctx  = NULL
    };

    if (httpd_start(&stream_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(stream_httpd, &index_uri);
        httpd_register_uri_handler(stream_httpd, &stream_uri);
        Serial.println("Camera server started");
    }
}

bool initCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_VGA;  // 640x480
    config.jpeg_quality = 12;           // 0-63, lower is higher quality
    config.fb_count = 2;                // Double buffer for smoother streaming
    config.grab_mode = CAMERA_GRAB_LATEST;

    // Use PSRAM if available for larger frame buffers
    if (psramFound()) {
        config.frame_size = FRAMESIZE_SVGA;  // 800x600
        config.jpeg_quality = 10;
        config.fb_count = 2;
        Serial.println("PSRAM found, using higher resolution");
    } else {
        config.frame_size = FRAMESIZE_VGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
        Serial.println("No PSRAM, using lower resolution");
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        return false;
    }

    Serial.println("Camera initialized successfully");
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n\n=== ESP32 Camera Stream ===\n");

    // Initialize camera first
    if (!initCamera()) {
        Serial.println("Camera init failed! Check wiring and restart.");
        while (1) delay(1000);
    }

    // Connect to WiFi (WPA2 Enterprise)
    Serial.print("Connecting to: ");
    Serial.println(ssid);

    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);

    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)eap_identity, strlen(eap_identity));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)eap_identity, strlen(eap_identity));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)eap_password, strlen(eap_password));
    esp_wifi_sta_wpa2_ent_enable();

    WiFi.begin(ssid);

    int counter = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        counter++;
        if (counter >= 60) {
            Serial.println("\nWiFi connection failed! Rebooting...");
            ESP.restart();
        }
    }

    Serial.println("\n\nWiFi connected!");
    Serial.print("Stream URL: http://");
    Serial.println(WiFi.localIP());

    // Start the camera web server
    startCameraServer();
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected, restarting...");
        ESP.restart();
    }
    delay(10000);
}
