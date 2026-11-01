import os
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def graficar_parte_1():
    csv_file = "resultados_lista.csv"
    if not os.path.exists(csv_file):
        print(f"[ADVERTENCIA] No se encontro '{csv_file}'. Saltando graficos de Parte I.")
        return

    df = pd.read_csv(csv_file)
    
    if "Eficiencia" not in df.columns and "Speedup" in df.columns and "Hilos" in df.columns:
        df["Eficiencia"] = df["Speedup"] / df["Hilos"]

    df_parallel = df[df["Estrategia"] != "Secuencial"]

    sns.set_theme(style="whitegrid")
    scenarios = ["Read-Heavy", "Balanced", "Write-Heavy"]
    threads = [1, 2, 4, 6, 8]

    # --- 1. Grafico de Speedup ---
    fig1, axes1 = plt.subplots(1, 3, figsize=(16, 5), sharey=False)
    for i, scenario in enumerate(scenarios):
        ax = axes1[i]
        sub_df = df_parallel[df_parallel["Escenario"] == scenario]
        if sub_df.empty:
            continue
        
        ax.plot(threads, threads, 'k--', label="Speedup Ideal", alpha=0.5, linewidth=1.5)
        sns.lineplot(
            data=sub_df, 
            x="Hilos", 
            y="Speedup", 
            hue="Estrategia", 
            marker="o", 
            markersize=8,
            linewidth=2.5,
            ax=ax
        )
        ax.set_title(f"Escenario: {scenario}", fontsize=12, fontweight='bold')
        ax.set_xlabel("Cantidad de Hilos", fontsize=10)
        ax.set_ylabel("Speedup ($T_{seq} / T_{par}$)", fontsize=10)
        
        if scenario == "Read-Heavy":
            ax.set_ylim(0, 8.5)
        elif scenario == "Balanced":
            ax.set_ylim(0, 3.5)
        else:
            ax.set_ylim(0, 1.5)
        
        ax.set_xticks(threads)
        ax.grid(True, linestyle="--", alpha=0.7)

    plt.suptitle("Parte I: Evaluacion de Speedup - Lista Enlazada Multithreading", fontsize=14, fontweight='bold')
    plt.tight_layout()
    plt.savefig("grafico_lista_speedup.png", dpi=300)
    plt.close()
    print("[OK] Grafico Parte I (Speedup) guardado en 'grafico_lista_speedup.png'.")

    # --- 2. Grafico de Eficiencia ---
    fig2, axes2 = plt.subplots(1, 3, figsize=(16, 5), sharey=False)
    for i, scenario in enumerate(scenarios):
        ax = axes2[i]
        sub_df = df_parallel[df_parallel["Escenario"] == scenario]
        if sub_df.empty:
            continue
        
        ax.axhline(1.0, color='k', linestyle='--', alpha=0.5, label="Eficiencia Ideal (1.0)")
        sns.lineplot(
            data=sub_df, 
            x="Hilos", 
            y="Eficiencia", 
            hue="Estrategia", 
            marker="o", 
            markersize=8,
            linewidth=2.5,
            ax=ax
        )
        ax.set_title(f"Escenario: {scenario}", fontsize=12, fontweight='bold')
        ax.set_xlabel("Cantidad de Hilos", fontsize=10)
        ax.set_ylabel("Eficiencia ($Speedup / Hilos$)", fontsize=10)
        ax.set_ylim(0, 1.1)
        ax.set_xticks(threads)
        ax.grid(True, linestyle="--", alpha=0.7)

    plt.suptitle("Parte I: Evaluacion de Eficiencia - Lista Enlazada Multithreading", fontsize=14, fontweight='bold')
    plt.tight_layout()
    plt.savefig("grafico_lista_eficiencia.png", dpi=300)
    plt.close()
    print("[OK] Grafico Parte I (Eficiencia) guardado en 'grafico_lista_eficiencia.png'.")


def graficar_parte_2():
    csv_file = "resultados_oddeven_5rep.csv" if os.path.exists("resultados_oddeven_5rep.csv") else "resultados_oddeven.csv"
    if not os.path.exists(csv_file):
        print(f"No se encontro CSV para Odd-Even Sort")
        return

    df_raw = pd.read_csv(csv_file)

    if "Repeticion" in df_raw.columns:
        df_avg = df_raw.groupby(
            ["Patron", "N", "Hilos"], 
            as_index=False
        )[["Tiempo_Seq_ms", "Tiempo_Par_ms"]].mean()
        df_avg["Speedup"] = df_avg["Tiempo_Seq_ms"] / df_avg["Tiempo_Par_ms"]
        df_avg["Eficiencia"] = df_avg["Speedup"] / df_avg["Hilos"]
    else:
        df_avg = df_raw
        if "Eficiencia" not in df_avg.columns and "Speedup" in df_avg.columns and "Hilos" in df_avg.columns:
            df_avg["Eficiencia"] = df_avg["Speedup"] / df_avg["Hilos"]

    sns.set_theme(style="whitegrid")
    patterns = ["Aleatorio", "Ordenado", "Inverso", "Duplicados"]
    threads = [1, 2, 4, 6, 8]

    # --- 1. Grafico de Speedup ---
    fig1, axes1 = plt.subplots(2, 2, figsize=(14, 9))
    for i, pattern in enumerate(patterns):
        ax = axes1[i // 2, i % 2]
        sub_df = df_avg[df_avg["Patron"] == pattern]
        if sub_df.empty:
            continue
        
        ax.plot(threads, threads, 'k--', label="Speedup Ideal", alpha=0.3, linewidth=1.5)
        sns.lineplot(
            data=sub_df,
            x="Hilos",
            y="Speedup",
            hue="N",
            palette="tab10",
            marker="o",
            markersize=7,
            linewidth=2,
            ax=ax
        )
        ax.set_title(f"Patron: {pattern}", fontsize=12, fontweight='bold')
        ax.set_xlabel("Cantidad de Hilos", fontsize=10)
        ax.set_ylabel("Speedup Promedio ($T_{seq} / T_{par}$)", fontsize=10)
        ax.set_xticks(threads)
        ax.set_ylim(0.0, 3.0)
        ax.grid(True, linestyle="--", alpha=0.7)

    plt.suptitle("Parte II: Escalabilidad Promedio de Odd-Even Sort (Speedup)", fontsize=14, fontweight='bold')
    plt.tight_layout()
    plt.savefig("grafico_oddeven_speedup.png", dpi=300)
    plt.close()
    print("Grafico Parte II (Speedup) guardado en 'grafico_oddeven_speedup.png'.")

    # --- 2. Grafico de Eficiencia ---
    fig2, axes2 = plt.subplots(2, 2, figsize=(14, 9))
    for i, pattern in enumerate(patterns):
        ax = axes2[i // 2, i % 2]
        sub_df = df_avg[df_avg["Patron"] == pattern]
        if sub_df.empty:
            continue
        
        ax.axhline(1.0, color='k', linestyle='--', alpha=0.3, label="Eficiencia Ideal (1.0)")
        sns.lineplot(
            data=sub_df,
            x="Hilos",
            y="Eficiencia",
            hue="N",
            palette="tab10",
            marker="o",
            markersize=7,
            linewidth=2,
            ax=ax
        )
        ax.set_title(f"Patron: {pattern}", fontsize=12, fontweight='bold')
        ax.set_xlabel("Cantidad de Hilos", fontsize=10)
        ax.set_ylabel("Eficiencia Promedio ($Speedup / Hilos$)", fontsize=10)
        ax.set_xticks(threads)
        ax.set_ylim(0.0, 1.1)
        ax.grid(True, linestyle="--", alpha=0.7)

    plt.suptitle("Parte II: Eficiencia Promedio de Odd-Even Sort", fontsize=14, fontweight='bold')
    plt.tight_layout()
    plt.savefig("grafico_oddeven_eficiencia.png", dpi=300)
    plt.close()
    print("Grafico Parte II (Eficiencia) guardado en 'grafico_oddeven_eficiencia.png'.")


if __name__ == "__main__":
    graficar_parte_1()
    graficar_parte_2()
    print("PROCESO FINALIZADO")
