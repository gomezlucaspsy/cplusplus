#include "ui.h"
#include "config.h"
#include <iostream>
#include <iomanip>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <cstdlib>
#include <sys/ioctl.h>
#include <termios.h>
#endif

std::vector<std::string> UI::messageHistory;
std::string UI::currentStatus = "Ready";
int UI::maxLines = 24;
int UI::maxCols = 80;

void UI::Initialize() {
    ClearScreen();
    Draw();
}

void UI::ClearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void UI::SetColor(const std::string& color) {
    std::cout << color;
}

void UI::ResetColor() {
    std::cout << Config::Colors::RESET;
}

void UI::GotoXY(int x, int y) {
#ifdef _WIN32
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

void UI::DrawHeader() {
    SetColor(Config::Colors::BRIGHT_CYAN);
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::string(5, ' ') << Config::APP_NAME << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    ResetColor();
}

void UI::DrawBorders() {
    SetColor(Config::Colors::BRIGHT_GREEN);
    
    // Draw message area border
    std::cout << std::string(80, '-') << std::endl;
    
    ResetColor();
}

void UI::DrawFooter() {
    SetColor(Config::Colors::BRIGHT_CYAN);
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "Status: ";
    SetColor(Config::Colors::BRIGHT_YELLOW);
    std::cout << currentStatus << std::endl;
    SetColor(Config::Colors::BRIGHT_CYAN);
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "[Type message and press ENTER] [/help] [/exit]" << std::endl;
    ResetColor();
}

void UI::Draw() {
    ClearScreen();
    DrawHeader();
    DrawBorders();
    
    // Draw message history
    SetColor(Config::Colors::BRIGHT_GREEN);
    for (const auto& msg : messageHistory) {
        std::cout << msg << std::endl;
    }
    ResetColor();
    
    DrawFooter();
}

void UI::PrintMessage(const std::string& sender, const std::string& message, bool isOwn) {
    std::string formattedMsg;
    
    if (isOwn) {
        formattedMsg = "[";
        formattedMsg += sender;
        formattedMsg += "] > ";
        formattedMsg += message;
    } else {
        formattedMsg = "[";
        formattedMsg += sender;
        formattedMsg += "] < ";
        formattedMsg += message;
    }
    
    messageHistory.push_back(formattedMsg);
    
    // Keep only last 15 messages
    if (messageHistory.size() > 15) {
        messageHistory.erase(messageHistory.begin());
    }
    
    Draw();
}

void UI::SetStatus(const std::string& status) {
    currentStatus = status;
    Draw();
}

std::string UI::GetInput() {
    SetColor(Config::Colors::BRIGHT_YELLOW);
    std::cout << ">> ";
    ResetColor();
    
    std::string input;
    std::getline(std::cin, input);
    
    return input;
}

int UI::ShowConnectionMenu() {
    ClearScreen();
    SetColor(Config::Colors::BRIGHT_CYAN);
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::string(25, ' ') << Config::APP_NAME << std::endl;
    std::cout << std::string(80, '=') << std::endl << std::endl;
    
    SetColor(Config::Colors::BRIGHT_GREEN);
    std::cout << "  [1] Start as SERVER (Listen for connections)" << std::endl;
    std::cout << "  [2] Connect as CLIENT" << std::endl;
    std::cout << "  [3] Exit" << std::endl << std::endl;
    
    SetColor(Config::Colors::BRIGHT_YELLOW);
    std::cout << "Choose option [1-3]: ";
    ResetColor();
    
    std::string choice;
    std::getline(std::cin, choice);
    
    if (choice == "1") return 1;
    if (choice == "2") return 2;
    return 3;
}

void UI::ShowHelp() {
    ClearScreen();
    SetColor(Config::Colors::BRIGHT_CYAN);
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::string(30, ' ') << "HELP" << std::endl;
    std::cout << std::string(80, '=') << std::endl << std::endl;
    
    SetColor(Config::Colors::BRIGHT_GREEN);
    std::cout << "Commands:" << std::endl;
    std::cout << "  /help       - Display this help message" << std::endl;
    std::cout << "  /status     - Show connection status" << std::endl;
    std::cout << "  /clear      - Clear message history" << std::endl;
    std::cout << "  /exit       - Exit the application" << std::endl << std::endl;
    
    std::cout << "Features:" << std::endl;
    std::cout << "  * End-to-end encryption" << std::endl;
    std::cout << "  * Real-time messaging" << std::endl;
    std::cout << "  * MS-DOS retro style interface" << std::endl << std::endl;
    
    SetColor(Config::Colors::BRIGHT_YELLOW);
    std::cout << "Press ENTER to continue...";
    ResetColor();
    
    std::string temp;
    std::getline(std::cin, temp);
}
