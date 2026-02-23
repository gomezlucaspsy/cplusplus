#include "network.h"
#include "encryption.h"
#include "config.h"
#include <iostream>
#include <cstring>
#include <functional>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define closesocket(s) close(s)
typedef int SOCKET;
#endif

int Network::serverSocket = INVALID_SOCKET;
int Network::clientSocket = INVALID_SOCKET;
bool Network::running = false;
std::mutex Network::socketMutex;
Network::MessageCallback Network::messageCallback = nullptr;
std::thread Network::receiveThread;

bool Network::InitializeSockets() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return false;
    }
#endif
    return true;
}

void Network::CleanupSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

bool Network::StartServer(int port, MessageCallback callback) {
    if (!InitializeSockets()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(socketMutex);
    
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        return false;
    }
    
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    
    int reuse = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
        return false;
    }
    
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
        return false;
    }
    
    running = true;
    messageCallback = callback;
    
    // Accept connection in separate thread
    receiveThread = std::thread([]() {
        sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        
        clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);
        if (clientSocket != INVALID_SOCKET) {
            ReceiveLoop();
        }
    });
    
    return true;
}

bool Network::Connect(const std::string& host, int port, MessageCallback callback) {
    if (!InitializeSockets()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(socketMutex);
    
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        return false;
    }
    
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }
    
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }
    
    running = true;
    messageCallback = callback;
    receiveThread = std::thread(ReceiveLoop);
    
    return true;
}

bool Network::SendEncryptedMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(socketMutex);
    
    int target = (clientSocket != INVALID_SOCKET) ? clientSocket : serverSocket;
    if (target == INVALID_SOCKET) {
        return false;
    }
    
    auto encrypted = Encryption::Encrypt(message);
    
    // Send size first (4 bytes)
    uint32_t size = htonl(encrypted.size());
    if (send(target, (const char*)&size, 4, 0) == SOCKET_ERROR) {
        return false;
    }
    
    // Send encrypted data
    if (send(target, (const char*)encrypted.data(), encrypted.size(), 0) == SOCKET_ERROR) {
        return false;
    }
    
    return true;
}

void Network::ReceiveLoop() {
    uint8_t buffer[Config::MAX_MESSAGE_SIZE];
    
    while (running) {
        int target = (clientSocket != INVALID_SOCKET) ? clientSocket : serverSocket;
        if (target == INVALID_SOCKET) break;
        
        // Receive size
        uint32_t size_net;
        int recvBytes = recv(target, (char*)&size_net, 4, 0);
        if (recvBytes <= 0) break;
        
        uint32_t size = ntohl(size_net);
        if (size > Config::MAX_MESSAGE_SIZE) break;
        
        // Receive data
        int totalRecv = 0;
        while (totalRecv < (int)size) {
            int received = recv(target, (char*)buffer + totalRecv, size - totalRecv, 0);
            if (received <= 0) break;
            totalRecv += received;
        }
        
        if (totalRecv == (int)size) {
            std::vector<uint8_t> ciphertext(buffer, buffer + size);
            std::string decrypted = Encryption::Decrypt(ciphertext);
            
            if (messageCallback) {
                messageCallback(decrypted, false);
            }
        }
    }
}

void Network::Shutdown() {
    running = false;
    
    {
        std::lock_guard<std::mutex> lock(socketMutex);
        if (clientSocket != INVALID_SOCKET) {
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
        }
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
        }
    }
    
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
    
    CleanupSockets();
}

bool Network::IsConnected() {
    std::lock_guard<std::mutex> lock(socketMutex);
    return clientSocket != INVALID_SOCKET && running;
}

bool Network::IsServerRunning() {
    std::lock_guard<std::mutex> lock(socketMutex);
    return serverSocket != INVALID_SOCKET && running;
}
