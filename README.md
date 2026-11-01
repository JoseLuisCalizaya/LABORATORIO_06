# Guia de Ejecucion - Laboratorio OpenMP

Instrucciones paso a paso para compilar, ejecutar y generar los graficos del laboratorio de programacion paralela con OpenMP (Lista Enlazada y Odd-Even Transposition Sort).

## 1. Requisitos Previos

Asegurate de contar con las siguientes herramientas instaladas en el sistema:

- Compilador C++ con soporte para OpenMP y C++17 (`g++`).
- Python 3 y las librerias para procesamiento de datos y visualizacion:

```bash
pip install pandas matplotlib seaborn
```

## 2. Ejecucion Rapida usando Makefile (Recomendado)

El archivo `Makefile` ya incluye la bandera `-fopenmp` necesaria para compilar el codigo paralelo.

**Paso 1: Compilar ambos programas**

```bash
make all
```

**Paso 2: Ejecutar los experimentos**

Ejecutar Parte I (Lista Enlazada):

```bash
make run-lista
```

Ejecutar Parte II (Odd-Even Sort):

```bash
make run-odd
```

Ejecutar ambas partes en secuencia:

```bash
make run-all
```

**Paso 3: Generar los graficos de rendimiento**

Procesa los archivos `.csv` generados y exporta las imagenes `.png`:

```bash
make plot
```

**Paso 4: Limpiar archivos generados**

Elimina binarios ejecutables, archivos `.csv` de resultados y graficos `.png`:

```bash
make clean
```

## 3. Ejecucion Manual (Sin Makefile)

Si prefieres ejecutar cada comando directamente en la terminal:

**Paso 1: Compilar los archivos C++ con OpenMP**

Parte I: Lista Enlazada

```bash
g++ -fopenmp -std=c++17 -O2 p3.cpp -o lista
```

Parte II: Odd-Even Transposition Sort

```bash
g++ -fopenmp -std=c++17 -O2 p5.cpp -o odd
```

**Paso 2: Ejecutar los binarios**

Genera los datos de la Parte I (`resultados_lista.csv`):

```bash
./lista
```

Genera el archivo `resultados_oddeven_5rep.csv`:

```bash
./odd
```

**Paso 3: Generar las graficas con Python**

```bash
python3 graficas.py
```

