#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <boost/asio.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "response.hpp"

using std::map;
using std::string;
using std::vector;

class ServerSocket {
 public:
  // Configura la direccion y puerto (en address), numConexiones y tam_buffer del servidor; y crea el socket del servidor
  explicit ServerSocket(size_t numConexiones, size_t tam_buffer, size_t port, string ip = "ANY");
  ~ServerSocket();

  // Aceptar conexiones (secuenciales) de clientes
  void aceptarClientes_seq();
  // Aceptar conexiones (concurrnetes) de clientes
  void aceptarClientes_conc(size_t numHilos);

  // Devuelve el descriptor del socket del server
  int getSocket() const { return socket_; }
  // Devuelve referencia al buffer
  char* getReferenceToBuffer() { return buffer.data(); }
  // Devuelve el buffer
  vector<char> getBuffer() const { return buffer; }
  // Devuelve tamaño del buffer
  size_t getBufferSize() const { return tam_buffer; }

 private:
  int socket_;
  sockaddr_in address;

  size_t numConexiones;
  size_t tam_buffer;
  vector<char> buffer;
  map<string, Response> responses;
};