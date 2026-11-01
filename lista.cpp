#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <omp.h>
#include <random>
#include <shared_mutex>
#include <vector>

struct Node {
  int data;
  Node *next;
  Node(int val) : data(val), next(nullptr) {}
};

class SequentialList {
private:
  Node *head;

public:
  SequentialList() : head(nullptr) {}
  ~SequentialList() {
    Node *curr = head;
    while (curr) {
      Node *temp = curr;
      curr = curr->next;
      delete temp;
    }
  }
  bool insert(int val) {
    Node *prev = nullptr;
    Node *curr = head;
    while (curr != nullptr && curr->data < val) {
      prev = curr;
      curr = curr->next;
    }
    if (curr != nullptr && curr->data == val)
      return false;
    Node *newNode = new Node(val);
    newNode->next = curr;
    if (prev == nullptr)
      head = newNode;
    else
      prev->next = newNode;
    return true;
  }
  bool search(int val) const {
    Node *curr = head;
    while (curr != nullptr && curr->data < val)
      curr = curr->next;
    return (curr != nullptr && curr->data == val);
  }
  int count_nodes() const {
    int count = 0;
    Node *curr = head;
    while (curr) {
      count++;
      curr = curr->next;
    }
    return count;
  }
  bool has_cycle() const {
    if (!head)
      return false;
    Node *slow = head, *fast = head->next;
    while (fast && fast->next) {
      if (slow == fast)
        return true;
      slow = slow->next;
      fast = fast->next->next;
    }
    return false;
  }
};

// BLOQUEO GLOBAL
class GlobalLockList {
private:
  Node *head;
  omp_lock_t list_lock;

public:
  GlobalLockList() : head(nullptr) { omp_init_lock(&list_lock); }
  ~GlobalLockList() {
    omp_destroy_lock(&list_lock);
    Node *curr = head;
    while (curr) {
      Node *temp = curr;
      curr = curr->next;
      delete temp;
    }
  }
  bool insert(int val) {
    omp_set_lock(&list_lock);
    Node *prev = nullptr;
    Node *curr = head;
    while (curr != nullptr && curr->data < val) {
      prev = curr;
      curr = curr->next;
    }
    if (curr != nullptr && curr->data == val) {
      omp_unset_lock(&list_lock);
      return false;
    }
    Node *newNode = new Node(val);
    newNode->next = curr;
    if (prev == nullptr)
      head = newNode;
    else
      prev->next = newNode;
    omp_unset_lock(&list_lock);
    return true;
  }
  bool search(int val) {
    omp_set_lock(&list_lock);
    Node *curr = head;
    while (curr != nullptr && curr->data < val)
      curr = curr->next;
    bool found = (curr != nullptr && curr->data == val);
    omp_unset_lock(&list_lock);
    return found;
  }
  int count_nodes() {
    omp_set_lock(&list_lock);
    int count = 0;
    Node *curr = head;
    while (curr) {
      count++;
      curr = curr->next;
    }
    omp_unset_lock(&list_lock);
    return count;
  }
  bool has_cycle() {
    omp_set_lock(&list_lock);
    if (!head) {
      omp_unset_lock(&list_lock);
      return false;
    }
    Node *slow = head, *fast = head->next;
    while (fast && fast->next) {
      if (slow == fast) {
        omp_unset_lock(&list_lock);
        return true;
      }
      slow = slow->next;
      fast = fast->next->next;
    }
    omp_unset_lock(&list_lock);
    return false;
  }
};

// BLOQUEO LECTURA/ESCRITURA (Read-Write Lock)
class ReadWriteList {
private:
  Node *head;
  mutable std::shared_mutex rw_lock;

public:
  ReadWriteList() : head(nullptr) {}
  ~ReadWriteList() {
    Node *curr = head;
    while (curr) {
      Node *temp = curr;
      curr = curr->next;
      delete temp;
    }
  }
  bool insert(int val) {
    std::unique_lock<std::shared_mutex> lock(
        rw_lock); // Exclusivo para escritura
    Node *prev = nullptr;
    Node *curr = head;
    while (curr != nullptr && curr->data < val) {
      prev = curr;
      curr = curr->next;
    }
    if (curr != nullptr && curr->data == val)
      return false;
    Node *newNode = new Node(val);
    newNode->next = curr;
    if (prev == nullptr)
      head = newNode;
    else
      prev->next = newNode;
    return true;
  }
  bool search(int val) const {
    std::shared_lock<std::shared_mutex> lock(
        rw_lock); // Compartido para múltiples lecturas
    Node *curr = head;
    while (curr != nullptr && curr->data < val)
      curr = curr->next;
    return (curr != nullptr && curr->data == val);
  }
  int count_nodes() const {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    int count = 0;
    Node *curr = head;
    while (curr) {
      count++;
      curr = curr->next;
    }
    return count;
  }
  bool has_cycle() const {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    if (!head)
      return false;
    Node *slow = head, *fast = head->next;
    while (fast && fast->next) {
      if (slow == fast)
        return true;
      slow = slow->next;
      fast = fast->next->next;
    }
    return false;
  }
};

// BANCO DE PRUEBAS Y GENERACIÓN DE RESULTADOS
void run_experiment(std::ofstream &csv_file, const std::string &scenario_name,
                    int total_inserts, int total_searches, int num_threads) {
  std::vector<int> insert_data(total_inserts);
  std::vector<int> search_data(total_searches);
  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(
      1, std::max(total_inserts, total_searches) * 2);

  for (int i = 0; i < total_inserts; ++i)
    insert_data[i] = dist(rng);
  for (int i = 0; i < total_searches; ++i)
    search_data[i] = dist(rng);

  // 1. Secuencial
  SequentialList seq_list;
  auto t1 = std::chrono::high_resolution_clock::now();
  for (int val : insert_data)
    seq_list.insert(val);
  int seq_found = 0;
  for (int val : search_data)
    if (seq_list.search(val))
      seq_found++;
  auto t2 = std::chrono::high_resolution_clock::now();
  double time_seq = std::chrono::duration<double, std::milli>(t2 - t1).count();

  // Bloqueo Global
  GlobalLockList global_list;
  omp_set_num_threads(num_threads);
  t1 = std::chrono::high_resolution_clock::now();
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < total_inserts; ++i)
    global_list.insert(insert_data[i]);
  int global_found = 0;
#pragma omp parallel for reduction(+ : global_found) schedule(dynamic)
  for (int i = 0; i < total_searches; ++i)
    if (global_list.search(search_data[i]))
      global_found++;
  t2 = std::chrono::high_resolution_clock::now();
  double time_global =
      std::chrono::duration<double, std::milli>(t2 - t1).count();

  // Read-Write Lock
  ReadWriteList rw_list;
  t1 = std::chrono::high_resolution_clock::now();
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < total_inserts; ++i)
    rw_list.insert(insert_data[i]);
  int rw_found = 0;
#pragma omp parallel for reduction(+ : rw_found) schedule(dynamic)
  for (int i = 0; i < total_searches; ++i)
    if (rw_list.search(search_data[i]))
      rw_found++;
  t2 = std::chrono::high_resolution_clock::now();
  double time_rw = std::chrono::duration<double, std::milli>(t2 - t1).count();

  // Validaciones de Correctitud
  assert(seq_list.count_nodes() == global_list.count_nodes());
  assert(seq_list.count_nodes() == rw_list.count_nodes());
  assert(seq_found == global_found);
  assert(seq_found == rw_found);

  csv_file << scenario_name << "," << num_threads << ",Secuencial," << time_seq
           << "," << 1.0 << "\n";
  csv_file << scenario_name << "," << num_threads << ",Bloqueo Global,"
           << time_global << "," << (time_seq / time_global) << "\n";
  csv_file << scenario_name << "," << num_threads << ",Read-Write Lock,"
           << time_rw << "," << (time_seq / time_rw) << "\n";

  std::cout << "Escenario: " << std::left << std::setw(13) << scenario_name
            << " | Hilos: " << num_threads << " | T_seq: " << std::setw(6)
            << (int)time_seq << " ms | T_glob: " << std::setw(6)
            << (int)time_global << " ms | T_fine: " << std::setw(6)
            << (int)time_rw << " ms\n";
}

int main() {
  std::ofstream csv_file("resultados_lista.csv");
  csv_file << "Escenario,Hilos,Estrategia,Tiempo_ms,Speedup\n";

  std::vector<int> thread_counts = {1, 2, 4, 6, 8};
  struct Scenario {
    std::string name;
    int inserts;
    int searches;
  };
  std::vector<Scenario> scenarios = {{"Read-Heavy", 1000, 9000},
                                     {"Balanced", 5000, 5000},
                                     {"Write-Heavy", 9000, 1000}};

  std::cout << "=== EJECUTANDO PRUEBAS DE RENDIMIENTO (INCLUYENDO OPTIMIZACIÓN "
               "RW-LOCK) ===\n";
  for (const auto &sc : scenarios) {
    for (int threads : thread_counts) {
      run_experiment(csv_file, sc.name, sc.inserts, sc.searches, threads);
    }
  }
  csv_file.close();
  std::cout << "\n[ÉXITO] Datos exportados a 'resultados_lista.csv'.\n";

  return 0;
}
