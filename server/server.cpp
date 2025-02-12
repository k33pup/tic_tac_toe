#include <iostream>
#include <unistd.h>
#include <mutex>

#include "server.hpp"
#include "game.hpp"

Server::Server(int port) : address_repr_("127.0.0.1:" + std::to_string(port)) {
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  server_address_.sin_family = AF_INET;
  server_address_.sin_port = htons(port);
  server_address_.sin_addr.s_addr = INADDR_ANY;
}

void Server::Start() {
  bind(server_socket_, reinterpret_cast<sockaddr*>(&server_address_), sizeof(server_address_));
  listen(server_socket_, 100);
  std::cout << "Listening for incoming connections on address " << address_repr_ << "..." << std::endl;
  AcceptConnections();
}

void Server::AcceptConnections() {
  while (true) {
    int player_socket = accept(server_socket_, nullptr, nullptr);
    if (player_socket < 0) {
      std::cerr << "Failed to make a connection" << std::endl;
      continue;
    }
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      player_queue_.push(player_socket);
    }
    std::cout << "Player" << " " << player_socket << " " << "connected..." << std::endl;
    if (player_queue_.size() >= 2) {
      int first_player = -1;
      int second_player = -1; {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        first_player = player_queue_.front();
        player_queue_.pop();
        second_player = player_queue_.front();
        player_queue_.pop();
      }
      std::cout << "Starting game session..." << std::endl;
      std::thread(&Server::HandleGame, this, first_player, second_player).detach();
    } else {
      SendMessage(player_socket, "Please wait for the other player to connect...\n");
    }
  }
}

void Server::HandleGame(int first_player, int second_player) {
  Game game;
  int cur_player = first_player;
  int opponent = second_player;
  bool has_ended = false;
  SendMessage(first_player, "Player 2 connected! Game started! You are X. Good luck!\n");
  SendMessage(second_player, "Player 1 connected! Game started! You are 0. Good luck!\n");
  try {
    while (!has_ended) {
      SendMessage(cur_player, "The board is\n" + game.BoardRepr() + "\nEnter your turn: ");
      SendMessage(opponent, "The board is\n" + game.BoardRepr() + "\nWaiting for opponent turn...\n");
      int player_move = std::stoi(ReceiveMessage(cur_player));
      if (game.MakeMove(cur_player == first_player ? 1 : 2, player_move)) {
        if (game.CheckWin(cur_player == first_player ? 1 : 2)) {
          std::cout << "Player" << " " << cur_player << " " << "won" << std::endl;
          SendMessage(cur_player, "Congratulations! You won!\n");
          SendMessage(opponent, "Sorry! You lost!\n");
          has_ended = true;
        } else if (game.IsDraw()) {
          std::cout << "It is a draw" << std::endl;
          SendMessage(cur_player, "It's a draw!\n");
          SendMessage(opponent, "It's a draw!\n");
          has_ended = true;
        } else {
          std::swap(cur_player, opponent);
        }
      } else {
        SendMessage(cur_player, "Invalid move. Try again.\n");
      }
    }
  } catch (const std::exception& ex) {
    std::cerr << "Stopping game due to exception: " << ex.what() << std::endl;
  }
  std::cout << "Disconnecting player " << first_player << " and " << second_player << std::endl;
  CloseConnection(first_player);
  CloseConnection(second_player);
}

void Server::SendMessage(int player_socket, const std::string &message) {
  send(player_socket, message.c_str(), message.size(), 0);
}

std::string Server::ReceiveMessage(int player_socket) {
  char buffer[1024];
  size_t bytes_received = recv(player_socket, buffer, sizeof(buffer) - 1, 0);
  if (bytes_received <= 0) {
    throw std::runtime_error("Client disconnected or error receiving data");
  }
  return {buffer, bytes_received};
}

void Server::CloseConnection(int player_socket) {
  close(player_socket);
}

Server::~Server() {
  std::cout << "Shutting down server..." << std::endl;
  close(server_socket_);
}

int main(int argc, char *argv[]) {
  int port = 1234;

  if (argc == 3 && std::string(argv[1]) == "--port") {
    port = std::stoi(argv[2]);
  } else {
    std::cerr << "Usage: " << argv[0] << " --port <port_number>" << std::endl;
    return 1;
  }

  try {
    Server server(port);
    server.Start();
  } catch (const std::exception &ex) {
    std::cerr << "Server error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
