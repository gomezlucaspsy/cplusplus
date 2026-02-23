/**
 * HolyC Version - TempleOS Encrypted Communications
 * User Interface Module for TempleOS
 */

#define MAX_MESSAGE_HISTORY 15
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24

CDblList message_history;
U8 *current_status = "Ready";

/**
 * Initialize UI
 */
VOID UIInitialize() {
    // Clear screen and initialize message list
    MemSet(&message_history, 0, sizeof(CDblList));
    current_status = "Ready";
}

/**
 * Print text with color (TempleOS graphics)
 */
VOID UIPrintColored(U8 *text, COLORREF color) {
    if (!text) return;
    // TempleOS graphics command
    // This would use GrPrint or similar
}

/**
 * Draw header
 */
VOID UIDrawHeader() {
    UIPrintColored("════════════════════════════════════════════════════════════════════════════════\n", WHITE);
    UIPrintColored("               ENCRYPTED COMM v1.0 - TempleOS Edition\n", CYAN);
    UIPrintColored("════════════════════════════════════════════════════════════════════════════════\n\n", WHITE);
}

/**
 * Draw footer with status
 */
VOID UIDrawFooter() {
    UIPrintColored("════════════════════════════════════════════════════════════════════════════════\n", WHITE);
    UIPrintColored("Status: ", CYAN);
    UIPrintColored(current_status, YELLOW);
    UIPrintColored("\n", WHITE);
    UIPrintColored("════════════════════════════════════════════════════════════════════════════════\n", WHITE);
    UIPrintColored("[Type message and press ENTER]  [/help]  [/exit]\n", WHITE);
}

/**
 * Redraw entire UI
 */
VOID UIDraw() {
    // Clear screen (TempleOS way)
    '\f';  // Form feed clears screen in TempleOS
    
    UIDrawHeader();
    
    // Display message history
    CNode *n = message_history.next;
    WHILE (n != &message_history) {
        U8 *msg = n->data;
        UIPrintColored(msg, GREEN);
        UIPrintColored("\n", WHITE);
        n = n->next;
    }
    
    UIDrawFooter();
}

/**
 * Print message with sender info
 */
VOID UIPrintMessage(U8 *sender, U8 *message, BOOL is_own) {
    if (!sender || !message) return;

    U8 *formatted = MAlloc(512);
    if (!formatted) return;

    // Format message
    if (is_own) {
        SPrintF(formatted, "[%s] > %s", sender, message);
    } else {
        SPrintF(formatted, "[%s] < %s", sender, message);
    }

    // Add to history
    CNode *node = CAlloc(sizeof(CNode));
    node->data = formatted;
    InsQue(node, message_history.last);

    // Keep only last 15 messages
    I64 count = 0;
    CNode *n = message_history.next;
    WHILE (n != &message_history) {
        count++;
        n = n->next;
    }

    WHILE (count > MAX_MESSAGE_HISTORY) {
        n = message_history.next;
        RemQue(n);
        Free(n->data);
        Free(n);
        count--;
    }

    UIDraw();
}

/**
 * Set status message
 */
VOID UISetStatus(U8 *status) {
    if (!status) return;
    current_status = status;
    UIDraw();
}

/**
 * Get user input line
 */
U8 *UIGetInput() {
    U8 *input = MAlloc(256);
    if (!input) return NULL;

    UIPrintColored(">> ", YELLOW);
    
    // Read input from keyboard (TempleOS)
    // In TempleOS, this uses GetStr or similar functions
    GetStr(input, 256);
    
    return input;
}

/**
 * Show main menu
 */
I64 UIShowMenu() {
    '\f';  // Clear screen
    
    UIDrawHeader();
    
    UIPrintColored("\n  [1] Start as SERVER (Listen for connections)\n", GREEN);
    UIPrintColored("  [2] Connect as CLIENT\n", GREEN);
    UIPrintColored("  [3] Exit\n\n", GREEN);
    
    UIPrintColored("Choose option [1-3]: ", YELLOW);
    
    U8 choice[2];
    GetStr(choice, 2);
    
    return Str2I64(choice);
}

/**
 * Show help message
 */
VOID UIShowHelp() {
    '\f';  // Clear screen
    
    UIPrintColored("════════════════════════════════════════════════════════════════════════════════\n", CYAN);
    UIPrintColored("                              HELP\n", CYAN);
    UIPrintColored("════════════════════════════════════════════════════════════════════════════════\n\n", CYAN);
    
    UIPrintColored("Commands:\n", GREEN);
    UIPrintColored("  /help       - Display this help message\n", GREEN);
    UIPrintColored("  /status     - Show connection status\n", GREEN);
    UIPrintColored("  /clear      - Clear message history\n", GREEN);
    UIPrintColored("  /exit       - Exit the application\n\n", GREEN);
    
    UIPrintColored("Features:\n", GREEN);
    UIPrintColored("  * End-to-end encryption\n", GREEN);
    UIPrintColored("  * Real-time messaging\n", GREEN);
    UIPrintColored("  * TempleOS native implementation\n\n", GREEN);
    
    UIPrintColored("Press ENTER to continue...", YELLOW);
    U8 temp[2];
    GetStr(temp, 2);
}

/**
 * Cleanup UI resources
 */
VOID UICleanup() {
    CNode *n = message_history.next;
    WHILE (n != &message_history) {
        CNode *next = n->next;
        Free(n->data);
        Free(n);
        n = next;
    }
}
