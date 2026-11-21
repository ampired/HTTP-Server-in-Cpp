#include "server.hpp"

void crearServidor(int& serverSocket, sockaddr_in& serverAddress, size_t numConexiones) {
  // Crea el socket del servidor --> serverSocket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) throw std::runtime_error("Error al crear el servidor");

  // Configura la direccion y el puerto del servidor --> serverAddress
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(80);
  serverAddress.sin_family = AF_INET;

  // Enlaza el descriptor serverSocket con la IP y Puerto configurados en serverAddress
  bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));
  // Marca el socket como pasivo(Listo para recibir conexiones)
  listen(serverSocket, numConexiones);
}

void aceptarClientes(int& serverSocket, list<int>& clientsSockets) {
  // Acepta conexión de un cliente y lo guarda en un descriptor de socket --> clientSocket
  int client = accept(serverSocket, nullptr, nullptr); // accept es una operación bloqueante
  if (client == -1) throw std::runtime_error("Error al aceptar cliente");
  clientsSockets.insert(clientsSockets.end(), client);
}

int main() {
  try {
    int serverSocket{};
    sockaddr_in serverAddress{};
    size_t numConexiones = 5;
    list<int> clientsSockets{};
    char buffer[1024]{};

    crearServidor(serverSocket, serverAddress, numConexiones);
    aceptarClientes(serverSocket, clientsSockets);
    // Recepción de los datos por parte del client en el buffer
    recv(clientsSockets.front(), buffer, sizeof(buffer), 0);
    close(serverSocket);

    std::cout << buffer << std::endl;

  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
  }

  // Curiosidad: Ver como se traduce el puerto para el socket
  //   std::cout << "Puerto (Host Byte Order): " << 80 << std::endl;
  //   std::cout << "Puerto (Network Byte Order): " << htons(80) << std::endl;

  return 0;
}
