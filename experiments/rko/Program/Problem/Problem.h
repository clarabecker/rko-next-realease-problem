#ifndef PROBLEM_H
#define PROBLEM_H

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

    // Método para limpar a instância
    void clear() {
        N = M = b = 0;
        c.clear(); w.clear(); v.clear();
        P.clear(); Q.clear(); S.clear();
    }

    // Carrega os dados de um arquivo
    static Problem LoadFromFile(const std::string& instanceName) {
        Problem p;
        const double f = 0.7;
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
        return p;
    }
};

// --- Interface Global para Compatibilidade ---
inline Problem g_problem_instance;
extern int n; 

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
    std::vector<int> sC(problem.N);
    std::iota(sC.begin(), sC.end(), 0); 

    std::sort(sC.begin(), sC.end(), [&](int i1, int i2) {
        return s.rk[i1] < s.rk[i2];
    });

    std::vector<bool> selecionado(problem.N, false);
    int custo_total = 0;

    for (int req_idx : sC) {
        bool pode_adicionar = true;
        for (const auto& dep : problem.P) {
            if (dep.second == req_idx && !selecionado[dep.first]) {
                pode_adicionar = false;
                break;
            }
        }
        if (!pode_adicionar) continue;

        if (custo_total + problem.c[req_idx] <= problem.b) {
            selecionado[req_idx] = true;
            custo_total += problem.c[req_idx];
        }
    }

    double lucro_total = 0.0;
    std::vector<bool> cliente_satisfeito(problem.M, true);

    for (const auto& item : problem.Q) {
        if (!selecionado[item.first]) {
            cliente_satisfeito[item.second] = false;
        }
    }
    for (int i = 0; i < problem.M; ++i) {
        if (cliente_satisfeito[i]) {
            lucro_total += problem.w[i];
        }
    }

    s.ofv = lucro_total * (-1);
    return lucro_total * (-1);
}

#endif