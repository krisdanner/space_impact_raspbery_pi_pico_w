# Space Impact para Raspberry Pi Pico W

Este projeto é uma implementação do clássico jogo **Space Impact** para o **Raspberry Pi Pico W**, utilizando um display OLED SSD1306 e um joystick para controle. O jogo foi desenvolvido em C, utilizando a SDK do Raspberry Pi Pico, e inclui funcionalidades como menu inicial, ajuste de dificuldade, música de tema, e um sistema de pontuação.

O objetivo deste repositório é fornecer uma base para que outros desenvolvedores possam expandir o jogo, adicionando novas fases, inimigos, cenários, chefes e muito mais!

---

## Funcionalidades Atuais

1. **Menu Inicial**:
   - Título "Space Impact" sublinhado.
   - Opções "Start" (iniciar o jogo) e "Options" (configurações).
   - Música de tema tocada no Buzzer A.

2. **Submenu de Opções**:
   - **Enemies**: Ajusta a quantidade de inimigos (mínimo de 50).
   - **Back**: Volta ao menu inicial.

3. **Jogo Principal**:
   - Controle da nave com joystick.
   - Sistema de tiros e colisões.
   - Inimigos que se movem horizontalmente.
   - Pontuação baseada no número de inimigos abatidos.
   - Efeito sonoro ao abater inimigos (Buzzer B).

4. **Gráficos**:
   - Nave do jogador e inimigos representados por bitmaps.
   - Chão com montanhas geradas aleatoriamente.

5. **Hardware**:
   - Display OLED SSD1306 (128x64 pixels).
   - Joystick analógico para controle.
   - Dois buzzers (Buzzer A para música e tiros, Buzzer B para efeitos sonoros).
   - LEDs RGB para feedback visual.

---

## Como Compilar e Executar

### Requisitos

- **Hardware**:
  - Raspberry Pi Pico W.
  - Display OLED SSD1306 (conectado via I2C).
  - Joystick analógico.
  - Dois buzzers (conectados aos GPIOs 21 e 10).
  - LEDs RGB (opcional).

- **Software**:
  - [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk).
  - Compilador GCC para ARM (já incluído no SDK).
  - Ferramentas de depuração (opcional, como OpenOCD).
