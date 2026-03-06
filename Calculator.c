#include <windows.h>
#include <string.h>
#include <stdio.h>

// Global handles for controls
HWND hTextInput;
HWND hButton;
HWND hOutputText;
int operatorIdx = 0;

// Function to extract numbers from equation provided in the text input
int extractNumber(const char* str, size_t startIndex) {
    int number = 0;
    for (size_t i = startIndex; i < strlen(str); i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            number = number * 10 + (str[i] - '0');
        } else {
            break;  // Stop at first non-digit character
        }
    }
    return number;
}

int extractOperator(const char* str) {
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/') {
            operatorIdx = i;
            return str[i];
        }
    }
    return 0; // No operator found
}

int calculate(int num1, int num2, char op) {
    switch (op) {
        case '+':
            return num1 + num2;
        case '-':
            return num1 - num2;
        case '*':
            return num1 * num2;
        case '/':
            if (num2 != 0) {
                return num1 / num2;
            } else {
                MessageBoxA(NULL, "Division by zero is not allowed!", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }
        default:
            MessageBoxA(NULL, "Invalid operator!", "Error", MB_OK | MB_ICONERROR);
            return 0;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            {
                // Create text input label
                CreateWindowA("STATIC", "Enter equation:", WS_VISIBLE | WS_CHILD,
                              10, 10, 150, 20, hwnd, NULL, NULL, NULL);
                
                // Create text input field (edit control)
                hTextInput = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                           10, 35, 200, 25, hwnd, NULL, NULL, NULL);
                
                // Create button
                hButton = CreateWindowA("BUTTON", "Calculate", WS_VISIBLE | WS_CHILD,
                                        220, 35, 100, 25, hwnd, (HMENU)1, NULL, NULL);
                
                // Create output text display
                hOutputText = CreateWindowA("STATIC", "Output will appear here", 
                                            WS_VISIBLE | WS_CHILD | WS_BORDER,
                                            10, 70, 310, 100, hwnd, NULL, NULL, NULL);
            }
            return 0;
        
        case WM_COMMAND:
            {
                int controlId = LOWORD(wParam);
                // Check if the button was clicked
                if (controlId == 1) {
                    char buffer[256];
                    // Get text from input field
                    GetWindowTextA(hTextInput, buffer, sizeof(buffer));
                    
                    // Extract number from the input text
                    int number1 = extractNumber(buffer, 0);
                    int op = extractOperator(buffer);
                    int number2 = extractNumber(strchr(buffer, op) + 1, 0);

                    int result = calculate(number1, number2, op);

                    // Display it in output

                    char outputBuffer[256];
                    sprintf(outputBuffer, buffer);
                    strcat(outputBuffer, " = ");
                    char answerBuffer[256];
                    sprintf(answerBuffer, "%d", result);
                    strcat(outputBuffer, answerBuffer);
                    SetWindowTextA(hOutputText, outputBuffer);
                }
            }
            return 0;
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                
                // Fill background with white
                FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

                EndPaint(hwnd, &ps);
            }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    const char CLASS_NAME[] = "Sample Window Class";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Just a Calculator", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
                               NULL, NULL, hInstance, NULL);
    
    ShowWindow(hwnd, nShowCmd);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}