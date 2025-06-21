import os
import subprocess
from pathlib import Path
import re

def experiment1():
    # diretório do script
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # definir caminhos
    program_dir = os.path.join(script_dir, "rko", "Program")
    test_scenario_file = os.path.join(script_dir, "testScenario.csv")
    results_dir = os.path.join(script_dir, "results")

    os.makedirs(os.path.dirname(test_scenario_file), exist_ok=True)
    os.makedirs(results_dir, exist_ok = True)

    # parâmetros
    max_runtime = 1
    max_runs = 1
    num_replicacoes = 10

    metaheuristics = {
        "BRKGA": 0,  "GA": 6, "BRKGA_CS": 8, "RKO": 9
    }

    tuning_method = 0
    debug_mode = 1

    instance_files = list(Path(script_dir, "instances/").glob("*.txt"))

    if not instance_files:
        print("No instances found in instances")
        exit(1)

    # escrever arquivo de cenário de teste
    with open(test_scenario_file, "w") as f:
        f.write("Instance | Max Run Time | Max Runs\n")
        for instance in instance_files:
            instance_name = instance.name
            f.write(f"NRP/{instance_name}\t{max_runtime}\t{max_runs}\n")

    if not os.path.exists(program_dir):
        print("Program not found")
        exit(1)

    # compilar o programa
    os.chdir(program_dir)
    subprocess.run(["make", "rebuild"], check=True)

    # executar experimentos
    for alg_name, alg_id in metaheuristics.items():
        output_file = os.path.join(results_dir, f"results_{alg_name}.txt")
        with open(output_file, "w") as f_out:
            f_out.write(f"Algorithm: {alg_name}\n")
            f_out.write("=" * 60 + "\n")
            f_out.write("Replication\tInstance\tOFV\tTotal Time\tBest Time\n")

            rep = 1
            while rep <= num_replicacoes:
                command = ["./runTest", test_scenario_file, str(alg_id), str(tuning_method), str(debug_mode)]
                print(f"Running: {' '.join(command)} (Metaheuristic: {alg_name}, Replication: {rep})")

                try:
                    result = subprocess.run(command, capture_output=True, text=True, check=True)
                    stdout = result.stdout

                    blocks = re.findall(
                        r"Instance:\s*(.*?)\s+ofv:\s*(-?[0-9.]+)\s+Total time:\s*([0-9.]+)\s+Best time:\s*([0-9.]+)",
                        stdout,
                        re.DOTALL
                    )

                    for instance in instance_files:
                        instance_name = instance.name
                        found = False

                        for inst, ofv, total_time, best_time in blocks:
                            if instance_name in inst:
                                f_out.write(f"{rep}\t{instance_name}\t{ofv}\t{total_time}\t{best_time}\n")
                                found = True
                                break

                        if not found:
                            f_out.write(f"{rep}\t{instance_name}\tN/A\tN/A\tN/A\n")

                    f_out.write("\n")

                except subprocess.CalledProcessError as e:
                    print(f"Erro ao executar {alg_name} na replicação {rep}: {e}")
                    print(f"Comando:{command}")
                    print(f"Return code: {e.returncode}")
                    #exit()
                    rep -=1
                rep += 1

    print("Experiments completed!")

if __name__ == '__main__':
    experiment1()
