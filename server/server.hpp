#pragma once

#include <netinet/in.h>
#include <thread>

class Server {
 public:
  explicit Server(int port);

  void Start();

  ~Server();

 private:
  void SendMessage(int player_socket, const std::string &message);

  std::string ReceiveMessage(int player_socket);

  void CloseConnection(int player_socket);

  void AcceptConnections();

  void HandleGame(int first_player, int second_player);

  std::string address_repr_;
  int server_socket_;
  struct sockaddr_in server_address_{};
  std::queue<int> player_queue_;
  std::mutex queue_mutex_;
};
