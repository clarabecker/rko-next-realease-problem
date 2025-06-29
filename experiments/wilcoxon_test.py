import pandas as pd
import os
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import numpy as np


def plot_bar_comparison(df_plot):
    print("\nGerando Gráfico 1: Comparação por Barras Agrupadas...")

    plt.style.use('seaborn-v0_8-whitegrid')
    fig, ax = plt.subplots(figsize=(14, 8))

    x = np.arange(len(df_plot['Instance']))
    width = 0.35

    rects1 = ax.bar(x - width / 2, df_plot['BRKGA_avg'].abs(), width, label='BRKGA', color='royalblue')
    rects2 = ax.bar(x + width / 2, df_plot['BRKGA_CS_avg'].abs(), width, label='BRKGA_CS', color='darkorange')

    ax.set_ylabel('Valor Absoluto da Média da Função Objetivo (Score)', fontsize=12)
    ax.set_title('Comparação de Desempenho Médio: BRKGA vs. BRKGA_CS', fontsize=16, pad=20)
    ax.set_xticks(x)
    ax.set_xticklabels(df_plot['Instance'], rotation=45, ha="right")
    ax.legend()
    ax.grid(axis='x')

    ax.bar_label(rects1, padding=3, fmt='%.0f', rotation=90, fontsize=11)
    ax.bar_label(rects2, padding=3, fmt='%.0f', rotation=90, fontsize=11)

    fig.tight_layout()
    plt.show()


def run_statistical_analysis():

    try:
        script_dir = os.path.dirname(os.path.abspath(__file__))
        results_dir = os.path.join(script_dir, "results")
        summary_file = os.path.join(results_dir, "summarized_results.csv")
        if not os.path.exists(summary_file):
            summary_file = os.path.join(script_dir, "summarized_results.csv")
    except NameError:
        summary_file = "summarized_results.csv"

    if not os.path.exists(summary_file):
        print(f"Erro: O arquivo '{summary_file}' não foi encontrado.")
        print("Certifique-se de que 'summarized_results.csv' está na mesma pasta que o script.")
        return

    print(f"Lendo dados de '{summary_file}'...")
    try:
        df = pd.read_csv(summary_file)
    except Exception as e:
        print(f"Erro ao ler o arquivo CSV: {e}")
        return

    col_brkga = 'Algorithm: BRKGA (avg ± std / best)'
    col_brkga_cs = 'Algorithm: BRKGA_CS (avg ± std / best)'

    if col_brkga not in df.columns or col_brkga_cs not in df.columns:
        print(f"Erro: O CSV não contém as colunas esperadas. Colunas encontradas: {df.columns.tolist()}")
        return

    print("Processando dados para análise...")
    try:
        df_analysis = pd.DataFrame()
        df_analysis['Instance'] = df['Instance']
        df_analysis['BRKGA_avg'] = df[col_brkga].str.split(' ± ').str[0].astype(float)
        df_analysis['BRKGA_CS_avg'] = df[col_brkga_cs].str.split(' ± ').str[0].astype(float)
    except Exception as e:
        print(f"Erro ao processar os dados das colunas. Verifique o formato. Detalhes: {e}")
        return

    print("\nAnálise Estatística (Teste de Wilcoxon)")
    try:
        statistic, p_value = stats.wilcoxon(df_analysis['BRKGA_avg'].abs(), df_analysis['BRKGA_CS_avg'].abs())
        print(f"Estatística do teste: {statistic:.4f}")
        print(f"p-valor: {p_value:.4f}")

        alpha = 0.05
        if p_value < alpha:
            print(f"\nComo o p-valor ({p_value:.4f}) é MENOR que {alpha}, rejeitamos a Hipótese Nula.")
            print(">> Conclusão: HÁ uma diferença estatisticamente significativa entre o desempenho dos algoritmos.")
        else:
            print(f"\nComo o p-valor ({p_value:.4f}) é MAIOR ou IGUAL a {alpha}, não podemos rejeitar a Hipótese Nula.")
            print(">> Conclusão: A diferença de desempenho observada NÃO é estatisticamente significativa.")
    except ValueError as e:
        print(f"\nErro ao executar o teste de Wilcoxon: {e}")
        print("Isso pode ocorrer se as duas amostras de dados forem idênticas.")

    plot_bar_comparison(df_analysis)


if __name__ == '__main__':
    run_statistical_analysis()