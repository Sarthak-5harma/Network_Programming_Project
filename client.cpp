#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Thread function: listen for broadcast messages on port 4001
void listenBroadcast(int sock) {
    char buffer[1024];
    while (true) {
        struct sockaddr_in srcAddr;
        socklen_t addrlen = sizeof(srcAddr);
        int n = recvfrom(sock, buffer, sizeof(buffer)-1, 0,
                         (struct sockaddr*)&srcAddr, &addrlen);
        if (n > 0) {
            buffer[n] = '\0';
            std::cout << "\n[BROADCAST] " << buffer << std::endl;
            std::cout << "Enter your guess (or 'exit'): " << std::flush;
        }
    }
}

int main(int argc, char* argv[]) {
    // Default to localhost:4000
    std::string serverIp = "127.0.0.1";
    int serverPort = 4000;
    if (argc >= 2) serverIp = argv[1];
    if (argc >= 3) serverPort = std::atoi(argv[2]);

    // Client banner
    std::cout << "🎮*************************************🎮\n";
    std::cout << "🎮   UDP Guessing Game Client Started  🎮\n";
    std::cout << "🎮*************************************🎮\n";

    // Create UDP socket for sending guesses
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::perror("socket creation failed");
        return 1;
    }

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid server IP address\n";
        return 1;
    }

    // Create UDP socket for broadcast listening (port 4001)
    int bcastSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bcastSock < 0) {
        std::perror("broadcast socket creation failed");
        return 1;
    }
    int opt = 1;
    setsockopt(bcastSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    struct sockaddr_in bcastAddr;
    std::memset(&bcastAddr, 0, sizeof(bcastAddr));
    bcastAddr.sin_family = AF_INET;
    bcastAddr.sin_addr.s_addr = INADDR_ANY;
    bcastAddr.sin_port = htons(4001);
    if (bind(bcastSock, (struct sockaddr*)&bcastAddr, sizeof(bcastAddr)) < 0) {
        std::perror("bind failed");
        return 1;
    }

    // Start thread to listen for broadcast messages
    std::thread listener(listenBroadcast, bcastSock);
    listener.detach();

    std::cout << "Connected to server " << serverIp << ":" << serverPort << "\n";

    // Main loop: read user input, send guess, receive response
    std::string guess;
    while (true) {
        std::cout << "Enter your guess (or 'exit'): ";
        if (!std::getline(std::cin, guess)) break;
        if (guess == "exit" || guess == "quit") break;
        // Validate numeric input
        try {
            int val = std::stoi(guess);
        } catch (...) {
            std::cout << "Please enter a valid number or 'exit'.\n";
            continue;
        }
        // Send guess to server
        sendto(sock, guess.c_str(), guess.size(), 0,
               (struct sockaddr*)&serverAddr, sizeof(serverAddr));

        // Receive response from server
        char buffer[1024];
        int n = recvfrom(sock, buffer, sizeof(buffer)-1, 0, nullptr, nullptr);
        if (n > 0) {
            buffer[n] = '\0';
            std::cout << buffer << std::endl;
        }
    }

    close(sock);
    close(bcastSock);
    std::cout << "Client exiting.\n";
    return 0;
}
