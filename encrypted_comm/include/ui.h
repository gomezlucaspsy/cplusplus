#ifndef UI_H
#define UI_H

#include <string>
#include <vector>

class UI {
public:
    // Initialize the UI (clear screen, draw borders, etc.)
    static void Initialize();
    
    // Draw the complete UI
    static void Draw();
    
    // Print a message in the output area
    static void PrintMessage(const std::string& sender, const std::string& message, bool isOwn = false);
    
    // Get user input from the input line
    static std::string GetInput();
    
    // Set status message
    static void SetStatus(const std::string& status);
    
    // Clear the screen
    static void ClearScreen();
    
    // Draw the header
    static void DrawHeader();
    
    // Draw the borders
    static void DrawBorders();
    
    // Draw the footer
    static void DrawFooter();
    
    // Display connection menu
    static int ShowConnectionMenu();
    
    // Display help
    static void ShowHelp();
    
private:
    static std::vector<std::string> messageHistory;
    static std::string currentStatus;
    static int maxLines;
    static int maxCols;
    
    // Helper functions
    static void SetColor(const std::string& color);
    static void ResetColor();
    static void GotoXY(int x, int y);
};

#endif // UI_H
