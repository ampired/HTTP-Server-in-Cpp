#include "server.hpp"

// Devuelve 1 si da error
int creaServidor(int& serverSocket, sockaddr_in& serverAddress, int numConexiones) {
  // Crea el socket del servidor --> serverSocket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    std::cerr << "ERROR: No se puede crear el socket del servidor" << std::endl;
    return 1;
  }

  // Configura la direccion y el puerto del servidor --> serverAddress
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(8080);
  serverAddress.sin_family = AF_INET;

  // Prepara el servidor para poder reutilizar el puerto
  int opt = 1;
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
    std::cerr << "ERROR: No se puede reutilizar el puerto" << std::endl;
    close(serverSocket);
    return 1;
  }

  // Enlaza el descriptor serverSocket con la IP y Puerto configurados en serverAddress
  if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
    std::cerr << "ERROR: No se puede enlazar el serverSocket con el serverAddress" << std::endl;
    close(serverSocket);
    return 1;
  }
  // Marca el socket como pasivo(Listo para recibir conexiones)
  if (listen(serverSocket, numConexiones) == -1) {
    std::cerr << "ERROR: No se puede recibir conexiones" << std::endl;
    close(serverSocket);
    return 1;
  }

  return 0;
}

// Devuelve "ERROR" si da error
string crearRespuesta(int code, string body) {
  body = "html/" + body + ".html";
  string code_string = "responses/code" + std::to_string(code) + ".txt";

  // Primero leer el contenido del html
  std::ifstream content_file(body, std::ios::in | std::ios::binary);
  if (!content_file.is_open()) {
    std::cerr << "ERROR: No se encuentra index.html en el directorio actual." << std::endl;
    return "ERROR";
  }
  string content_string((std::istreambuf_iterator<char>(content_file)), std::istreambuf_iterator<char>());
  content_file.close();

  // Luego leer el contenido de la respuesta según el codigo y añadir lo restante (html y length)
  std::ifstream file(code_string);
  if (!file.is_open()) {
    std::cerr << "ERROR: No se encuentra responses/code200.txt" << std::endl;
    return "ERROR";
  }
  string response((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();

  std::string target = "{{LENGTH}}";
  size_t pos = response.find(target);
  if (pos != std::string::npos)
    response.replace(pos, target.length(), std::to_string(content_string.size()));

  response.append("\r\n\r\n");
  response.append(content_string);

  return response;
}

int main() {
  int serverSocket{};
  sockaddr_in serverAddress{};
  size_t numConexiones = 5; // Num máximo de conexiones simultáneas
  if (creaServidor(serverSocket, serverAddress, numConexiones) == 1) return 1;  // Crea el servidor y a la vez comprueba si falla

  // Acepta conexión de un cliente y lo guarda en un descriptor de socket --> clientSocket
  while (true) {
    int clientSocket = accept(serverSocket, nullptr, nullptr);  // accept es una operación bloqueante
    if (clientSocket == -1) return 1;

    // Recepción de los datos por parte del client en el buffer
    char buffer[1024]{};
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << buffer << std::endl;

    // Crear y enviar respuesta al cliente
    string response = crearRespuesta(200, "index");
    if (response == "ERROR") {
      close(clientSocket);
      continue;
    }
    send(clientSocket, response.data(), response.size(), 0);

    close(clientSocket);
  }
  close(serverSocket);

  return 0;
}