#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstddef>
#include <iostream>
#include <list>
#include <stdexcept>

using std::list;

void crearServidor(int& serverSocket, sockaddr_in& serverAddress, size_t numConexiones);
void aceptarClientes(int& serverSocket, list<int>& clientSocket);