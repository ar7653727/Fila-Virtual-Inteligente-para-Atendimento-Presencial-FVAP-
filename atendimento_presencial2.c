// Código 2 - Atendimento Presencial

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include <string.h>

// Definições dos pinos da BitDogLab
#define BUTTON_A 5
#define BUTTON_B 6
#define BUZZER 10
#define LED_RGB 13
#define OLED_SDA 14
#define OLED_SCL 15
#define WIFI_SSID "NETWORK REIS"
#define WIFI_PASS "Zsd@142721"

int senha_atual = 0;
bool chamada_pausada = false;
char http_response[1024];

// Função para exibir a senha na web
void create_http_response() {
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
             "<!DOCTYPE html>"
             "<html>"
             "<head><meta charset=\"UTF-8\">"
             "<title>Fila Virtual</title>"
             "<script>setTimeout(() => location.reload(), 5000);</script>"
             "</head>"
             "<body>"
             "<h1>Fila Virtual</h1>"
             "<h2>Senha Atual: %d</h2>"
             "<p>%s</p>"
             "</body></html>",
             senha_atual, chamada_pausada ? "Chamada pausada" : "Aguardando atendimento...");
}

// Função de callback HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    create_http_response();
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
    pbuf_free(p);
    return ERR_OK;
}

// Callback de conexão do servidor HTTP
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

// Inicializa o servidor HTTP
static void start_http_server() {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb || tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) return;
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
}

// Simula impressão na Epson 4260 via Wi-Fi
void imprimir_senha() {
    printf("Imprimindo: Senha %d\n", senha_atual);
}

// Gera nova senha
void gerar_senha() {
    senha_atual++;
    gpio_put(LED_RGB, 1);
    imprimir_senha();
    sleep_ms(500);
    gpio_put(LED_RGB, 0);
}

// Chama a senha
void chamar_senha() {
    if (!chamada_pausada) {
        printf("Chamando senha %d\n", senha_atual);
        for (int i = 0; i < 3; i++) {
            gpio_put(BUZZER, 1);
            sleep_ms(200);
            gpio_put(BUZZER, 0);
            sleep_ms(200);
        }
    }
}

int main() {
    stdio_init_all();
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_init(LED_RGB);
    gpio_set_dir(LED_RGB, GPIO_OUT);
    gpio_put(LED_RGB, 0);

    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
    cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000);
    start_http_server();

    while (true) {
        if (gpio_get(BUTTON_A) == 0) {
            gerar_senha();
            chamar_senha();
            sleep_ms(500);
        }
        if (gpio_get(BUTTON_B) == 0) {
            chamada_pausada = !chamada_pausada;
            printf("Chamada %s\n", chamada_pausada ? "pausada" : "retomada");
            sleep_ms(500);
        }
        sleep_ms(100);
    }
    return 0;
}
