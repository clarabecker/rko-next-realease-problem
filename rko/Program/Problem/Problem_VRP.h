#ifndef _PROBLEM_H
#define _PROBLEM_H

#include "../Main/Data.h"
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <iostream>
using namespace std;

// Global Variables
extern int n;                                   // size of the vector solution
int N, M;
const double speed = 15.0;                                       

//---------------------- DEFINITION OF TYPES OF PROBLEM SPECIFIC --------------------
// NA


//------ DEFINITION OF GLOBAL CONSTANTS AND VARIABLES OF SPECIFIC PROBLEM  ----------

static std::vector<std::vector<double>> dist;    // matrix containing distances
static std::vector<std::vector<double>> times;   // matrix containing travel times


//----------------------- IMPLEMENTATION OF FUNCTIONS  -------------------------------

/************************************************************************************
 Method: ReadData
 Description: Read input data for the problem
*************************************************************************************/

void ReadData(char nameTable[]) { 
    char name[200] = "../Instances/";
    strcat(name, nameTable);

    FILE* arq;
    arq = fopen(name, "r");
    if (arq == NULL) {
        printf("\nERROR: File (%s) not found!\n", name);
        getchar();
        exit(1);   
    }

    // Read the instance head 
    char linha[256];
    fgets(linha, sizeof(linha), arq);
    sscanf(linha, "%d %d", &N, &M);  

    dist.clear();
    dist.resize(N + 1, std::vector<double>(N + 1));
    times.clear();
    times.resize(N + 1, std::vector<double>(N + 1));

    int i, j;
    double d;

    for(unsigned i = 0; i < N + 2; i++) {
        fgets(linha, sizeof(linha), arq);
    }

    // Read matrix informations 
    unsigned nbLines = ((N+1)*(N+1)-(N+1))/2;
    for(unsigned l = 0; l < nbLines; l++) {
        fgets(linha, sizeof(linha), arq);
        sscanf(linha, "%d %d %lf", &i, &j, &d);
        dist[i][j] = d;  
        dist[j][i] = d;
        times[i][j] = d / speed;
        times[j][i] = d / speed;
    }

    for(unsigned i = 0; i < N + 1; i++) {
        dist[i][i] = 0;
        times[i][i] = 0;
    }
    
    fclose(arq);
    n = N + M - 1;
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

    /*
    std::cout << "Valores de rk: ";
    int counter = 0;
    for (double val : s.rk) { 
        std::cout << val << "(" << counter << ")" << " ";
        counter++;
    }   
    std::cout << std::endl;

    std::cout << "Valores de sC: ";
    for (int val : sC) {
        std::cout << val << " ";
    }   
    std::cout << std::endl;
    */

    // Sort the list based on values in the random-key vector (rk)
    std::sort(sC.begin(), sC.end(), [&s](int i1, int i2) {
        return s.rk[i1] < s.rk[i2];
    });
    
    /*
    std::cout << "Valores de sC: ";
    for (int val : sC) {
        std::cout << val << " ";
    }   
    std::cout << std::endl;
    */

    std::vector<vector<int>> sol(M);  
    
    int veiculoAtual = 0;
    for(unsigned i = 0; i < n; i++) {
        if (sC[i] > N - 1) {
            veiculoAtual++;
            continue;
        }
        sol[veiculoAtual].push_back(sC[i]);
    }

    /*
    for(unsigned i = 0; i < M; i++) {
        cout << "Rota " << i + 1 << ": ";
        for(unsigned j = 0; j < sol[i].size(); j++) {
            cout << sol[i][j] << " ";
        }
        cout << "\n";
    }
    */

    // Objective function
    s.ofv = 0.0;
    double totalTime = 0.0;
    double largestTime = 0.0;
    for (unsigned vehicle = 0; vehicle < M; vehicle++) {
        double routeTime = 0.0;
        int size = sol[vehicle].size();
        for (int i = 0; i < size - 1; i++)
            routeTime += times[sol[vehicle][i]+1][sol[vehicle][i+1]+1];
        if (size >= 1)
            routeTime += times[0][sol[vehicle][0]+1] + times[0][sol[vehicle][size - 1]+1];
        totalTime += routeTime;
        if (routeTime > largestTime)
            largestTime = routeTime;
    }
    s.ofv = largestTime;

    // Print solution
    if (debug && print) {
        cout << endl;
        for (int i = 0; i < M; i++) {
            std::cout << "Vehicle " << (i + 1) << ": ";
            for (int j : sol[i]) {
                std::cout << j << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "Travel time: " << s.ofv << std::endl;
    } 
    
    // Return the objective function value
    return s.ofv;
}

/************************************************************************************
 Method: FreeMemoryProblem
 Description: Frees the local memory allocated for the problem
*************************************************************************************/
void FreeMemoryProblem() {
    dist.clear();
    times.clear();
}

#endif