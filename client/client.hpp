#pragma once

#include <arpa/inet.h>
#include <string>

class Client {
 public:
  Client(const std::string &server_ip, int server_port);

  void Start();

 private:
  void SendMessage(const std::string &message) const;

  [[nodiscard]] std::string ReceiveMessage() const;

  int client_socket_{};
  struct sockaddr_in server_address_{};
};
