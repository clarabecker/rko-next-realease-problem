import pandas as pd
import os
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import numpy as np


def plot_bar_comparison(df_plot):
    """Gera um gráfico de barras agrupadas comparando os algoritmos."""
    print("\nGerando Gráfico 1: Comparação por Barras Agrupadas...")

    # Configurações do gráfico
    plt.style.use('seaborn-v0_8-whitegrid')
    fig, ax = plt.subplots(figsize=(14, 8))

    # Posições das barras no eixo x
    x = np.arange(len(df_plot['Instance']))
    width = 0.35  # Largura das barras

    # Criando as barras para cada algoritmo
    rects1 = ax.bar(x - width/2, df_plot['BRKGA_avg'], width, label='BRKGA', color='royalblue')
    rects2 = ax.bar(x + width/2, df_plot['BRKGA_CS_avg'], width, label='BRKGA_CS', color='darkorange')

    # Adicionando títulos e legendas
    ax.set_ylabel('Valor Médio da Função Objetivo (Score)', fontsize=12)
    ax.set_title('Comparação de Desempenho Médio: BRKGA vs. BRKGA_CS', fontsize=16, pad=20)
    ax.set_xticks(x)
    ax.set_xticklabels(df_plot['Instance'], rotation=45, ha="right")
    ax.legend()
    ax.grid(axis='x') # Remove a grade vertical para um visual mais limpo

    # Adiciona os valores no topo das barras (opcional)
    ax.bar_label(rects1, padding=3, fmt='%.0f', rotation=90)
    ax.bar_label(rects2, padding=3, fmt='%.0f', rotation=90)

    fig.tight_layout()  # Ajusta o layout para evitar sobreposição
    plt.show()


def run_statistical_analysis():
    # Supondo que o script está na raiz do projeto e o CSV em uma pasta "results"
    try:
        results_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "results")
        summary_file = os.path.join(results_dir, "summarized_results.csv")
    except NameError:
        # Fallback para execução interativa (ex: Jupyter Notebook)
        summary_file = "summarized_results.csv" # Assuma que está na mesma pasta

    if not os.path.exists(summary_file):
        print(f"Erro: O arquivo '{summary_file}' não foi encontrado.")
        return

    print(f"Lendo dados de '{summary_file}'...")
    df = pd.read_csv(summary_file)

    # --- Extração e Limpeza dos Dados ---
    col_brkga = 'Algorithm: BRKGA (avg / best)'
    col_brkga_cs = 'Algorithm: BRKGA_CS (avg / best)'

    try:
        # Cria um novo DataFrame para as análises
        df_analysis = pd.DataFrame()
        df_analysis['Instance'] = df['Instance']
        df_analysis['BRKGA_avg'] = df[col_brkga].str.split(' / ').str[0].astype(float)
        df_analysis['BRKGA_CS_avg'] = df[col_brkga_cs].str.split(' / ').str[0].astype(float)
    except Exception as e:
        print(f"Erro ao processar dados. Verifique o formato do CSV. Detalhes: {e}")
        return

    # --- Execução do Teste Estatístico ---
    print("\n--- Teste de Wilcoxon dos Postos Sinalizados ---")
    print("Comparando os resultados MÉDIOS (avg) de cada algoritmo para cada instância.")
    print("Hipótese Nula (H₀): Não há diferença significativa entre os algoritmos.")
    print("--------------------------------------------------\n")

    try:
        statistic, p_value = stats.wilcoxon(df_analysis['BRKGA_avg'], df_analysis['BRKGA_CS_avg'])
        print(f"Estatística do teste: {statistic:.4f}")
        print(f"p-valor: {p_value:.4f}")
        print("\n--- Conclusão Estatística ---")

        alpha = 0.05
        if p_value < alpha:
            print(f"Como o p-valor ({p_value:.4f}) é MENOR que {alpha}, rejeitamos a Hipótese Nula.")
            print(">> Conclui-se que HÁ uma diferença estatisticamente significativa entre a performance dos algoritmos.")
        else:
            print(f"Como o p-valor ({p_value:.4f}) é MAIOR ou IGUAL a {alpha}, não podemos rejeitar a Hipótese Nula.")
            print(">> Conclui-se que a diferença observada NÃO é estatisticamente significativa.")
    except ValueError as e:
        print(f"Erro ao executar o teste de Wilcoxon: {e}")
        print("Isso pode acontecer se as duas amostras forem idênticas.")

    plot_bar_comparison(df_analysis)

if __name__ == '__main__':
    run_statistical_analysis()