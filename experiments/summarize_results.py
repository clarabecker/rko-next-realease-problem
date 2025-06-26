import pandas as pd
import os
import numpy as np

# Caminho da pasta de resultados
# Para funcionar corretamente, ajuste o caminho ou rode a partir do diretório do script
results_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "results")

# Baselines por instância (melhores valores conhecidos)
baseline = {
    "nrp-e2.txt": -7446,
    "nrp-g2.txt": -4580,
    "nrp1.txt": -1204,
    "nrp4.txt": -10690,
    "nrp-e4.txt": -5814,
    "nrp2.txt": -4970,
    "nrp-g1.txt": -6130,
    "nrp-e3.txt": -6666,
    "nrp3.txt": -7488,
    "nrp-e1.txt": -7919,
    "nrp5.txt": -18510,
    "nrp-g3.txt": -5932,
}

def summarize_results():
    all_results = []

    # Certifique-se de que a pasta de resultados existe
    if not os.path.isdir(results_dir):
        print(f"Erro: A pasta de resultados '{results_dir}' não foi encontrada.")
        return

    for filename in os.listdir(results_dir):
        filepath = os.path.join(results_dir, filename)
        if not os.path.isfile(filepath) or not filename.endswith('.txt'):
            continue

        try:
            with open(filepath, "r", encoding="utf-8") as f:
                first_line = f.readline().strip()
                alg_name = first_line.replace("Algoritmo:", "").strip()

            df = pd.read_csv(filepath, sep="\t", skiprows=2)

            if df.empty or df.shape[1] != 5:
                continue

            df.columns = ["Replication", "Instance", "OFV", "Total Time", "Best Time"]

            for instance, group in df.groupby("Instance"):
                if instance not in baseline:
                    print(f"Instância '{instance}' não encontrada no baseline. Ignorando.")
                    continue

                mean_ofv = group["OFV"].mean()
                min_ofv = group["OFV"].min()

                # 2. Gap para o Baseline (Qualidade)
                baseline_value = baseline[instance]
                if baseline_value != 0:
                    baseline_gap = 100 * (mean_ofv - baseline_value) / np.abs(baseline_value)
                else:
                    baseline_gap = 0.0

                all_results.append({
                    "Instance": instance,
                    "Algorithm": alg_name,
                    "Avg_Best": f"{mean_ofv:.3f} / {min_ofv:.3f}",
                    "Baseline_Gap(%)": f"{baseline_gap:.2f}",
                })

        except Exception as e:
            print(f"Erro ao processar {filename}: {e}")

    if not all_results:
        print("Nenhum resultado para processar.")
        return

    df_all = pd.DataFrame(all_results)

    # --- SEÇÃO DE PIVOT ATUALIZADA PARA INCLUIR OS DOIS GAPS ---
    df_avg_best = df_all.pivot(index="Instance", columns="Algorithm", values="Avg_Best")
    df_baseline_gap = df_all.pivot(index="Instance", columns="Algorithm", values="Baseline_Gap(%)") # Novo pivot

    # Renomeia colunas para clareza
    df_avg_best.columns = [f"{col} (avg / best)" for col in df_avg_best.columns]
    df_baseline_gap.columns = [f"{col} (baseline gap %)" for col in df_baseline_gap.columns] # Novo rename

    # Junta todos os dataframes em uma ordem lógica
    df_final = pd.concat([df_avg_best, df_baseline_gap], axis=1).sort_index()

    # Salva o CSV final com um nome descritivo
    output_path = os.path.join(results_dir, "summarized_results.csv")
    df_final.to_csv(output_path)
    print(f"Resultados resumidos e completos salvos em: {output_path}")


if __name__ == '__main__':
    summarize_results()