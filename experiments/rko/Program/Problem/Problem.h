#ifndef PROBLEM_H
#define PROBLEM_H

#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>

// --- Estrutura de Dados do Problema ---
struct Problem {
    int N = 0, M = 0, b = 0;
    std::vector<int> c, w, v;
    std::vector<std::pair<int, int>> P, Q;
    std::vector<std::vector<int>> S;

    // Novo vetor para dependências indexadas por requisito (para otimização do Decoder)
    std::vector<std::vector<int>> depsPorReq;

    // Método para limpar a instância
    void clear() {
        N = M = b = 0;
        c.clear(); w.clear(); v.clear();
        P.clear(); Q.clear(); S.clear();
        depsPorReq.clear();
    }

    // Carrega os dados de um arquivo
    static Problem LoadFromFile(const std::string& instanceName) {
        Problem p;
        const double f = 0.3;   //Orçamento --> 30% do total
        const std::string fullPath = "../Instances/" + instanceName;

        std::ifstream file(fullPath);
        if (!file.is_open()) {
            throw std::runtime_error("Nao foi possivel abrir o arquivo: " + fullPath);
        }

        std::string line;
        int total_requirements = 0;

        std::getline(file, line); int levels = std::stoi(line);

        for (int l = 0; l < levels; ++l) {
            std::getline(file, line);
            total_requirements += std::stoi(line);
            std::getline(file, line);
            std::stringstream ss_costs(line); int cost;
            while (ss_costs >> cost) p.c.push_back(cost);
        }
        p.N = total_requirements;
        p.v.resize(p.N, 0);

        std::getline(file, line); int dependencies = std::stoi(line);
        for (int i = 0; i < dependencies; ++i) {
            std::getline(file, line); std::stringstream ss_deps(line);
            int from, to; ss_deps >> from >> to;
            p.P.emplace_back(from - 1, to - 1);
        }

        std::getline(file, line); p.M = std::stoi(line);

        for (int i = 0; i < p.M; ++i) {
            std::getline(file, line); std::stringstream ss_customer(line);
            int profit; ss_customer >> profit; p.w.push_back(profit);
            int req; while (ss_customer >> req) p.Q.emplace_back(req - 1, i);
        }

        long long total_cost = std::accumulate(p.c.begin(), p.c.end(), 0LL);
        double raw_b = static_cast<double>(total_cost) * f;
        p.b = static_cast<int>(std::round(raw_b));

        p.S.resize(p.N);
        for (const auto& item : p.Q) {
            p.v[item.first] += p.w[item.second];
            p.S[item.first].push_back(item.second);
        }

        // Preencher depsPorReq para acelerar o decoder
        p.depsPorReq.resize(p.N);
        for (const auto& dep : p.P) {
            int from = dep.first;
            int to = dep.second;
            p.depsPorReq[to].push_back(from);
        }

        return p;
    }
};

// --- Interface Global para Compatibilidade ---
inline Problem g_problem_instance;
extern int n;   //VETOR DE CHAVE ALEATORIA DO RKO --> todos os requisitos

inline void FreeMemoryProblem() {
    g_problem_instance.clear();
}

inline void ReadData(const char nameTable[]) {
    try {
        g_problem_instance = Problem::LoadFromFile(nameTable);
        n = g_problem_instance.N;
        // Removidos os couts para manter a função limpa
    } catch (const std::runtime_error& e) {
        std::fprintf(stderr, "\nERRO: %s\n", e.what());
        std::exit(1);
    }
}

inline double Decoder(TSol s) {
    const auto& problem = g_problem_instance;
    std::vector<int> ordem(problem.N);
    std::iota(ordem.begin(), ordem.end(), 0);

    // Ordenar requisitos com base nas random keys
    std::sort(ordem.begin(), ordem.end(), [&](int i, int j) {
        return s.rk[i] < s.rk[j];
    });

    std::vector<bool> selecionado(problem.N, false);
    int custo_total = 0;

    for (int req : ordem) {
        // Verifica se todas as dependências estão selecionadas
        bool dependencias_ok = true;
        for (int dep : problem.depsPorReq[req]) {
            if (!selecionado[dep]) {
                dependencias_ok = false;
                break;
            }
        }

        if (!dependencias_ok) continue;

        if (custo_total + problem.c[req] <= problem.b) {
            selecionado[req] = true;
            custo_total += problem.c[req];
        }
    }

    // Verificar clientes satisfeitos
    double lucro_total = 0.0;
    std::vector<bool> cliente_satisfeito(problem.M, true);

    for (const auto& [req, cli] : problem.Q) {
        if (!selecionado[req]) {
            cliente_satisfeito[cli] = false;
        }
    }

    for (int j = 0; j < problem.M; ++j) {
        if (cliente_satisfeito[j]) {
            lucro_total += problem.w[j];
        }
    }

    s.ofv = -lucro_total;
    return -lucro_total;
}

#endif
