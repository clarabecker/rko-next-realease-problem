#ifndef _PROBLEM_H
#define _PROBLEM_H

#include "../Main/Data.h"
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <numeric>
#include <iostream>

using namespace std;

// Global Variables
int n = 0, m =0;                                     

//---------------------- DEFINITION OF TYPES OF PROBLEM SPECIFIC --------------------
// NA


//------ DEFINITION OF GLOBAL CONSTANTS AND VARIABLES OF SPECIFIC PROBLEM  ----------

static std::vector<int> c;    // Cost of requirements
static std::vector<int> w;    // Profit of customers
static std::vector<int> v;    // Computed value of requirements
static std::vector<std::pair<int, int>> P;  // Prerequisites (i, j): i is prerequisite of j
static std::vector<std::pair<int, int>> Q;  // Requests (i, k): requirement i requested by customer k
double f = 0;                   // Factor to compute budget
int b = 0;                      // Budget
static std::vector<std::vector<int>> S;   // S[i] = list of customers that requested requirement i


//----------------------- IMPLEMENTATION OF FUNCTIONS  -------------------------------

/************************************************************************************
 Method: ReadData
 Description: Read input data for the problem
*************************************************************************************/

// Leitura da instância no estilo da função Python
void ReadData(const char nameTable[], double factor) {
    char name[200] = "../Instances/";
    strcat(name, nameTable);

    FILE* arq = fopen(name, "r");
    if (arq == NULL) {
        printf("\nERROR: File (%s) not found!\n", name);
        getchar();
        exit(1);
    }

    FreeMemoryProblem();
    f = factor;
    char linha[1024];

    // Lê número de níveis
    fgets(linha, sizeof(linha), arq);
    int levels = atoi(linha);

    int total_requirements = 0;

    // Lê os requisitos
    for (int l = 0; l < levels; ++l) {
        fgets(linha, sizeof(linha), arq);
        int nb_requirements = atoi(linha);
        total_requirements += nb_requirements;

        fgets(linha, sizeof(linha), arq);
        char* token = strtok(linha, " ");
        for (int i = 0; i < nb_requirements && token != NULL; ++i) {
            c.push_back(atoi(token));
            v.push_back(0);
            token = strtok(NULL, " ");
        }
    }
    n = total_requirements;

    // Lê dependências
    fgets(linha, sizeof(linha), arq);
    int dependencies = atoi(linha);

    for (int i = 0; i < dependencies; ++i) {
        fgets(linha, sizeof(linha), arq);
        int from, to;
        sscanf(linha, "%d %d", &from, &to);
        P.emplace_back(from - 1, to - 1);  // Convertendo para índice 0-based
    }

    // Lê clientes
    fgets(linha, sizeof(linha), arq);
    m = atoi(linha);

    for (int customer = 0; customer < m; ++customer) {
        fgets(linha, sizeof(linha), arq);
        int profit;
        char* ptr = strtok(linha, " ");
        profit = atoi(ptr);
        ptr = strtok(NULL, " ");  // Número de requisitos pedidos (não usado)

        w.push_back(profit);

        while ((ptr = strtok(NULL, " ")) != NULL) {
            int req = atoi(ptr) - 1;  // Convertendo para índice 0-based
            Q.emplace_back(req, customer); // Adicionando ao vetor Q
        }
    }

    // Calcula o orçamento
    int total_cost = accumulate(c.begin(), c.end(), 0);
    double raw_b = total_cost * f;
    if (raw_b - static_cast<int>(raw_b) == 0.5) raw_b += 0.01;
    b = round(raw_b);

    // Atualiza valores v com base nos clientes
    for (const auto& item : Q) {
        int req = item.first;  // Requisito
        int cus = item.second; // Cliente
        v[req] += w[cus];
    }

    // Preenche S[i] com clientes que pedem o requisito i
    S.resize(n); // Redimensiona o vetor S para ter n elementos
    for (const auto& item : Q) {
        int req = item.first;  // Requisito
        int cus = item.second; // Cliente
        S[req].push_back(cus); // Adiciona o cliente à lista do requisito
    }

    fclose(arq);
}

/************************************************************************************
 Method: Decoder
 Description: Maps the random-key solutions into problem solutions
*************************************************************************************/
double Decoder(TSol s)
{
    std::vector<int> sC(n);
    
    for (int i = 0; i < n; i++) { 
        sC[i] = i; 
    }

    
    // Sort the list based on values in the random-key vector (rk)
    std::sort(sC.begin(), sC.end(), [&s](int i1, int i2) {
        return s.rk[i1] < s.rk[i2];
    });
    
    std::vector<bool> selecionado(n, false);
    int custo_total = 0;
    
     for (int i : sC) {
        if (selecionado[i]) continue;

        // Verificar se pré-requisitos já estão selecionados
        bool pode_adicionar = true;
        for (auto [pre, pos] : P) {
            if (pos == i && !selecionado[pre]) {
                pode_adicionar = false;
                break;
            }
        }
    
        if (!pode_adicionar) continue;

        // Verificar se cabe no orçamento
        if (custo_total + c[i] <= b) {
            selecionado[i] = true;
            custo_total += c[i];
        }
    }

     // Calcula lucro total dos clientes satisfeitos
    double lucro_total = 0.0;
    std::vector<bool> cliente_satisfeito(m, true);

    for (auto [req, cliente] : Q) {
        if (!selecionado[req]) {
            cliente_satisfeito[cliente] = false;
        }
    }

    for (int i = 0; i < m; ++i) {
        if (cliente_satisfeito[i]) {
            lucro_total += w[i];
        }
    }

    s.ofv = lucro_total;

    if (debug && print) {
        std::cout << "Lucro total: " << lucro_total << "\nRequisitos selecionados: ";
        for (int i = 0; i < n; ++i) {
            if (selecionado[i]) std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    return lucro_total;
}

/************************************************************************************
 Method: FreeMemoryProblem
 Description: Frees the local memory allocated for the problem
*************************************************************************************/
void FreeMemoryProblem() {
    n = m = 0;
    c.clear();
    w.clear();
    v.clear();
    P.clear();
    Q.clear();
    b = 0;
    S.clear();
}

#endif