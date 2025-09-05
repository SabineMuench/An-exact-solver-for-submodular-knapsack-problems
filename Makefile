# Compiler und Compiler-Optionen
CXX = g++
CXXFLAGS = -O2 -std=c++17

# Quell-Dateien
SRCS = main.cpp Load.cpp functions.cpp DCOSUBSolver.cpp ACRSolver.cpp LESolver.cpp LECRSolver.cpp

# Das Ziel, das erstellt werden soll
TARGET = main

# Die Standardregel: Ziel erstellen
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Eine Regel zum Löschen der erstellten Dateien
clean:
	rm -f $(TARGET) *.o

# Eine Regel, um nur Objektdateien zu erstellen (optional)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

