// Projeto jogo Pico Space Impact para Raspberry Pi Pico
// Autor: Christian Danner Ramos de Carvalho
// 04/03/2025
// e-mail: eng.chrisdanner@gmail.com

#include "pico/stdlib.h"  // Biblioteca padrão do Raspberry Pi Pico
#include <ssd1306.h>      // Biblioteca para o display OLED SSD1306
#include <stdio.h>        // Biblioteca padrão para entrada/saída
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "stdlib.h"

#define I2C_PORT i2c1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Definição dos GPIOs
#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_SW_PIN 22
#define BUTTON_A_PIN 6 // Botão B trocado para facilitar o tiro com a mão direita
#define BUTTON_B_PIN 5 // Botão A implementar o select ou pause 
#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12
#define BUZZER_A_PIN 21
#define BUZZER_B_PIN 10
#define MAX_BULLETS 10
#define MAX_ENEMIES 100

// Estrutura para representar a nave
typedef struct {
    int x, y;
} Ship;

typedef struct {
    int x, y;
    bool active;
} Bullet;

typedef struct {
    int x, y;
    bool active;
} Enemy;

Ship player = {10, 32}; // Inicializa a nave na posição (10, 32)
Ship menu = {100, 31}; // Posição da nave no menu (98, 35)
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
ssd1306_t oled; // Estrutura do display OLED
int score = 0; // Variável para armazenar o score
bool game_started = false; // Variável para controlar se o jogo começou
int enemy_count = 50; // Quantidade inicial de inimigos

// Bitmap da nave (10x10 pixels)
const uint8_t ship_bitmap[10][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 1, 0, 1, 0, 0},
    {1, 1, 0, 1, 1, 1, 1, 0, 1, 1},
    {0, 0, 1, 0, 1, 1, 0, 1, 0, 0},
    {0, 1, 1, 0, 1, 1, 1, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Bitmap da nave inimiga (8x10 pixels)
const uint8_t enemy_bitmap[10][8] = {
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 0, 1, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 0, 1, 1, 0, 1, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

// Configuração do hardware
void setup_hardware() {
    stdio_init_all();
    sleep_ms(2000);

    // Configurar I2C e OLED
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
    
    oled.external_vcc = false;
    ssd1306_init(&oled, SCREEN_WIDTH, SCREEN_HEIGHT, 0x3C, I2C_PORT);
    ssd1306_clear(&oled);
    
    // Configurar ADC para o joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    
    // Configurar botões
    gpio_init(JOYSTICK_SW_PIN);
    gpio_set_dir(JOYSTICK_SW_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_SW_PIN);
    
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    
    // Configurar LED RGB
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    
    // Acender LED verde ao ligar
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);

    // Configurar buzzer A 
    gpio_init(BUZZER_A_PIN);

    // Configurar buzzer A (PWM para som dos tiros)
    gpio_set_function(BUZZER_A_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_A_PIN);
    pwm_set_enabled(slice_num, true);

    // Configurar buzzer B (som grave ao atingir inimigo)
    gpio_init(BUZZER_B_PIN);

    gpio_set_function(BUZZER_B_PIN, GPIO_FUNC_PWM);
    uint slice_num_b = pwm_gpio_to_slice_num(BUZZER_B_PIN);
    pwm_set_enabled(slice_num_b, true);

    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].x = 100 + (i * 20);
        enemies[i].y = 10 + (i * 15);
        enemies[i].active = true;
    }
}

// Toca uma música de tema usando o Buzzer A
void play_theme_music() {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_A_PIN);
    pwm_set_wrap(slice_num, 10000); // Frequência base
    pwm_set_clkdiv(slice_num, 100.0);

    // Notas da música (frequências)
    int notes[] = {262, 330, 392, 523}; // Dó, Mi, Sol, Dó (oitava acima)
    int durations[] = {200, 200, 200, 400}; // Duração de cada nota
    
    for (int i = 0; i < 4; i++) {
        pwm_set_wrap(slice_num, 1000000 / notes[i]); // Define a frequência da nota
        pwm_set_gpio_level(BUZZER_A_PIN, 500000 / notes[i]); // 50% duty cycle
        sleep_ms(durations[i]); // Toca a nota pela duração especificada
    }
    
    pwm_set_gpio_level(BUZZER_A_PIN, 0); // Desliga o som
}

// Desenha a nave no menu principal no display OLED
void draw_ship_menu() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (ship_bitmap[i][j]) { // Se for 1, desenha o pixel
                ssd1306_draw_pixel(&oled, menu.x + j, menu.y + i);
            }
        }
    }
}

// Desenha o menu inicial
void draw_menu(int selected_option) {
    ssd1306_clear(&oled);

    // Desenha o título "Space Impact" sublinhado
    ssd1306_draw_string(&oled, 30, 0, 2, "SPACE");
    ssd1306_draw_string(&oled, 25, 18, 2, "IMPACT");
    ssd1306_draw_line(&oled, 20, 35, 98, 35); // Linha sublinhada

    // Desenha a nave no menu
    draw_ship_menu();

    // Desenha as opções
    ssd1306_draw_string(&oled, 30, 42, 1, selected_option == 0 ? "> Start" : "  Start");
    ssd1306_draw_string(&oled, 30, 52, 1, selected_option == 1 ? "> Options" : "  Options");

    ssd1306_show(&oled);
}

// Desenha o submenu de opções
void draw_options_menu(int selected_option, int enemy_count) {
    ssd1306_clear(&oled);

    // Desenha o título "Options"
    ssd1306_draw_string(&oled, 40, 10, 2, "Options");

    // Desenha as opções
    char enemies_text[30];
    snprintf(enemies_text, sizeof(enemies_text), "Enemies: %d", enemy_count);
    ssd1306_draw_string(&oled, 20, 30, 1, selected_option == 0 ? "> Enemies" : "  Enemies");
    ssd1306_draw_string(&oled, 40, 40, 1, enemies_text);
    ssd1306_draw_string(&oled, 20, 50, 1, selected_option == 1 ? "> Back" : "  Back");

    ssd1306_show(&oled);
}

void options_loop();

// Loop do menu inicial
void menu_loop() {
    int selected_option = 0; // 0 = Start, 1 = Options
    bool option_selected = false;

    play_theme_music(); // Toca a música de tema

    while (!option_selected) {
        draw_menu(selected_option);

        // Navegação no menu com o joystick
        adc_select_input(0); // Ler eixo Y do joystick
        int y_val = adc_read();

        if (y_val < 1000 && selected_option < 1) {
            selected_option++; // Move para baixo
            sleep_ms(200); // Debounce
        } else if (y_val > 3000 && selected_option > 0) {
            selected_option--; // Move para cima
            sleep_ms(200); // Debounce
        }

        // Seleção com o botão A
        if (gpio_get(BUTTON_A_PIN) == 0) {
            option_selected = true;
            sleep_ms(200); // Debounce
        }

        sleep_ms(10);
    }

    // Inicia o jogo se "Start" foi selecionado
    if (selected_option == 0) {
        game_started = true;
    }
    // Abre o submenu de opções se "Options" foi selecionado
    else if (selected_option == 1) {
        options_loop();
    }
}

// Loop do submenu de opções
void options_loop() {
    int selected_option = 0; // 0 = Enemies, 1 = Back
    bool option_selected = false;

    while (!option_selected) {
        draw_options_menu(selected_option, enemy_count);

        // Navegação no menu com o joystick
        adc_select_input(0); // Ler eixo Y do joystick
        int y_val = adc_read();

        if (y_val < 1000 && selected_option < 1) {
            selected_option++; // Move para baixo
            sleep_ms(200); // Debounce
        } else if (y_val > 3000 && selected_option > 0) {
            selected_option--; // Move para cima
            sleep_ms(200); // Debounce
        }

        // Ajusta a quantidade de inimigos se "Enemies" estiver selecionado
        if (selected_option == 0) {
            adc_select_input(1); // Ler eixo X do joystick
            int x_val = adc_read();

            if (x_val < 1000 && enemy_count > 0) {
                enemy_count--; // Diminui a quantidade de inimigos
                sleep_ms(100); // Debounce
            } else if (x_val > 3000 && enemy_count < 100) {
                enemy_count++; // Aumenta a quantidade de inimigos
                sleep_ms(100); // Debounce
            }
        }

        // Seleção com o botão A
        if (gpio_get(BUTTON_A_PIN) == 0 && selected_option == 1) {
            option_selected = true;
            sleep_ms(200); // Debounce
        }

        sleep_ms(10);
    }
    // Volta ao menu inicial se "Back" foi selecionado
    if (selected_option == 1) {
        menu_loop();
    }
}

void shoot_bullet() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x = player.x + 10;
            bullets[i].y = player.y + 5;
            bullets[i].active = true;
            uint slice_num = pwm_gpio_to_slice_num(BUZZER_A_PIN);
            pwm_set_wrap(slice_num, 12500);
            pwm_set_clkdiv(slice_num, 100.0);
            pwm_set_gpio_level(BUZZER_A_PIN, 6250);
            sleep_ms(50);
            pwm_set_gpio_level(BUZZER_A_PIN, 0);
            break;
        }
    }
}

void update_bullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].x += 2;
            if (bullets[i].x > SCREEN_WIDTH) bullets[i].active = false;
        }
    }
}

void update_enemies() {
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].active) {
            enemies[i].x -= 1;
            if (enemies[i].x < 0) {
                enemies[i].x = SCREEN_WIDTH;
                enemies[i].y = (rand() % (SCREEN_HEIGHT - 10));
            }
        }
    }
}

void play_low_pitch_sound() {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_B_PIN);
    pwm_set_wrap(slice_num, 25000); // Frequência mais baixa para som grave
    pwm_set_clkdiv(slice_num, 100.0);
    pwm_set_gpio_level(BUZZER_B_PIN, 12500); // 50% duty cycle
    sleep_ms(100); // Duração do som
    pwm_set_gpio_level(BUZZER_B_PIN, 0); // Desliga o som
}

void check_collisions() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < enemy_count; j++) {
                if (enemies[j].active && bullets[i].x >= enemies[j].x && bullets[i].y >= enemies[j].y && bullets[i].y <= enemies[j].y + 10) {
                    enemies[j].active = false;
                    bullets[i].active = false;
                    score++; // Incrementa o score ao abater um inimigo
                    play_low_pitch_sound(); // Toca o som grave
                }
            }
        }
    }
}

// Atualiza a posição da nave com base no joystick
void update_ship_position() {
    adc_select_input(0);
    int y_val = adc_read();
    adc_select_input(1);
    int x_val = adc_read();

    // Movimentação com limites para evitar que saia da tela
    if (x_val < 1000 && player.x > 0) player.x -= 1;   // Esquerda
    if (x_val > 3000 && player.x < SCREEN_WIDTH - 10) player.x += 1;   // Direita
    if (y_val < 1000 && player.y < SCREEN_HEIGHT - 10) player.y += 1;   // Baixo
    if (y_val > 3000 && player.y > 0) player.y -= 1;   // Cima
}

// Desenha a nave do jogador no display OLED
void draw_ship() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (ship_bitmap[i][j]) { // Se for 1, desenha o pixel
                ssd1306_draw_pixel(&oled, player.x + j, player.y + i);
            }
        }
    }
}

// Desenha a nave inimiga no display OLED
void draw_enemy(int x, int y) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 8; j++) {
            if (enemy_bitmap[i][j]) { // Se for 1, desenha o pixel
                ssd1306_draw_pixel(&oled, x + j, y + i);
            }
        }
    }
}

// Desenha o chão com montanhas
void draw_ground() {
    int ground_level = SCREEN_HEIGHT - 10; // Altura do chão
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        int y = ground_level + (rand() % 5); // Adiciona variação para criar montanhas
        ssd1306_draw_pixel(&oled, x, y);
    }
}

// Desenha a barra de score no topo da tela
void draw_score_bar() {
    char score_text[20];
    snprintf(score_text, sizeof(score_text), "Score: %d", score); // Formata o texto do score
    ssd1306_draw_string(&oled, 0, 0, 1, score_text); // Desenha o texto na posição (0, 0)
}

// Renderiza o jogo na tela
void render_game() {
    ssd1306_clear(&oled); // Limpa a tela
    draw_score_bar();      // Desenha a barra de score
    draw_ship();          // Desenha a nave na nova posição
    draw_ground();        // Desenha o chão com montanhas

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) ssd1306_draw_pixel(&oled, bullets[i].x, bullets[i].y);
    }
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].active) {
            draw_enemy(enemies[i].x, enemies[i].y); // Desenha a nave inimiga
        }
    }

    ssd1306_show(&oled);  // Atualiza a tela
}

int main() {
    setup_hardware();

    // Exibe o menu inicial
    menu_loop();

    while (game_started) {
        update_ship_position(); // Atualiza posição da nave

        if (gpio_get(BUTTON_A_PIN) == 0) shoot_bullet();

        update_bullets();
        update_enemies();
        check_collisions();

        render_game(); // Desenha a nave

        sleep_ms(1); // Pequeno atraso para suavizar a animação
    }
}

