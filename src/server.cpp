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

  // Mapa que sirve como "cache" para los archivos estáticos más solicitados
  vector<string> archivos_cacheados{"index.html", "contact.html", "about.html"};
  for (auto x : archivos_cacheados)
    responses.emplace(x, Response(x));
}

ServerSocket::~ServerSocket() {
  close(socket_);
}

void ServerSocket::aceptarClientes_seq() {
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
      if (copy) {
        if (aux == " ")
          stop = true;
        else
          page.append(aux);
      }
      aux = getBuffer()[i];
    }

    if (page == "/")
      page = "index.html";

    string response = Response(page).getResponse();

    if (response == "ERROR") {
      close(clientSocket);
      continue;
    }
    send(clientSocket, response.data(), response.size(), 0);

    close(clientSocket);
  }
}

void ServerSocket::aceptarClientes_conc(size_t numHilos) {
  boost::asio::thread_pool pool(numHilos);

  while (true) {
    int clientSocket = accept(getSocket(), nullptr, nullptr);  // accept es una operación bloqueante
    if (clientSocket == -1) continue;

    boost::asio::post(pool,
        [this, clientSocket]() {
          // Creación de un buffer local en cada hebra para evitar condiciones de carrera
          vector<char> buffer_local(getBufferSize(), 0);

          // Recepción de los datos por parte del client en el buffer
          recv(clientSocket, buffer_local.data(), buffer_local.size(), 0);
          std::cout << buffer_local.data() << std::endl;

          // Crear y enviar respuesta al cliente en función de lo que solicite ver
          string page{};
          string aux{buffer_local[0]};
          bool copy{false};
          bool stop{false};
          for (size_t i = 1; !stop && i < buffer_local.size(); ++i) {
            if (aux == "/")
              copy = true;
            if (copy) {
              if (aux == " ")
                stop = true;
              else
                page.append(aux);
            }
            aux = buffer_local[i];
          }

          if (page == "/")
            page = "index.html";

          auto it = responses.find(page);
          string response;
          if (it != responses.cend()) // Si es distinto significa que existe
            response = it->second.getResponse();
          else
            response = Response(page).getResponse();

          if (response == "ERROR") {
            close(clientSocket);
            return;
          }

          send(clientSocket, response.data(), response.size(), 0);

          close(clientSocket);
        });
  }
}

int main() {
  ServerSocket serverSocket(5, 1024, 8080);
  // serverSocket.aceptarClientes_seq();
  serverSocket.aceptarClientes_conc(8);
  return 0;
}