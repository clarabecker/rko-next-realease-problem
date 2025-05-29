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
extern int n; // Variable 'n' (random key solution size)
int N, M;
const double f = 0.7;                                  

void FreeMemoryProblem(); 

//------ DEFINITION OF GLOBAL CONSTANTS AND VARIABLES OF SPECIFIC PROBLEM  ----------

// These are the STATIC GLOBAL VARIABLES that need to be declared BEFORE FreeMemoryProblem()'s IMPLEMENTATION.
static std::vector<int> c;                  // Cost of requirements
static std::vector<int> w;                  // Profit of customers
static std::vector<int> v;                  // Computed value of requirements
static std::vector<std::pair<int, int>> P;  // Prerequisites (i, j): i is prerequisite of j
static std::vector<std::pair<int, int>> Q;  // Requests (i, k): requirement i requested by customer k              // Factor to compute budget
int b;                                      // Budget
static std::vector<std::vector<int>> S;     // S[i] = list of customers that requested requirement i


//----------------------- IMPLEMENTATION OF FUNCTIONS  -------------------------------

/************************************************************************************
 Method: ReadData
 Description: Read input data for the problem
*************************************************************************************/

void ReadData(const char nameTable[]) {
    char name[200] = "../Instances/";
    strcat(name, nameTable);

    FILE* arq = fopen(name, "r");
    if (arq == NULL) {
        printf("\nERROR: File (%s) not found!\n", name);
        getchar();
        exit(1);
    }

    FreeMemoryProblem();
    char linha[10000];

    if (fgets(linha, sizeof(linha), arq) == NULL) {
        printf("Erro ao ler número de níveis do arquivo!\n");
        fclose(arq); // Fechar o arquivo antes de sair
        exit(1);
    }
    int levels = atoi(linha);

    int total_requirements = 0;

    // Read requirements
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

    N = total_requirements;

    fgets(linha, sizeof(linha), arq);
    int dependencies = atoi(linha);

    for (int i = 0; i < dependencies; ++i) {
        fgets(linha, sizeof(linha), arq);
        int from, to;
        sscanf(linha, "%d %d", &from, &to);
        P.emplace_back(from - 1, to - 1);  
    }

    fgets(linha, sizeof(linha), arq);
    M = atoi(linha);

    for (int customer = 0; customer < M; ++customer) {
        fgets(linha, sizeof(linha), arq);
        int profit;
        char* ptr = strtok(linha, " ");
        profit = atoi(ptr);
        ptr = strtok(NULL, " ");  

        w.push_back(profit);

        while ((ptr = strtok(NULL, " ")) != NULL) {
            int req = atoi(ptr) - 1; 
            Q.emplace_back(req, customer); 
        }
    }

    // Calculate budget
    int total_cost = accumulate(c.begin(), c.end(), 0);
    double raw_b = total_cost * f;
    if (raw_b - static_cast<int>(raw_b) == 0.5) raw_b += 0.01;
    b = round(raw_b);

    // Update v values based on customers
    for (const auto& item : Q) {
        int req = item.first; 
        int cus = item.second; 
        v[req] += w[cus];
    }

    S.resize(N); 
    for (const auto& item : Q) {
        int req = item.first;  
        int cus = item.second; 
        S[req].push_back(cus);
    }
    
    n = N; 
    fclose(arq);
}

/************************************************************************************
 Method: Decoder
 Description: Maps the random-key solutions into problem solutions
*************************************************************************************/
double Decoder(TSol s)
{
    std::cout << "n = " << n << ", c.size() = " << c.size() << ", w.size() = " << w.size()
          << ", Q.size() = " << Q.size() << ", P.size() = " << P.size() << ", M = " << M << std::endl;

    std::vector<int> sC(n); // Using 'n' which should be equal to 'N'
    
    for (int i = 0; i < n; i++) { 
        sC[i] = i; 
    }

    // Sort the list based on values in the random-key vector (rk)
    std::sort(sC.begin(), sC.end(), [&s](int i1, int i2) {
        return s.rk[i1] < s.rk[i2];
    });
    
    std::vector<bool> selecionado(n, false); // Using 'n' (should be N)
    int custo_total = 0;
    
     for (int i : sC) {
        if (selecionado[i]) continue;

        // Check if prerequisites are selected
        bool pode_adicionar = true;
        for (auto [pre, pos] : P) {
            if (pos == i && !selecionado[pre]) {
                pode_adicionar = false;
                break;
            }
        }
    
        if (!pode_adicionar) continue;

        // Check if it fits the budget
        if (custo_total + c[i] <= b) {
            selecionado[i] = true;
            custo_total += c[i];
        }
    }

     // Calculate total profit of satisfied customers
    double lucro_total = 0.0;
    std::vector<bool> cliente_satisfeito(M, true); // Using M (number of customers)

    for (const auto& item : Q) { // Iterate through Q, not requirements
        int req = item.first;
        int cliente = item.second;
        if (!selecionado[req]) {
            cliente_satisfeito[cliente] = false;
        }
    }

    for (int i = 0; i < M; ++i) { // Iterate through customers
        if (cliente_satisfeito[i]) {
            lucro_total += w[i];
        }
    }

    s.ofv = lucro_total;

    // Debugging print
    if (debug && print) { // Ensure 'debug' and 'print' are defined elsewhere if used
        std::cout << "Lucro total: " << lucro_total << "\nRequisitos selecionados: ";
        for (int i = 0; i < n; ++i) {
            if (selecionado[i]) std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    return lucro_total;
}

// <--- MOVE THE ENTIRE IMPLEMENTATION OF FreeMemoryProblem() TO HERE --->
/************************************************************************************
 Method: FreeMemoryProblem
 Description: Frees the local memory allocated for the problem
*************************************************************************************/
void FreeMemoryProblem() {
    n = 0; 
    N = 0; 
    M = 0;
    c.clear();
    w.clear();
    v.clear();
    P.clear();
    Q.clear();
    b = 0; 
    S.clear();
}


#endif