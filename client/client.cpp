#include "client.hpp"

#include <iostream>
#include <unistd.h>

Client::Client(const std::string &server_ip, int server_port) {
  client_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  server_address_.sin_family = AF_INET;
  server_address_.sin_port = htons(server_port);
  if (inet_pton(AF_INET, server_ip.c_str(), &server_address_.sin_addr) <= 0) {
    std::cerr << "Wrong address!" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Client::SendMessage(const std::string &message) const {
  send(client_socket_, message.c_str(), message.size(), 0);
}

std::string Client::ReceiveMessage() const {
  char buffer[1024];
  size_t response_size = recv(client_socket_, buffer, 1024, 0);
  if (response_size <= 0) {
    return "";
  }
  return {buffer, response_size};
}

void Client::Start() {
  if (connect(client_socket_, reinterpret_cast<sockaddr*>(&server_address_), sizeof(server_address_)) < 0) {
    std::cerr << "Failed to connect to server!" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "Connected to server!" << std::endl;
  while (true) {
    std::string response = ReceiveMessage();
    std::cout << response;
    if (response.find("your turn") != std::string::npos) {
      int move;
      std::cin >> move;
      SendMessage(std::to_string(move));
    }
    if (response.find("won") != std::string::npos ||
        response.find("lost") != std::string::npos ||
        response.find("draw") != std::string::npos) {
      break;
    }
  }
  close(client_socket_);
}

int main(int argc, char *argv[]) {
  if (argc != 3 || std::string(argv[1]) != "--connect") {
    std::cerr << "Usage: " << argv[0] << " --connect <IP>:<PORT>" << std::endl;
    return 1;
  }
  std::string connection_string = argv[2];
  size_t delim_pos = connection_string.find(':');
  if (delim_pos == std::string::npos) {
    std::cerr << "Invalid format. Use <IP>:<PORT>" << std::endl;
    return 1;
  }
  std::string server_ip = connection_string.substr(0, delim_pos);
  int server_port = std::stoi(connection_string.substr(delim_pos + 1));
  try {
    Client client(server_ip, server_port);
    client.Start();
  } catch (const std::exception &e) {
    std::cerr << "Client error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
