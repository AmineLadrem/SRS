#ifndef SERVER_CONTROL_H
#define SERVER_CONTROL_H

#include <stdlib.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "spiffs_control.h"
#include "rtc_control.h"

static const char *server_tag = "MQTT_TCP";
char uri[128];
esp_mqtt_client_handle_t client;

void handle_cards_message(const char *data, size_t data_len)
{
    FILE *cards_fp = fopen("/lock_data/cards.txt", "w");
    if (cards_fp == NULL)
    {
        printf("Failed to open cards.txt for writing\n");
        return;
    }

    fprintf(cards_fp, "%s\n", data);
    fclose(cards_fp);
    ESP_LOGI(server_tag, "Received and saved card data to cards.txt");
}

void send_log_to_server()
{
    ESP_LOGI(server_tag, "Sending Log to the server");
    FILE *log_fp = fopen("/lock_data/log.txt", "r");
    if (log_fp == NULL)
    {
        printf("Failed to open log.txt for reading\n");
        return;
    }

    char log_line[256];
    int num_lines = 0;
    char lines_phrase[50];
    while (fgets(log_line, sizeof(log_line), log_fp))
    {
        num_lines++;
    }
    fclose(log_fp);
    snprintf(lines_phrase, sizeof(lines_phrase), "Number of lines is: %d", num_lines);
    esp_mqtt_client_publish(client, "log", lines_phrase, 0, 1, 0);
    log_fp = fopen("/lock_data/log.txt", "r");
    if (log_fp == NULL)
    {
        printf("Failed to open log.txt for reading\n");
        return;
    }
    while (fgets(log_line, sizeof(log_line), log_fp))
    {

        esp_mqtt_client_publish(client, "log", log_line, 0, 1, 0);
    }

    fclose(log_fp);

    log_fp = fopen("/lock_data/log.txt", "w");
    if (log_fp == NULL)
    {
        printf("Failed to open log.txt for writing\n");
        return;
    }

    fclose(log_fp);
    ESP_LOGI(server_tag, "The log was sent successfully");
}

void send_to_server(const char *topic, const char *data)
{

    esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:

        ESP_LOGI(server_tag, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "set_time", 0);
        esp_mqtt_client_subscribe(client, "300", 0);
        // send_log_to_server();
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(server_tag, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(server_tag, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(server_tag, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(server_tag, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(server_tag, "MQTT_EVENT_DATA");
        printf("\nTOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        if (strncmp(event->topic, "300", event->topic_len) == 0)
        {
            ESP_LOGI(server_tag, "Getting cards from server");
            handle_cards_message(event->data, event->data_len);
        }
        else if (strncmp(event->topic, "set_time", event->topic_len) == 0)
        {
            ESP_LOGI(server_tag, "Getting RTC from server");
            const char *epoch_time_str = event->data;
            set_time(epoch_time_str);
            ESP_LOGI(server_tag, "RTC was initialized");
            handle_cards_message(event->data, event->data_len);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(server_tag, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(server_tag, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    mqtt_event_handler_cb(event_data);
}

int server_connection()
{
    init_spiffs();

    char temp_uri[128];

    FILE *fp = fopen("/lock_data/MQTT_Params.txt", "r");
    if (fp == NULL)
    {
        printf("Failed to open file for reading\n");
        return -1;
    }
    char line[128];
    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, "uri") != NULL)
        {
            sscanf(line, "uri='%[^']'", temp_uri);
        }
    }
    fclose(fp);

    strcpy(uri, temp_uri);

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

     vTaskDelay(pdMS_TO_TICKS(200));

    return 0;
}

void server_disconnection()
{
    esp_mqtt_client_stop(client);
    esp_mqtt_client_destroy(client);
}

void update_cards()
{
    send_to_server("get_cards", "1");
}

#endif /* SERVER_CONTROL_H */