#include "client.hpp"

int main() {
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in serverAddress;
  serverAddress.sin_port = htons(80);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_family = AF_INET;

  connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));

  const char* mensaje = "HOLA";
  send(clientSocket, mensaje, strlen(mensaje), 0);

  close(clientSocket);
}