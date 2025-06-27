import numpy as np
import matplotlib.pyplot as plt

if __name__ == '__main__':
    #Médias e desvios padrão coletadas anteriormente
    media_brkga = np.array([673, 344, 690, 263, 758, 282, 755, 1130, 3771, 3467, 3601, 15297])
    std_brkga = np.array(
        [45.160, 35.287, 65.825, 33.813, 45.724, 34.004, 54.060, 37.008, 206.779, 117.085, 112.158, 1371.605])

    media_cs = np.array([659, 199, 508, 192, 646, 144, 535, 1025, 2963, 3468, 3469, 14747])
    std_cs = np.array(
        [333.951, 108.764, 158.919, 77.010, 314.872, 28.335, 61.866, 40.596, 611.606, 662.232, 102.005, 1821.767])

    instances = [
        "nrp-e1.txt", "nrp-e2.txt", "nrp-e3.txt", "nrp-e4.txt",
        "nrp-g1.txt", "nrp-g2.txt", "nrp-g3.txt",
        "nrp1.txt", "nrp2.txt", "nrp3.txt", "nrp4.txt", "nrp5.txt"
    ]

    # Cálculo de EffectSize
    effect_sizes = []
    for i in range(len(instances)):
        m1 = media_brkga[i]
        m2 = media_cs[i]
        sd1 = std_brkga[i]
        sd2 = std_cs[i]

        pooled_sd = np.sqrt((sd1 ** 2 + sd2 ** 2) / 2)
        d = (m1 - m2) / pooled_sd # d>0 --> CS é melhor, caso contrario BRKGA é melhor
        effect_sizes.append(d)
        print(f"{instances[i]}: d = {d:.3f}")

    # Lista de todos os d-values
    print("\nEffect sizes (Cohen's d):")
    print(np.round(effect_sizes, 3))

    #Salvo aqui -> Resulados
    cohen_d = np.array(effect_sizes)

    colors = ['green' if d > 0.8 else 'orange' if d > 0.2 else 'gray' for d in cohen_d]

    plt.figure(figsize=(12, 6))
    bars = plt.bar(instances, cohen_d, color=colors)
    plt.axhline(y=0.8, color='green', linestyle='--', label='Grande efeito (d = 0.8)')
    plt.axhline(y=0.5, color='orange', linestyle='--', label='Médio efeito (d = 0.5)')
    plt.axhline(y=0.2, color='gray', linestyle='--', label='Pequeno efeito (d = 0.2)')
    plt.axhline(y=0, color='black', linewidth=0.8)


    plt.title("Effect Size - BRKGA vs BRKGA_CS")
    plt.ylabel("Effect Size")
    plt.xticks(rotation=45)
    plt.legend()
    plt.tight_layout()

    #Barras
    for barra, d in zip(bars, cohen_d):
        yval = barra.get_height()
        plt.text(barra.get_x() + barra.get_width() / 2, yval + 0.1, f"{d:.2f}", ha='center', va='bottom')

    plt.grid(axis='y', linestyle='--', alpha=0.6)
    plt.show()




