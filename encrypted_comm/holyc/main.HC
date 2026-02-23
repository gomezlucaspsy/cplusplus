/**
 * HolyC Version - TempleOS Encrypted Communications
 * Main Application for TempleOS
 *
 * Compile with: TempleOS C Compiler
 * Usage: Start TempleOS, compile and run this file
 */

#include "encryption.HC"
#include "network.HC"
#include "ui.HC"

BOOL app_running = TRUE;
U8 *user_name = NULL;
U8 *local_ip = "127.0.0.1";

/**
 * Callback for received messages
 */
VOID OnMessageReceived(U8 *message) {
    if (message) {
        UIPrintMessage("Remote", message, FALSE);
        Free(message);
    }
}

/**
 * Main application entry point
 */
VOID EncryptedCommMain() {
    UIPrintColored("\n", WHITE);
    UIPrintColored("Encrypted Communications - TempleOS Edition\n", CYAN);
    UIPrintColored("Loading...\n\n", WHITE);

    // Get username
    UIInitialize();
    UIPrintColored("Enter your username: ", YELLOW);
    user_name = MAlloc(64);
    GetStr(user_name, 64);

    // Get encryption password
    UIPrintColored("Enter encryption password: ", YELLOW);
    U8 *password = MAlloc(128);
    GetStr(password, 128);

    // Initialize encryption
    if (!EncryptionInitialize(password)) {
        UIPrintColored("Failed to initialize encryption!\n", RED);
        Free(password);
        Free(user_name);
        return;
    }
    Free(password);

    // Initialize network
    if (!NetworkInitialize()) {
        UIPrintColored("Failed to initialize network!\n", RED);
        EncryptionCleanup();
        Free(user_name);
        return;
    }

    // Show connection menu
    I64 choice = UIShowMenu();

    IF (choice == 1) {
        // Server mode
        UIInitialize();
        UIPrintColored("Starting server...\n", CYAN);

        if (!NetworkStartServer(DEFAULT_PORT)) {
            UIPrintColored("Failed to start server!\n", RED);
            NetworkShutdown();
            EncryptionCleanup();
            Free(user_name);
            return;
        }

        UISetStatus("Server running on 127.0.0.1:7777 - Waiting for connection...");
        UIPrintMessage(user_name, "Server started. Waiting for client connection...", TRUE);

    } ELSE IF (choice == 2) {
        // Client mode
        UIInitialize();
        UIPrintColored("\nEnter server IP: ", YELLOW);
        U8 *server_ip = MAlloc(32);
        GetStr(server_ip, 32);

        UIPrintColored("Enter server port (default 7777): ", YELLOW);
        U8 *port_str = MAlloc(10);
        GetStr(port_str, 10);
        I64 port = DEFAULT_PORT;
        if (StrLen(port_str) > 0) {
            port = Str2I64(port_str);
        }
        Free(port_str);

        if (!NetworkConnect(server_ip, port)) {
            UIPrintColored("Failed to connect to server!\n", RED);
            Free(server_ip);
            NetworkShutdown();
            EncryptionCleanup();
            Free(user_name);
            return;
        }

        UISetStatus("Connected to server");
        UIPrintMessage(user_name, "Connected to server", TRUE);
        Free(server_ip);

    } ELSE {
        // Exit
        NetworkShutdown();
        EncryptionCleanup();
        UICleanup();
        Free(user_name);
        return;
    }

    // Main message loop
    UIDraw();

    WHILE (app_running) {
        U8 *input = UIGetInput();
        if (!input) continue;

        IF (StrLen(input) == 0) {
            Free(input);
            continue;
        }

        IF (!StrCmp(input, "/exit")) {
            UISetStatus("Exiting...");
            app_running = FALSE;
            Free(input);
            break;

        } ELSE IF (!StrCmp(input, "/help")) {
            UIShowHelp();
            UIDraw();
            Free(input);

        } ELSE IF (!StrCmp(input, "/status")) {
            U8 *status;
            IF (NetworkIsConnected()) {
                status = "Client Connected";
            } ELSE IF (NetworkIsServerRunning()) {
                status = "Server Running - Waiting for connection";
            } ELSE {
                status = "Disconnected";
            }
            UISetStatus(status);
            Free(input);

        } ELSE IF (!StrCmp(input, "/clear")) {
            UICleanup();
            UIInitialize();
            UIDraw();
            Free(input);

        } ELSE IF (input[0] == '/') {
            U8 error_msg[256];
            SPrintF(error_msg, "Unknown command: %s", input);
            UIPrintMessage(user_name, error_msg, TRUE);
            Free(input);

        } ELSE {
            // Send message
            IF (!NetworkSendMessage(input)) {
                U8 error_msg[256];
                SPrintF(error_msg, "[FAILED TO SEND] %s", input);
                UIPrintMessage(user_name, error_msg, TRUE);
            } ELSE {
                UIPrintMessage(user_name, input, TRUE);
            }
            Free(input);
        }
    }

    // Cleanup
    NetworkShutdown();
    EncryptionCleanup();
    UICleanup();
    Free(user_name);

    UIPrintColored("\nThank you for using Encrypted Communications!\n", GREEN);
}

// TempleOS entry point
EncryptedCommMain();
