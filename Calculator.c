#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

// Global handles for controls
HWND hTextInput;
HWND hButton;
HWND hOutputText;

// Parser state
const char* expr;
int pos;
int parseError;

// Forward declarations
double parseExpression();

double parseNumber() {
    double num = 0;
    if (!isdigit(expr[pos])) {
        parseError = 1;
        return 0;
    }
    while (isdigit(expr[pos])) {
        num = num * 10 + (expr[pos] - '0');
        pos++;
    }
    // Handle decimal point
    if (expr[pos] == '.') {
        pos++;
        double decimal = 0.1;
        while (isdigit(expr[pos])) {
            num += (expr[pos] - '0') * decimal;
            decimal *= 0.1;
            pos++;
        }
    }
    return num;
}

double parseFactor() {
    if (expr[pos] == '(') {
        pos++;  // Skip '('
        double result = parseExpression();
        if (expr[pos] == ')') {
            pos++;  // Skip ')'
        } else {
            parseError = 1;
        }
        return result;
    }
    return parseNumber();
}

double parseTerm() {
    double result = parseFactor();
    
    while (expr[pos] == '*' || expr[pos] == '/' || expr[pos] == '^') {
        char op = expr[pos];
        pos++;
        double right = parseFactor();
        
        if (op == '*') {
            result = result * right;
        } else if (op == '/') {
            if (right == 0) {
                parseError = 1;
                return 0;
            }
            result = result / right;
        } else {
            result = pow(result, right);
        }
    }
    return result;
}

double parseExpression() {
    double result = parseTerm();
    
    while (expr[pos] == '+' || expr[pos] == '-') {
        char op = expr[pos];
        pos++;
        double right = parseTerm();
        
        if (op == '+') {
            result = result + right;
        } else {
            result = result - right;
        }
    }
    return result;
}

double evaluateExpression(const char* expression) {
    expr = expression;
    pos = 0;
    parseError = 0;
    
    // Skip leading whitespace
    while (expr[pos] == ' ') pos++;
    
    double result = parseExpression();
    
    // Skip trailing whitespace
    while (expr[pos] == ' ') pos++;
    
    // Check if we consumed the entire expression
    if (expr[pos] != '\0') {
        parseError = 1;
    }
    
    return result;
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
                    
                    // Parse and evaluate the expression
                    double result = evaluateExpression(buffer);
                    
                    // Display result
                    char outputBuffer[512];
                    if (parseError) {
                        sprintf(outputBuffer, "Error: Invalid expression");
                    } else {
                        sprintf(outputBuffer, "%s = %.6g", buffer, result);
                    }
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
    
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Calculator", WS_OVERLAPPEDWINDOW,
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