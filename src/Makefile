# Compilador y flags
CXX      = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
LDFLAGS  = -pthread

# Nombre del ejecutable final
TARGET   = server

# Archivos fuente y objetos
SRCS     = server.cpp response.cpp
OBJS     = $(SRCS:.cpp=.o)

# Regla por defecto
all: $(TARGET)

# Enlazado del ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compilación de archivos objeto con dependencias de cabeceras
server.o: server.cpp server.hpp response.hpp
	$(CXX) $(CXXFLAGS) -c server.cpp -o server.o

response.o: response.cpp response.hpp
	$(CXX) $(CXXFLAGS) -c response.cpp -o response.o

# Ejecutar el servidor directamente
run: $(TARGET)
	./$(TARGET)

# Limpiar archivos compilados
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean