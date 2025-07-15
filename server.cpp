#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <random>
#include <map>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    const int SERVER_PORT = 4000;
    const int BROADCAST_PORT = 4001;
    int sockfd;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::perror("socket creation failed");
        return 1;
    }

    // Allow address reuse and enable broadcast
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));

    // Bind socket to local address
    struct sockaddr_in servaddr;
    std::memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::perror("bind failed");
        close(sockfd);
        return 1;
    }

    // Banner and initial info
    std::cout << "🎮*************************************🎮\n";
    std::cout << "🎮   Welcome to the UDP Guessing Game   🎮\n";
    std::cout << "🎮       Scoreboard Enabled!           🎮\n";
    std::cout << "🎮*************************************🎮\n";
    std::cout << "Server listening on port " << SERVER_PORT << "\n";

    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);
    int secret = dist(gen);
    std::cout << "🎯 Secret number (for debugging): " << secret << "\n";

    // Scoreboard: track wins by client (IP:Port)
    std::map<std::string, int> scoreboard;

    while (true) {
        char buffer[1024];
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);

        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr*)&cliaddr, &len);
        if (n < 0) {
            std::perror("recvfrom error");
            continue;
        }
        buffer[n] = '\0';
        std::string guessStr(buffer);

        int guess = 0;
        try {
            guess = std::stoi(guessStr);
        } catch (...) {
            std::string resp = "❌ Invalid input. Please send a number.";
            sendto(sockfd, resp.c_str(), resp.size(), 0,
                   (struct sockaddr*)&cliaddr, len);
            continue;
        }

        std::string clientIP = inet_ntoa(cliaddr.sin_addr);
        int clientPort = ntohs(cliaddr.sin_port);

        // Correct guess
        if (guess == secret) {
            std::cout << "Received guess " << guess << " from "
                      << clientIP << ":" << clientPort << " ✅ (Correct!) 🎯\n";

            // Update scoreboard
            std::string playerKey = clientIP + ":" + std::to_string(clientPort);
            scoreboard[playerKey]++;

            // Announce winner and new round
            std::cout << "*********************************************************\n";
            std::cout << "🏆 Player " << playerKey << " guessed the number correctly! 🎉🎉🎉\n";

            // Print scoreboard
            std::cout << "📊 Current Scoreboard:\n";
            for (const auto &entry : scoreboard) {
                std::cout << "   " << entry.first << " - " << entry.second << "\n";
            }

            std::cout << "🔄 A new number has been generated. Game continues! 🎲\n";
            std::cout << "*********************************************************\n";

            // Send "Correct!" to winner
            std::string response = "✅ Correct! 🎉 You nailed it! 🏆";
            sendto(sockfd, response.c_str(), response.size(), 0,
                   (struct sockaddr*)&cliaddr, len);

            // Broadcast win to everyone
            std::string winMsg = "🎉 Player " + playerKey +
                                 " guessed it right! New number generated! 🎯";

            struct sockaddr_in bcastaddr;
            std::memset(&bcastaddr, 0, sizeof(bcastaddr));
            bcastaddr.sin_family = AF_INET;
            bcastaddr.sin_port = htons(BROADCAST_PORT);
            bcastaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

            sendto(sockfd, winMsg.c_str(), winMsg.size(), 0,
                   (struct sockaddr*)&bcastaddr, sizeof(bcastaddr));

            // Reset secret number
            secret = dist(gen);
            std::cout << "🎯 New secret number (for debugging): " << secret << "\n";

        } else {
            std::string hint = (guess < secret) ? "Too low 🧊" : "Too high 🔥";

            std::cout << "Received guess " << guess << " from "
                      << clientIP << ":" << clientPort
                      << " ❌ (" << hint << ")\n";

            sendto(sockfd, hint.c_str(), hint.size(), 0,
                   (struct sockaddr*)&cliaddr, len);
        }
    }

    close(sockfd);
    return 0;
}
