# Projeto-2

## Descrição

### O que faz?
Este projeto implementa um sistema embarcado utilizando o microcontrolador PIC18F4550 para monitoramento de temperatura em um forno industrial.

O sistema realiza a leitura de temperatura por meio de um sensor LM35 (simulado por um potenciômetro), exibe os valores em um display LCD e executa uma contagem regressiva de tempo selecionada pelo usuário, em dois modos de tempo diferente. Além disso, um LED é utilizado para indicar temperaturas superiores a 50 °C, simulando o acionamento da resistência de aquecimento do forno.

---

### Como é construído?
O sistema foi desenvolvido em linguagem C utilizando o compilador MikroC PRO for PIC e é composto pelos seguintes módulos:

- Conversor Analógico-Digital (ADC) do PIC18F4550
- Sensor LM35 (emulado por potenciômetro)
- Display LCD 16x2 operando em modo 4 bits
- Temporizadores TMR0 e TMR1
- Sistema de interrupções externas para acionamento dos botões
- LED indicador de temperatura
- Máquina de estados para controle da operação do sistema

---

### Por que?
O projeto integra diversos conceitos de sistemas microprocessados e sistemas embarcados, incluindo:

- Conversão Analógico-Digital (ADC)
- Temporizadores (Timers)
- Interrupções
- Tratamento de botões
- Controle de periféricos
- Interface Homem-Máquina (LCD)
- Monitoramento de variáveis de processo

A aplicação simula um equipamento de monitoramento utilizado em processos industriais que exigem controle simultâneo de temperatura e tempo de operação.

---

## Funcionalidades

- **Leitura de Temperatura:**  
  - Aquisição de dados através do ADC de 10 bits do PIC18F4550
  - Sensor LM35 emulado por potenciômetro
  - Faixa de medição de 0 °C a 100 °C

- **Seleção de Tempo:**  
  Botão para selecionar:
    - Contagem curta (10 s)
    - Contagem longa (60 s) 

- **Contagem Regressiva:**  
  - Exibição contínua do tempo restante no display LCD
  - Utilização dos temporizadores:
    - TMR0 para intervalos de aproximadamente 1 segundo
    - TMR1 para intervalos de aproximadamente 250 ms

- **Controle por Interrupções:**  
  - Botões operando através de interrupções externas
  - Detecção por mudança de borda
  - Menor utilização da CPU em comparação com polling 

- **Indicador de Aquecimento:**  
  - LED acende para temperaturas superiores a 50 °C
  - Simula o acionamento da resistência do forno 

---

## Detalhes de Implementação

- **ADC:**  
  - Resolução: 10 bits
  - Canal analógico: AN0
  - Referência externa:
    - Vref+ = 1 V
    - Vref− = 0 V

- **Timer 0:**  
  Responsável pela geração da base de tempo da contagem longa (60 segundos).  

- **Timer 1:**  
  Responsável pela geração da base de tempo da contagem curta (10 segundos).

- **LCD:**  
  Operação em modo 4 bits utilizando a biblioteca LCD do MikroC.

- **Interrupções:**
  - Interrupção externa para acionamento dos botões
  - Interrupções dos timers para atualização da contagem regressiva 

---

## Esquemático
O circuito foi desenvolvido no SimulIDE e contém:

- PIC18F4550
- Display LCD 16x2
- Dois botões
- Potenciômetro
- LED indicador
- Fonte de referência de 1 V para o ADC

---

## Instruções de Uso

1. Abrir o projeto no MikroC PRO for PIC
2. Compilar o código e gerar o arquivo .hex
3. Abrir o circuito no SimulIDE
4. Carregar o firmware gerado
5. Iniciar a simulação
6. Selecionar o tempo de medição através do botão correspondente
7. Pressionar o botão de início
8. Ajustar o potenciômetro para simular diferentes temperaturas
9. Observar:
  - Temperatura exibida no LCD
  - Contagem regressiva
  - Acionamento do LED acima de 50 °C

---

## Referências

Para a construção do projeto:
- Material disponibilizado na disciplina SEL0433 – Aplicação de Microprocessadores.
- Datasheet do PIC18F4550.
- Datasheet do LM35.
- Manual do Kit EasyPIC v7.
- Documentação do MikroC PRO for PIC.
- SimulIDE.

Para o desenvolvimento do README:
- https://www.youtube.com/watch?v=k4Rsy8GbKE0  

---

## Contribuição

Henrique Gaspar Monteiro — N° USP: 15459073  
Julia de Oliveira Giglio — N° USP: 15465167
