import pandas as pd
import os
from scipy import stats  # A biblioteca de ciência de dados que contém o teste


def run_statistical_test():
    results_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "results")
    summary_file = os.path.join(results_dir, "summarized_results.csv")

    if not os.path.exists(summary_file):
        print(f"Erro: O arquivo '{summary_file}' não foi encontrado.")
        print("Por favor, execute o script de sumarização primeiro.")
        return

    print(f"Lendo dados de '{summary_file}'...")
    df = pd.read_csv(summary_file)

    # --- Extração e Limpeza dos Dados ---
    # Vamos comparar os melhores resultados (best) de cada algoritmo
    col_brkga = 'Algorithm: BRKGA (avg / best)'
    col_brkga_cs = 'Algorithm: BRKGA_CS (avg / best)'

    if col_brkga not in df.columns or col_brkga_cs not in df.columns:
        print("Erro: As colunas de resultado (avg / best) não foram encontradas no CSV.")
        return

    try:
        avg_brkga = df[col_brkga].str.split(' / ').str[0].astype(float)
        avg_brkga_cs = df[col_brkga_cs].str.split(' / ').str[0].astype(float)
    except Exception as e:
        print(f"Erro ao processar os dados das colunas. Verifique o formato do CSV. Detalhes: {e}")
        return

    print("\n--- Teste de Wilcoxon dos Postos Sinalizados ---")
    print("Comparando os MELHORES resultados (best) de cada algoritmo para cada instância.")
    print("Hipótese Nula (H₀): Não há diferença significativa entre os algoritmos.")
    print("--------------------------------------------------\n")

    # --- Execução do Teste Estatístico ---
    try:
        statistic, p_value = stats.wilcoxon(avg_brkga, avg_brkga_cs)

        print(f"Estatística do teste: {statistic:.4f}")
        print(f"p-valor: {p_value:.4f}")
        print("\n--- Conclusão ---")

        alpha = 0.05
        if p_value < alpha:
            print(f"Como o p-valor ({p_value:.4f}) é MENOR que {alpha}, rejeitamos a Hipótese Nula.")
            print(
                ">> Conclui-se que HÁ uma diferença estatisticamente significativa entre a performance dos algoritmos.")
        else:
            print(f"Como o p-valor ({p_value:.4f}) é MAIOR ou IGUAL a {alpha}, não podemos rejeitar a Hipótese Nula.")
            print(
                ">> Conclui-se que a diferença observada na performance dos algoritmos NÃO é estatisticamente significativa.")

    except ValueError as e:
        print(f"Erro ao executar o teste de Wilcoxon: {e}")
        print("Isso pode acontecer se as duas amostras de dados forem idênticas.")


if __name__ == '__main__':
    run_statistical_test()