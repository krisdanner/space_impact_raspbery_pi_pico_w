# Space Impact para Raspberry Pi Pico W

![IMG-20250308-WA0006](https://github.com/user-attachments/assets/a1b65f21-927b-4c58-af9e-85b267c4c60a)

Este projeto é uma implementação do clássico jogo **Space Impact** para o **Raspberry Pi Pico W**, utilizando um display OLED SSD1306 e um joystick para controle. O jogo foi desenvolvido em C, utilizando a SDK do Raspberry Pi Pico, e inclui funcionalidades como menu inicial e menu de opções, ajuste de quantidade de inimigos, música inicial, sons dos tiros, colisão dos tiros com os inimigos e um sistema de pontuação.

O objetivo deste repositório é fornecer uma base para que outros desenvolvedores possam expandir o jogo, adicionando novas fases, inimigos, cenários, chefes e muito mais!

---

## Funcionalidades Atuais

1. **Menu Inicial**:
   - Título "Space Impact" sublinhado.
   - Opções "Start" (iniciar o jogo) e "Options" (configurações).
   - Música de tema tocada no Buzzer A.

2. **Submenu de Opções**:
   - **Enemies**: Ajusta a quantidade de inimigos.
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

---

## Estrutura do Código

### O código está organizado da seguinte forma:

-**main.c**:

   - Contém a lógica principal do jogo, incluindo o menu inicial, submenu de opções, e loop do jogo.

   - Funções para controle da nave, tiros, colisões, e renderização.

-**ssd1306.c e ssd1306.h**:

   - Biblioteca para controlar o display OLED SSD1306.

-**Bitmaps**:

   - Bitmaps da nave do jogador e dos inimigos estão definidos no código principal.

-**Música e Efeitos Sonoros**:

   - A música de tema e os efeitos sonoros são gerados usando PWM nos buzzers.

---

## Como Contribuir

### Este projeto está aberto para contribuições! Aqui estão algumas ideias para expandir o jogo:

1. **Novas Fases**:

   - Adicionar diferentes fases com inimigos e cenários únicos.

   - Implementar transições entre fases.

2. **Inimigos e Chefes**:

   - Criar novos tipos de inimigos com padrões de movimento e ataques diferentes.

   - Adicionar chefes no final de cada fase.

3. **Cenários Dinâmicos**:

   - Implementar cenários com rolagem de fundo.

   - Adicionar obstáculos e power-ups.

4. **Sistema de Progressão**:

   - Adicionar um sistema de níveis e experiência.

   - Permitir que o jogador melhore a nave com upgrades.

5. **Melhorias de Interface**:

   - Adicionar animações no menu inicial.

   - Melhorar a exibição de pontuação e status do jogo.

6. **Multiplayer**:

   - Implementar um modo cooperativo para dois jogadores.

---

## Licença
   Este projeto está licenciado sob a MIT License. Sinta-se à vontade para usar, modificar e distribuir o código conforme necessário.

---

## Agradecimentos

   - À comunidade do Raspberry Pi Pico por fornecer uma plataforma incrível para desenvolvimento.

   - Aos desenvolvedores da biblioteca SSD1306 por facilitar o uso do display OLED.

---

## Contato

   Se você tiver dúvidas, sugestões ou quiser contribuir, sinta-se à vontade para abrir uma issue ou enviar um pull request. Também pode me contatar diretamente pelo GitHub.

Vamos transformar este jogo em algo incrível juntos! 🚀🎮
-
Nota: Este projeto foi desenvolvido como um exemplo educacional e pode ser expandido para fins de aprendizado ou entretenimento. Divirta-se!
