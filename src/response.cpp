#include "response.hpp"

Response::Response(string file) : fileName{file} {
  content = "../public/html/" + file;
  if (std::filesystem::exists(content))
    code = 200;
  else {
    content = "../public/html/error.html";
    code = 404;
  }
  createResponse();
}

void Response::createResponse() {
  string code_string = "../public/responses/code" + std::to_string(code) + ".txt";

  // Primero leer el contenido del html
  std::ifstream content_file(content, std::ios::in | std::ios::binary);
  if (!content_file.is_open()) {
    std::cerr << "ERROR: No se encuentra index.html en el directorio actual." << std::endl;
    content = "ERROR";
    return;
  }
  string content_string((std::istreambuf_iterator<char>(content_file)), std::istreambuf_iterator<char>());
  content_file.close();

  // Luego leer el contenido de la respuesta según el codigo y añadir lo restante (html y length)
  std::ifstream file(code_string);
  if (!file.is_open()) {
    std::cerr << "ERROR: No se encuentra responses/code200.txt" << std::endl;
    content = "ERROR";
    return;
  }
  string response((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();

  std::string target = "{{LENGTH}}";
  size_t pos = response.find(target);
  if (pos != std::string::npos)
    response.replace(pos, target.length(), std::to_string(content_string.size()));

  response.append("\r\n\r\n");
  response.append(content_string);

  content = response;
}