import pandas as pd
import os

results_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "results")

def summarize_results():
    all_results = []

    for filename in os.listdir(results_dir):
        filepath = os.path.join(results_dir, filename)
        if not os.path.isfile(filepath):
            continue

        try:
            with open(filepath, "r", encoding="utf-8") as f:
                first_line = f.readline().strip()
                alg_name = first_line.replace("Algoritmo:", "").strip()

            df = pd.read_csv(filepath, sep="\t", skiprows=2)

            if df.empty or df.shape[1] != 5:
                continue

            df.columns = ["Replication", "Instance", "OFV", "Total Time", "Best Time"]

            grouped = df.groupby("Instance")["OFV"].agg(["mean", "min"]).round(3)
            grouped["Algorithm"] = alg_name
            grouped["Avg_Best"] = grouped["mean"].astype(str) + " / " + grouped["min"].astype(str)
            grouped.reset_index(inplace=True)
            all_results.extend(grouped[["Instance", "Algorithm", "Avg_Best"]].to_dict("records"))

        except Exception as e:
            print(f"Erro ao processar {filename}: {e}")

    if not all_results:
        print("Nenhum resultado processado.")
        return

    df_all = pd.DataFrame(all_results)
    df_pivot = df_all.pivot(index="Instance", columns="Algorithm", values="Avg_Best").sort_index()

    df_pivot.columns = [f"{col.replace('Algorithm: ', '')} (avg, best)" for col in df_pivot.columns]

    df_pivot.to_csv(os.path.join(results_dir, "summarized_results.csv"))

if __name__ == '__main__':
    summarize_results()
