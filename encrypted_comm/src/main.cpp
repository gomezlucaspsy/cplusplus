#include "ui.h"
#include "network.h"
#include "encryption.h"
#include "utils.h"
#include "config.h"
#include <iostream>
#include <thread>
#include <string>

bool isRunning = true;
std::string userName = "User";
std::string localIP = "";

void OnMessageReceived(const std::string& message, bool isOwn) {
    UI::PrintMessage("Remote", message, isOwn);
}

int main() {
    // Initialize
    UI::Initialize();
    
    // Get username
    UI::SetStatus("Enter encryption password");
    std::cout << "\n";
    SetColor(Config::Colors::BRIGHT_YELLOW);
    std::cout << "Enter your username: ";
    ResetColor();
    std::getline(std::cin, userName);
    
    SetColor(Config::Colors::BRIGHT_YELLOW);
    std::cout << "Enter encryption password: ";
    ResetColor();
    std::string password;
    std::getline(std::cin, password);
    
    // Initialize encryption
    if (!Encryption::InitializeKey(password)) {
        SetColor(Config::Colors::BRIGHT_RED);
        std::cout << "Failed to initialize encryption!" << std::endl;
        ResetColor();
        return 1;
    }
    
    // Show connection menu
    int choice = UI::ShowConnectionMenu();
    
    UI::Initialize();
    UI::SetStatus("Connecting...");
    
    localIP = Utils::GetLocalIP();
    
    if (choice == 1) {
        // Server mode
        if (!Network::StartServer(Config::DEFAULT_PORT, OnMessageReceived)) {
            SetColor(Config::Colors::BRIGHT_RED);
            std::cout << "Failed to start server!" << std::endl;
            ResetColor();
            return 1;
        }
        
        UI::SetStatus("Server running on " + localIP + ":" + std::to_string(Config::DEFAULT_PORT) + " - Waiting for connection...");
        UI::PrintMessage(userName, "Server started. Waiting for client connection...", true);
        
    } else if (choice == 2) {
        // Client mode
        UI::SetStatus("Connecting...");
        
        SetColor(Config::Colors::BRIGHT_YELLOW);
        std::cout << "\nEnter server IP: ";
        ResetColor();
        std::string serverIP;
        std::getline(std::cin, serverIP);
        
        SetColor(Config::Colors::BRIGHT_YELLOW);
        std::cout << "Enter server port (default " << Config::DEFAULT_PORT << "): ";
        ResetColor();
        std::string portStr;
        std::getline(std::cin, portStr);
        
        int port = Config::DEFAULT_PORT;
        if (!portStr.empty()) {
            try {
                port = std::stoi(portStr);
            } catch (...) {
                port = Config::DEFAULT_PORT;
            }
        }
        
        if (!Network::Connect(serverIP, port, OnMessageReceived)) {
            SetColor(Config::Colors::BRIGHT_RED);
            std::cout << "Failed to connect to server!" << std::endl;
            ResetColor();
            return 1;
        }
        
        UI::SetStatus("Connected to " + serverIP + ":" + std::to_string(port));
        UI::PrintMessage(userName, "Connected to server", true);
        
    } else {
        return 0;
    }
    
    // Main message loop
    UI::ClearScreen();
    UI::Draw();
    
    while (isRunning) {
        std::string input = UI::GetInput();
        
        if (input.empty()) continue;
        
        if (input == "/exit") {
            UI::SetStatus("Exiting...");
            isRunning = false;
            break;
        } else if (input == "/help") {
            UI::ShowHelp();
            UI::Draw();
        } else if (input == "/status") {
            std::string status = "Connected";
            if (Network::IsConnected()) {
                status = "Client Connected";
            } else if (Network::IsServerRunning()) {
                status = "Server Running - Waiting for connection";
            }
            UI::SetStatus(status);
        } else if (input == "/clear") {
            UI::ClearScreen();
            UI::Draw();
        } else if (input[0] == '/') {
            UI::PrintMessage(userName, "Unknown command: " + input, true);
        } else {
            // Send message
            if (!Network::SendMessage(input)) {
                UI::PrintMessage(userName, "[FAILED TO SEND] " + input, true);
            } else {
                UI::PrintMessage(userName, input, true);
            }
        }
    }
    
    Network::Shutdown();
    
    SetColor(Config::Colors::BRIGHT_GREEN);
    std::cout << "\nThank you for using " << Config::APP_NAME << std::endl;
    ResetColor();
    
    return 0;
}

// Helper functions for UI color setting
void SetColor(const std::string& color) {
    std::cout << color;
}

void ResetColor() {
    std::cout << Config::Colors::RESET;
}
