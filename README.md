# Projeto - Conversores A/D com Joystick e PWM

**Unidade 4 - Capítulo 8: Conversores A/D**  
**Atividade Prática Individual**

───────────────────────────────────────────────

*Desenvolvido por: Amanda Cardoso Lopes*

───────────────────────────────────────────────

## Objetivos do Projeto

O principal objetivo deste trabalho foi consolidar os conceitos de utilização do conversor analógico-digital (ADC) no microcontrolador RP2040, explorando também as funcionalidades da placa de desenvolvimento BitDogLab. Para isso, busquei:

- **Compreender o funcionamento do ADC:** Aprender como os sinais analógicos do joystick podem ser lidos e processados pelo RP2040.
- **Controlar a intensidade dos LEDs via PWM:** Ajustar de forma suave o brilho dos LEDs RGB (vermelho e azul) com base nos valores do joystick.
- **Interação gráfica no display SSD1306:** Representar, por meio de um quadrado móvel de 8x8 pixels, a posição do joystick na tela.
- **Utilizar o protocolo I2C:** Integrar e comunicar com o display SSD1306 para a exibição dos gráficos.
- **Implementar funcionalidades interativas:** Usar os botões (do joystick e o Botão A) com rotinas de interrupção e debouncing para controlar outros elementos do projeto, como alternar o LED verde e modificar a borda do display.

───────────────────────────────────────────────

## Descrição do Projeto

Neste projeto, o joystick é a peça central, fornecendo valores analógicos dos eixos **X** e **Y**. Esses valores são utilizados de diversas formas:

- **Controle dos LEDs via PWM:**  
  - **LED Azul:**  
    - O brilho é ajustado conforme o valor do eixo Y. Quando o joystick está centralizado (valor próximo a 2048), o LED permanece apagado. Conforme o joystick é movido para os extremos (valores próximos de 0 ou 4095), o brilho aumenta gradativamente.
  - **LED Vermelho:**  
    - Segue o mesmo princípio, mas controlado pelo valor do eixo X.

- **Movimentação do Quadrado no Display:**  
  - Um quadrado de 8x8 pixels é desenhado no display SSD1306 e se move proporcionalmente à posição capturada do joystick, oferecendo uma representação visual intuitiva dos movimentos.

- **Funcionalidades dos Botões:**  
  - **Botão do Joystick (GPIO 22):**  
    - Ao ser pressionado, alterna o estado do LED Verde (ligando ou desligando) e também modifica o estilo da borda do display, alternando entre uma borda fina e grossa.
  - **Botão A (GPIO 5):**  
    - Ativa ou desativa os efeitos dos LEDs controlados via PWM, permitindo que se compare o comportamento com e sem a modulação de intensidade.
  - **Botão B (GPIO 6):**
    - Ao ser acionado, a plaquinha entra em modo bootseel.

Além dessas funcionalidades, a aplicação foi desenvolvida com atenção à qualidade do código, utilizando interrupções (IRQ) para as ações dos botões e implementando debouncing via software para evitar leituras falsas.

───────────────────────────────────────────────

## Funcionalidades Implementadas

1. **Leitura Analógica com ADC:**  
   - Captura dos valores dos eixos X e Y do joystick para controle dos LEDs e do quadrado no display.

2. **Controle de Brilho dos LEDs com PWM:**  
   - Ajuste gradual do brilho dos LEDs Vermelho e Azul conforme o movimento do joystick, com mapeamento dos valores analógicos para ciclos de PWM.

3. **Interface Gráfica no Display SSD1306:**  
   - Desenho e movimentação de um quadrado de 8x8 pixels na tela, que se desloca de forma proporcional aos valores lidos do joystick.
   - Alteração do estilo da borda do display sempre que o botão do joystick é pressionado.

4. **Uso de Interrupções e Debouncing:**  
   - Implementação de rotinas de interrupção (IRQ) para os botões (Botão A, Botão do Joystick e um botão extra para entrada no modo bootloader) com tratamento de bouncing via software.

5. **Integração via I2C:**  
   - Comunicação com o display SSD1306 utilizando o protocolo I2C para enviar e atualizar as informações gráficas.

───────────────────────────────────────────────

## Componentes Utilizados

- **Microcontrolador:** RP2040 (Placa BitDogLab)
- **Display SSD1306 (128x64):**  
  - Conectado via I2C (SDA no GPIO 14 e SCL no GPIO 15)
- **LED RGB:**  
  - **LED Vermelho:** GPIO 13 (PWM)  
  - **LED Azul:** GPIO 12 (PWM)  
  - **LED Verde:** GPIO 11 (Liga/Desliga)
- **Joystick:**  
  - Eixos X e Y conectados aos GPIOs 26 e 27 (leitura via ADC)  
  - Botão integrado ao joystick: GPIO 22
- **Botão A:** Conectado ao GPIO 5

───────────────────────────────────────────────

## Como Executar o Projeto

1. **Clonagem do Repositório:**  
   Abra o VS Code (ou seu editor de preferência) e clone o repositório:
   ```sh
   git init
   git clone https://github.com/4mandaCardoso/EmbarcaTech_Atv10-02.git
      ```  

2. **Carregamento e Compilação:**  
   - Compile o código e gere o arquivo .UF2.

3. **Rodando na plaquinha:**  
   - Arraste o arquivo .UF2 para o diretório da plaquinha.

───────────────────────────────────────────────

## VÍDEO DE DEMONSTRAÇÃO 

───────────────────────────────────────────────

### Link do vídeo:
🔗 


───────────────────────────────────────────────

## CONCLUSÃO

───────────────────────────────────────────────



Este projeto foi uma experiência muito enriquecedora, permitindo-me explorar e integrar diversos conceitos importantes na programação de microcontroladores. Ao trabalhar com a leitura analógica do joystick, o controle dinâmico de LEDs por meio de PWM e a exibição gráfica no display SSD1306, pude compreender melhor a comunicação entre hardware e software. A implementação das rotinas de interrupção com debouncing também foi um grande desafio, mas fundamental para garantir a confiabilidade e a responsividade do sistema. Estou muito satisfeito com os resultados alcançados, pois o trabalho não só reforçou meus conhecimentos técnicos, como também aprimorou minha capacidade de organizar e documentar o código de forma clara e eficiente.



───────────────────────────────────────────────

*Desenvolvido por: Amanda Cardoso Lopes*
     **- GRUPO 4, SUBGRUPO 6**
