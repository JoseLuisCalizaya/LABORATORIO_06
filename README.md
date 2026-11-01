Guia de Ejecucion - Laboratorio OpenMP

Instrucciones paso a paso para compilar, ejecutar y generar los graficos del laboratorio de programación paralela con OpenMP (Lista Enlazada y Odd-Even Transposition Sort).

1. Requisitos Previos

Asegúrate de contar con las siguientes herramientas instaladas en el sistema:

Compilador C++ con soporte para OpenMP y C++17 (g++).

Python 3 y las librerias para procesamiento de datos y visualizacion:

pip install pandas matplotlib seaborn


2. Ejecucion Rapida usando Makefile (Recomendado)

El archivo Makefile ya incluye la bandera -fopenmp necesaria para compilar el codigo paralelo.

Paso 1: Compilar ambos programas

make all


Paso 2: Ejecutar los experimentos

Ejecutar Parte I (Lista Enlazada):

make run-lista


Ejecutar Parte II (Odd-Even Sort):

make run-odd


Ejecutar ambas partes en secuencia:

make run-all


Paso 3: Generar los graficos de rendimiento

Procesa los archivos .csv generados y exporta las imagenes .png:

make plot


Paso 4: Limpiar archivos generados

Elimina binarios ejecutables, archivos .csv de resultados y graficos .png:

make clean


3. Ejecucion Manual (Sin Makefile)

Si prefieres ejecutar cada comando directamente en la terminal:

Paso 1: Compilar los archivos C++ con OpenMP

# Parte I: Lista Enlazada
g++ -fopenmp -std=c++17 -O2 lista.cpp -o lista

# Parte II: Odd-Even Transposition Sort
g++ -fopenmp -std=c++17 -O2 odd.cpp -o odd


Paso 2: Ejecutar los binarios

# Genera los datos de la Parte I
./lista

# Genera el archivo resultados_oddeven_5rep.csv
./odd


Paso 3: Generar las graficas con Python

python3 graficas.py


4. Notas de Compatibilidad

En macOS: El compilador predeterminado clang puede no soportar OpenMP directamente. Si usas brew install libomp o gcc instalado via Homebrew, ajusta la compilacion con:

g++ -Xpreprocessor -fopenmp -lomp -std=c++17 -O2 odd.cpp -o odd

