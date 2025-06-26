# RKO Next Release Problem

Implementação do algoritmo **Random-Key Optimizer (RKO)** aplicado ao **Problema da Próxima Versão (Next Release Problem - NRP)**.

O **NRP** é um problema clássico de otimização na Engenharia de Software, proposto por **Bagnall, Rayward-Smith e Whittley (2001)**, que consiste em selecionar um subconjunto de requisitos para a próxima versão de um sistema de software que maximize a satisfação dos clientes, respeitando restrições de custo e dependência entre requisitos.

Este projeto utiliza a abordagem de **chaves aleatórias (Random-Key Encoding)** para representar soluções candidatas, e um decodificador guloso para avaliar a qualidade das soluções geradas.

## Referência

> Bagnall, A., Rayward-Smith, V. J., & Whittley, I. M. (2001). *The Next Release Problem*. Information and Software Technology, 43(14), 883–890.

### Formulação 
<img width="257" alt="Captura de Tela 2025-06-16 às 4 52 41 PM" src="https://github.com/user-attachments/assets/79731671-e2a3-4eb2-a2e1-0743f47a91a3" />

## Tecnologias

- C++
- OpenMP (para paralelização)
- Makefile
- MacOS/Linux compatível

## Como compilar e executar

### Pré-requisitos

- [g++ com suporte a OpenMP (ex: g++-13 ou superior no Mac via Homebrew)](https://brew.sh/)
- [`make`](https://www.gnu.org/software/make/)

### Passo a passo de execução 
- Acessar pasta experiment/rko/Program
  
### Compilar

```bash
make rebuild
```
### Executar

./runTest ../Instances/testScenario.csv K T D

-  Explicação dos parâmetros
- K: número de metaheurísticas usadas para resolver o problema (de 0 a 9)
- T: modo de ajuste de parâmetros (0 = offline, 1 = online)
- D: modo de debug (1 = imprime no terminal, 0 = salva em arquivo)

```bash
./runTest rko-next-release-problem/experiments/testScenario.csv 0 0 1
