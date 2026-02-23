#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <thread>
#include <mutex>
#include <functional>

class Network {
public:
    using MessageCallback = std::function<void(const std::string&, bool)>;
    
    // Server mode - Listen for incoming connections
    static bool StartServer(int port, MessageCallback callback);
    
    // Client mode - Connect to server
    static bool Connect(const std::string& host, int port, MessageCallback callback);
    
    // Send encrypted message
    static bool SendEncryptedMessage(const std::string& message);
    
    // Stop server/client
    static void Shutdown();
    
    // Get connection status
    static bool IsConnected();
    
    // Get server status
    static bool IsServerRunning();
    
private:
    static int serverSocket;
    static int clientSocket;
    static bool running;
    static std::mutex socketMutex;
    static MessageCallback messageCallback;
    static std::thread receiveThread;
    
    // Platform-specific socket initialization
    static bool InitializeSockets();
    static void CleanupSockets();
    
    // Receive loop
    static void ReceiveLoop();
};

#endif // NETWORK_H
