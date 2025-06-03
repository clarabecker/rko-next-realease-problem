#ifndef _PROBLEM_H
#define _PROBLEM_H

// Inclua Main/Data.h (verifique o caminho e o conteúdo de TSol)
#include "../Main/Data.h" 

#include <vector>
#include <algorithm>
#include <cstdio>   // Para std::printf, std::fopen, etc.
#include <cstdlib>  // Para std::exit, std::atoi
#include <cstring>  // Para std::strcat, std::strtok
#include <cmath>    // Para std::round
#include <numeric>  // Para std::accumulate
#include <iostream> // Para std::cout, std::cerr

int N = 0; 
int M = 0;
inline const double f = 0.7;

// Variáveis específicas do problema de requisitos
inline std::vector<int> c;                  
inline std::vector<int> w;                  
inline std::vector<int> v;                  
inline std::vector<std::pair<int, int>> P;  
inline std::vector<std::pair<int, int>> Q;  
inline int b = 0;                           
inline std::vector<std::vector<int>> S;     

// --- Protótipos/Implementações de Funções (inline) ---
// Mova FreeMemoryProblem() para CIMA para que ela seja declarada antes de ser chamada em ReadData.

/************************************************************************************
 Método: FreeMemoryProblem
 Descrição: Libera a memória alocada localmente para o problema
*************************************************************************************/
inline void FreeMemoryProblem() { // 'inline' aqui para consistência
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

/************************************************************************************
 Método: ReadData
 Descrição: Lê os dados de entrada para o problema
*************************************************************************************/
inline void ReadData(const char nameTable[]) { // 'inline' aqui também
    char name[200] = "../Instances/";
    std::strcat(name, nameTable); // Use std::strcat

    FILE* arq = std::fopen(name, "r"); // Use std::fopen
    if (arq == NULL) {
        std::printf("\nERRO: Arquivo (%s) não encontrado!\n", name);
        std::exit(1);
    }

    FreeMemoryProblem(); // Agora FreeMemoryProblem está declarada e implementada acima.
    char linha[10000];

    if (std::fgets(linha, sizeof(linha), arq) == NULL) {
        std::printf("Erro ao ler número de níveis do arquivo!\n");
        std::fclose(arq);
        std::exit(1);
    }
    int levels = std::atoi(linha);

    int total_requirements = 0;

    for (int l = 0; l < levels; ++l) {
        if (std::fgets(linha, sizeof(linha), arq) == NULL) {
            std::printf("Erro ao ler nb_requirements para o nível %d!\n", l);
            std::fclose(arq); std::exit(1);
        }
        int nb_requirements = std::atoi(linha);
        total_requirements += nb_requirements;

        if (std::fgets(linha, sizeof(linha), arq) == NULL) {
            std::printf("Erro ao ler linha de custos para o nível %d!\n", l);
            std::fclose(arq); std::exit(1);
        }
        char* token = std::strtok(linha, " "); // Use std::strtok
        for (int i = 0; i < nb_requirements && token != NULL; ++i) {
            c.push_back(std::atoi(token));
            v.push_back(0); 
            token = std::strtok(NULL, " ");
        }
    }

    N = total_requirements;

    if (std::fgets(linha, sizeof(linha), arq) == NULL) {
        std::printf("Erro ao ler número de dependências!\n");
        std::fclose(arq); std::exit(1);
    }
    int dependencies = std::atoi(linha);

    for (int i = 0; i < dependencies; ++i) {
        if (std::fgets(linha, sizeof(linha), arq) == NULL) {
            std::printf("Erro ao ler dependência %d!\n", i);
            std::fclose(arq); std::exit(1);
        }
        int from, to;
        std::sscanf(linha, "%d %d", &from, &to); 
        P.emplace_back(from - 1, to - 1);
    }

    if (std::fgets(linha, sizeof(linha), arq) == NULL) {
        std::printf("Erro ao ler número de clientes (M)!\n");
        std::fclose(arq); std::exit(1);
    }
    M = std::atoi(linha);

    for (int customer = 0; customer < M; ++customer) {
        if (std::fgets(linha, sizeof(linha), arq) == NULL) {
            std::printf("Erro ao ler linha do cliente %d!\n", customer);
            std::fclose(arq); std::exit(1);
        }
        char* ptr = std::strtok(linha, " ");
        int profit = std::atoi(ptr); // profit foi declarada dentro do loop
        ptr = std::strtok(NULL, " ");  

        w.push_back(profit);

        while ((ptr = std::strtok(NULL, " ")) != NULL) {
            int req = std::atoi(ptr) - 1; 
            Q.emplace_back(req, customer); 
        }
    }

    int total_cost = std::accumulate(c.begin(), c.end(), 0);
    double raw_b = total_cost * f;
    if (raw_b - static_cast<int>(raw_b) == 0.5) raw_b += 0.01; 
    b = std::round(raw_b);

    for (const auto& item : Q) {
        int req = item.first; 
        int cus = item.second; 
        // Correção dos warnings de signed/unsigned
        if (static_cast<size_t>(req) >= v.size() || req < 0) { std::cerr << "ERRO: Indice de requisito fora do limite para v." << std::endl; std::exit(1); }
        if (static_cast<size_t>(cus) >= w.size() || cus < 0) { std::cerr << "ERRO: Indice de cliente fora do limite para w." << std::endl; std::exit(1); }
        v[req] += w[cus];
    }

    S.resize(N); 
    for (const auto& item : Q) {
        int req = item.first;  
        int cus = item.second; 
        // Correção dos warnings de signed/unsigned
        if (static_cast<size_t>(req) >= S.size() || req < 0) { std::cerr << "ERRO: Indice de requisito fora do limite para S." << std::endl; std::exit(1); }
        S[req].push_back(cus);
    }
    
    n = N; 
    std::fclose(arq);
}

/************************************************************************************
 Método: Decoder
 Descrição: Mapeia as soluções random-key para soluções do problema
*************************************************************************************/
inline double Decoder(TSol s) { // 'inline' aqui
    std::vector<int> sC(n);
    
    for (int i = 0; i < n; i++) { 
        sC[i] = i; 
    }

    std::sort(sC.begin(), sC.end(), [&s](int i1, int i2) {
        // Assume que s.rk tem tamanho 'n'
        return s.rk[i1] < s.rk[i2];
    });
    
    std::vector<bool> selecionado(n, false);
    int custo_total = 0;
    
    for (int i : sC) {
        if (selecionado[i]) continue;

        bool pode_adicionar = true;
        for (auto [pre, pos] : P) {
            // Correção dos warnings de signed/unsigned
            if (pos == i && (static_cast<size_t>(pre) >= selecionado.size() || !selecionado[pre])) { // Se o pré-requisito não estiver selecionado
                pode_adicionar = false;
                break;
            }
        }
    
        if (!pode_adicionar) continue;

        // Correção dos warnings de signed/unsigned
        if (static_cast<size_t>(i) < c.size() && i >= 0 && custo_total + c[i] <= b) {
            selecionado[i] = true;
            custo_total += c[i];
        }
    }

    double lucro_total = 0.0;
    std::vector<bool> cliente_satisfeito(M, true);

    for (const auto& item : Q) {
        int req = item.first;
        int cliente = item.second;
        // Correção dos warnings de signed/unsigned
        if (static_cast<size_t>(req) < selecionado.size() && req >= 0 && !selecionado[req]) {
            if (static_cast<size_t>(cliente) < cliente_satisfeito.size() && cliente >= 0) {
                 cliente_satisfeito[cliente] = false;
            } else {
                 std::cerr << "ERRO: Indice de cliente " << cliente << " fora do limite para cliente_satisfeito (tamanho " << cliente_satisfeito.size() << ") ao verificar satisfação." << std::endl;
                 // Considere lançar uma exceção ou usar assert() em vez de std::exit em produção para estes checks
            }
        } else if (static_cast<size_t>(req) >= selecionado.size() || req < 0) {
            std::cerr << "ERRO: Indice de requisito " << req << " fora do limite para selecionado (tamanho " << selecionado.size() << ") ao verificar satisfação do cliente." << std::endl;
            // Considere lançar uma exceção ou usar assert() em vez de std::exit em produção para estes checks
        }
    }

    for (int i = 0; i < M; ++i) {
        // Correção dos warnings de signed/unsigned
        if (static_cast<size_t>(i) < cliente_satisfeito.size() && i >= 0 && cliente_satisfeito[i]) {
            if (static_cast<size_t>(i) < w.size() && i >= 0) {
                lucro_total += w[i];
            } else {
                std::cerr << "ERRO: Indice de cliente " << i << " fora do limite para w (tamanho " << w.size() << ") ao calcular lucro." << std::endl;
                // Considere lançar uma exceção ou usar assert() em vez de std::exit em produção para estes checks
            }
        }
    }

    s.ofv = lucro_total;

    // Se 'debug' e 'print' forem definidos em outro lugar, eles devem ser 'extern' lá
    // ou passados como parâmetros se não puderem ser globais.
    // if (debug && print) { 
    //     std::cout << "Lucro total: " << lucro_total << "\nRequisitos selecionados: ";
    //     for (int i = 0; i < n; ++i) {
    //         if (selecionado[i]) std::cout << i << " ";
    //     }
    //     std::cout << std::endl;
    // }

    return lucro_total;
}

#endif // _PROBLEM_H