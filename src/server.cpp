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
  vector<string> archivos_cacheados{"/index.html", "/contact.html", "/about.html"};
  for (auto x : archivos_cacheados)
    responses.emplace(x, Response(x));
}

ServerSocket::~ServerSocket() {
  close(socket_);
}

void ServerSocket::aceptarClientes(size_t numHilos) {
  boost::asio::thread_pool pool(numHilos);

  while (true) {
    int clientSocket = accept(getSocket(), nullptr, nullptr);  // accept es una operación bloqueante
    if (clientSocket == -1) continue;

    boost::asio::post(pool,
        [this, clientSocket]() {
          // Creación de un buffer local en cada hebra para evitar condiciones de carrera
          vector<char> buffer_local(getBufferSize(), 0);

          // Recepción de los datos por parte del client en el buffer
          ssize_t bytes = recv(clientSocket, buffer_local.data(), buffer_local.size(), 0);
          if (bytes <= 0) {
            close(clientSocket);
            return;
          }
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

          // Mejor envolver al send en un while por si hay alguna pérdida de bytes al enviar los datos
          size_t total_bytes_enviados = 0;
          while (total_bytes_enviados < response.size()) {
            ssize_t bytes_enviados = send(clientSocket, response.data(), response.size(), 0);

            if (bytes_enviados <= 0) {
              close(clientSocket);
              break;
            }

            total_bytes_enviados += bytes_enviados;
          }

          close(clientSocket);
        });
  }
}

int main(int argc, char* argv[]) {
  // Valores por defecto
  size_t numConexiones = 128;
  size_t tam_buffer = 4096;
  size_t puerto = 8080;

  int opcion;
  while ((opcion = getopt(argc, argv, "hp:n:b:")) != -1) {
    switch (opcion) {
      case 'h':
        std::cout << "Uso: " << argv[0] << " [-h] [-p puerto] [-n numConexiones] [-b tamBuffer]" << std::endl;
        return 0;
      case 'p':
        puerto = std::stoi(optarg);
        break;
      case 'n':
        numConexiones = std::stoi(optarg);
        break;
      case 'b':
        tam_buffer = std::stoi(optarg);
        break;
      case '?': // Se ejecuta si pasan una opción desconocida o falta un valor
        std::cout << "Uso: " << argv[0] << " [-h] [-p puerto] [-n numConexiones] [-b tamBuffer]" << std::endl;
        return 1;
    }
  }

  ServerSocket serverSocket(numConexiones, tam_buffer, puerto);
  serverSocket.aceptarClientes(8);
  return 0;
}