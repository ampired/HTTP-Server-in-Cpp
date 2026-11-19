#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using std::string;

int creaServidor(int& serverSocket, sockaddr_in& serverAddress, int numConexiones);
string crearRespuesta(int code, string body);