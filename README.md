# Projeto: Controle de LEDs e Display OLED com Joystick no RP2040

## Descrição Geral

Este projeto implementa o controle de LEDs e um display **OLED SSD1306** utilizando um **joystick analógico** no microcontrolador **RP2040**, presente na placa **Raspberry Pi Pico**. O código foi desenvolvido utilizando **Pico SDK** no **VS Code**.

## Objetivos

- **Controlar a intensidade de LEDs via PWM** utilizando o movimento do joystick.
- **Alternar o LED verde e exibir um retângulo extra no display** ao pressionar o botão do joystick.
- **Ativar e desativar o controle de LEDs via PWM** utilizando o Botão A.
- **Exibir um cursor quadrado que se move com o joystick** dentro do retângulo no display.

## Componentes Utilizados

- **Microcontrolador**: RP2040 (Raspberry Pi Pico)
- **Display OLED SSD1306**: Comunicação via I2C (GPIO 14 - SDA, GPIO 15 - SCL)
- **Joystick Analógico**: Leitura via ADC (GPIO 26 - Eixo X, GPIO 27 - Eixo Y)
- **LEDs**:
  - LED Vermelho (GPIO 13 - PWM)
  - LED Azul (GPIO 12 - PWM)
  - LED Verde (GPIO 11 - Alternado pelo botão do joystick)
- **Botões**:
  - Botão do Joystick (GPIO 22)
  - Botão A (GPIO 5)

## Funcionalidades Implementadas

### 1. Controle da Intensidade dos LEDs por Joystick
- **O eixo X do joystick controla o LED vermelho (GPIO 13).**
- **O eixo Y do joystick controla o LED azul (GPIO 12).**
- **Se o joystick estiver centralizado, os LEDs permanecem apagados.**
- **A intensidade do PWM aumenta conforme o joystick se move para os extremos.**

### 2. Controle do LED Verde e Retângulo no Display
- **Ao pressionar o botão do joystick (GPIO 22), o LED verde (GPIO 11) é alternado.**
- **O botão do joystick também alterna a exibição de um retângulo extra de 112x50 no display.**

### 3. Ativação e Desativação do PWM pelo Botão A
- **O Botão A (GPIO 5) ativa ou desativa o controle de intensidade dos LEDs via PWM.**
- **Quando desativado, os LEDs vermelho e azul permanecem apagados independentemente do joystick.**
- **Mensagens são exibidas no console para indicar o estado do PWM.**

### 4. Movimento do Cursor Quadrado no Display
- **Um cursor quadrado se move dentro do retângulo do display conforme o joystick é movimentado.**
- **O cursor respeita os limites do retângulo e sua posição é recalculada dinamicamente.**
- **Para exibir um quadrado em vez de um 'X', foi modificada a codificação hexadecimal correspondente à letra 'X' na biblioteca `font.h`, criando um quadrado de 4x4 pixels.**

## Como Executar o Projeto

### 1. Configuração do Ambiente
- Instale o **VS Code** e configure o **Pico SDK**.
- Conecte a placa **Raspberry Pi Pico** ao computador via USB.

### 2. Compilação e Execução
- Compile o código e gere um arquivo `.uf2`.
- Transfira o arquivo para a placa **Raspberry Pi Pico**.
- Utilize o **Serial Monitor** do VS Code para monitorar mensagens do programa.

### 3. Testes e Depuração
- Movimente o **joystick** para controlar os **LEDs** e o **cursor quadrado no display**.
- Pressione **o botão do joystick** para alternar o LED verde e o retângulo no display.
- Pressione **o Botão A** para ativar ou desativar o controle de intensidade dos LEDs via PWM.

## Estrutura do Projeto

- `main.c`: Código principal do projeto.
- `ssd1306.h / ssd1306.c`: Biblioteca para controle do display OLED SSD1306.
- `font.h`: Arquivo da fonte do display, onde a letra 'X' foi modificada para exibir um quadrado 4x4 pixels.
- `README.md`: Documentação do projeto.

## Espaço para Vídeo de Demonstração

(ESPAÇO PARA LINK)

