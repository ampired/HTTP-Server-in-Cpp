#include "server.hpp"

ServerSocket::ServerSocket(size_t numConexiones, size_t tam_buffer, size_t port, string ip)
    : numConexiones{numConexiones}, tam_buffer{tam_buffer}, buffer(tam_buffer) {
  address.sin_port = htons(port);
  address.sin_family = AF_INET;
  if (ip == "ANY")
    address.sin_addr.s_addr = INADDR_ANY;
  else
    address.sin_addr.s_addr = inet_addr(ip.c_str());

  // Crea el socket del servidor --> socket_
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ == -1)
    throw std::runtime_error("ERROR: No se puede crear el socket del servidor");

  // Prepara el servidor para poder reutilizar el puerto
  int opt = 1;
  if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    throw std::runtime_error("ERROR: No se puede reutilizar el puerto");

  // Enlaza el descriptor serverSocket con la IP y Puerto configurados en serverAddress
  if (bind(socket_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1)
    throw std::runtime_error("ERROR: No se puede enlazar el serverSocket con el serverAddress");

  // Marca el socket como pasivo(Listo para recibir conexiones)
  if (listen(socket_, numConexiones) == -1)
    throw std::runtime_error("ERROR: No se puede recibir conexiones");
}

ServerSocket::~ServerSocket() {
  close(socket_);
}

string ServerSocket::crearRespuesta(string body) const {
  body = "../public/html/" + body;
  string code_string = "../public/responses/code" + std::to_string(200) + ".txt";

  // Si no existe el .html mandamos error 404
  if (!std::filesystem::exists(body)) {
    body = "../public/html/error.html";
    code_string = "../public/responses/code" + std::to_string(404) + ".txt";
  }

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

void ServerSocket::aceptarClientes() {
  while (true) {
    // Acepta conexión de un cliente y lo guarda en un descriptor de socket --> clientSocket
    int clientSocket = accept(getSocket(), nullptr, nullptr);  // accept es una operación bloqueante
    if (clientSocket == -1) continue;

    // Recepción de los datos por parte del client en el buffer
    recv(clientSocket, getReferenceToBuffer(), getBufferSize(), 0);
    std::cout << getReferenceToBuffer() << std::endl;

    // Crear y enviar respuesta al cliente en función de lo que solicite ver
    string page{};
    string aux{getBuffer()[0]};
    bool copy{false};
    bool stop{false};
    for (size_t i = 1; !stop; ++i) {
      if (aux == "/")
        copy = true;
      if (copy)
        if (aux == " ")
          stop = true;
        else
          page.append(aux);
      aux = getBuffer()[i];
    }

    if (page == "/")
      page = "index.html";

    string response = crearRespuesta(page);
    if (response == "ERROR") {
      close(clientSocket);
      continue;
    }
    send(clientSocket, response.data(), response.size(), 0);

    close(clientSocket);
  }
}

int main() {
  ServerSocket serverSocket(5, 1024, 8080);
  serverSocket.aceptarClientes();
  return 0;
}