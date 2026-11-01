CXX = g++
CXXFLAGS = -fopenmp -std=c++17 -O2
PYTHON = python3

TARGETS = lista odd

all: $(TARGETS)

lista: lista.cpp
	$(CXX) $(CXXFLAGS) lista.cpp -o lista

odd: odd.cpp
	$(CXX) $(CXXFLAGS) odd.cpp -o odd

run-lista: lista
	./lista

run-odd: odd
	./odd

run-all: all
	./lista
	./odd

plot:
	$(PYTHON) graficas.py

clean:
	rm -f $(TARGETS) *.csv *.png
