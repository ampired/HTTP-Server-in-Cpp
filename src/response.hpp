#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using std::string;

class Response {
 public:
  Response() = default;
  explicit Response(string file);

  string getResponse() const { return content; }
  string getFileName() const { return fileName; }

 private:
  void createResponse();

  string fileName;
  string content;
  int code;
};