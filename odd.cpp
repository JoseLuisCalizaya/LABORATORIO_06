#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <random>
#include <string>
#include <vector>

// Version secuencial
void odd_even_sort_sequential(std::vector<int> &arr) {
  int n = arr.size();
  for (int phase = 0; phase < n; ++phase) {
    if (phase % 2 == 0) {
      for (int i = 0; i < n - 1; i += 2) {
        if (arr[i] > arr[i + 1])
          std::swap(arr[i], arr[i + 1]);
      }
    } else {
      for (int i = 1; i < n - 1; i += 2) {
        if (arr[i] > arr[i + 1])
          std::swap(arr[i], arr[i + 1]);
      }
    }
  }
}

// Version paralela con OpenMP
void odd_even_sort_parallel(std::vector<int> &arr, int num_threads) {
  int n = arr.size();
  omp_set_num_threads(num_threads);

  for (int phase = 0; phase < n; ++phase) {
    if (phase % 2 == 0) {
#pragma omp parallel for schedule(static)
      for (int i = 0; i < n - 1; i += 2) {
        if (arr[i] > arr[i + 1])
          std::swap(arr[i], arr[i + 1]);
      }
    } else {
#pragma omp parallel for schedule(static)
      for (int i = 1; i < n - 1; i += 2) {
        if (arr[i] > arr[i + 1])
          std::swap(arr[i], arr[i + 1]);
      }
    }
  }
}

// Generador de datos
std::vector<int> generate_data(int n, const std::string &type) {
  std::vector<int> arr(n);
  if (type == "Aleatorio") {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, n * 10);
    for (int i = 0; i < n; ++i)
      arr[i] = dist(rng);
  } else if (type == "Ordenado") {
    for (int i = 0; i < n; ++i)
      arr[i] = i + 1;
  } else if (type == "Inverso") {
    for (int i = 0; i < n; ++i)
      arr[i] = n - i;
  } else if (type == "Duplicados") {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, 10);
    for (int i = 0; i < n; ++i)
      arr[i] = dist(rng);
  }
  return arr;
}

void run_experiment(std::ofstream &csv_file, const std::string &pattern, int n,
                    int num_threads, int repetitions = 5) {
  std::vector<int> base_data = generate_data(n, pattern);

  for (int rep = 1; rep <= repetitions; ++rep) {
    // Medicion secuencial por repeticion
    std::vector<int> seq_arr = base_data;
    auto t1 = std::chrono::high_resolution_clock::now();
    odd_even_sort_sequential(seq_arr);
    auto t2 = std::chrono::high_resolution_clock::now();
    double time_seq =
        std::chrono::duration<double, std::milli>(t2 - t1).count();

    // Medicion paralela por repeticion
    std::vector<int> par_arr = base_data;
    t1 = std::chrono::high_resolution_clock::now();
    odd_even_sort_parallel(par_arr, num_threads);
    t2 = std::chrono::high_resolution_clock::now();
    double time_par =
        std::chrono::duration<double, std::milli>(t2 - t1).count();

    // Verificacion de correctitud
    assert(std::is_sorted(seq_arr.begin(), seq_arr.end()));
    assert(std::is_sorted(par_arr.begin(), par_arr.end()));

    double speedup = time_seq / time_par;
    double efficiency = speedup / num_threads;

    csv_file << pattern << "," << n << "," << num_threads << "," << rep << ","
             << std::fixed << std::setprecision(4) << time_seq << ","
             << time_par << "," << speedup << "," << efficiency << "\n";
  }

  std::cout << "5 repeticiones guardadas para: " << std::left << std::setw(11)
            << pattern << " | N: " << std::setw(5) << n
            << " | Hilos: " << num_threads << "\n";
}

int main() {
  std::ofstream csv_file("resultados_oddeven_5rep.csv");
  if (!csv_file.is_open())
    return 1;

  csv_file << "Patron,N,Hilos,Repeticion,Tiempo_Seq_ms,Tiempo_Par_ms,Speedup,"
              "Eficiencia\n";

  std::vector<int> sizes = {1000, 3000, 5000, 7000, 10000};
  std::vector<int> thread_counts = {1, 2, 4, 6, 8};
  std::vector<std::string> patterns = {"Aleatorio", "Ordenado", "Inverso",
                                       "Duplicados"};

  for (const auto &pattern : patterns) {
    for (int n : sizes) {
      for (int threads : thread_counts) {
        run_experiment(csv_file, pattern, n, threads, 5);
      }
    }
  }

  csv_file.close();
  std::cout << "\n[EXITO] Archivo 'resultados_oddeven_5rep.csv' generado con "
               "todas las repeticiones.\n";
  return 0;
}
