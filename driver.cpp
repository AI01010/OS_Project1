// *****************************************************************
// Ashraful Islam axi220026
// OS encryption logger
// Project 1
// *****************************************************************

// driver.cpp
// main driver file

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <unistd.h>          /* UNIX and POSIX constants and functions (fork, pipe) */
#include <sys/types.h>
// #ifdef __unix__
#include <sys/wait.h>
// #else
// #error "This program requires a POSIX-compliant system."
// #endif

using namespace std;
    
// Helper Functions --------------------------------------------------------------------------------------------------

// Function to check if a string contains only letters and spaces
bool isValidInput(const string& str) 
{
    return all_of(str.begin(), str.end(), [](char c) {return isalpha(c) || isspace(c);} );
}

// Function to send a command to the encryption program and read the response
string sendCommand(int writefd, int readfd, const string& command) 
{
    // Send the command
    write(writefd, command.c_str(), command.length());
    write(writefd, "\n", 1);
    
    // Read the response
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = read(readfd, buffer, sizeof(buffer) - 1);
    
    if (bytesRead > 0) 
    {
        // Remove trailing newline character if present
        string response(buffer);
        if (!response.empty() && response.back() == '\n') 
        {
            response.pop_back();
        }
        return response;
    }
    
    return "ERROR No response from encryption program";
}

// Function to log a message
void logMessage(int logWritefd, const string& action, const string& message) 
{
    string logEntry = action + " " + message;
    write(logWritefd, logEntry.c_str(), logEntry.length());
    write(logWritefd, "\n", 1);
}

    
// Main driver --------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
    // Check command line arguments
    if (argc != 2) 
    {
        cerr << "Usage: " << argv[0] << " [logfile]" << endl;
        return 1;
    }
    
    // Create pipes for encryption program
    int encryptIn[2];  // Pipe for writing to encryption program
    int encryptOut[2]; // Pipe for reading from encryption program
    
    if (pipe(encryptIn) < 0 || pipe(encryptOut) < 0) 
    {
        cerr << "Encrytion pipes creation failed" << endl;
        return 1;
    }
    
    // Create pipes for logger
    int loggerIn[2];  // Pipe for writing to logger
    
    if (pipe(loggerIn) < 0) 
    {
        cerr << "Logger pipe creation failed" << endl;
        return 1;
    }
    
    // Fork for encryption program
    pid_t encryptPid = fork();
    
    if (encryptPid < 0) 
    {
        cerr << "Fork failed for encryption program" << endl;
        return 1;
    } 
    else if (encryptPid == 0) 
    {
        // Child process for encryption program
        close(encryptIn[1]);   // Close write end of input pipe
        close(encryptOut[0]);  // Close read end of output pipe
        
        // Redirect stdin to read from pipe
        dup2(encryptIn[0], STDIN_FILENO);
        close(encryptIn[0]);
        
        // Redirect stdout to write to pipe
        dup2(encryptOut[1], STDOUT_FILENO);
        close(encryptOut[1]);
        
        // Execute encryption program
        execlp("./encryption", "encryption", NULL);
        
        // If we get here, exec failed
        cerr << "Failed to execute encryption program" << endl;
        return 1;
    }
    
    // Fork for logger program
    pid_t loggerPid = fork();
    
    if (loggerPid < 0) 
    {
        cerr << "Fork failed for logger program" << endl;
        return 1;
    } 
    else if (loggerPid == 0) 
    {
        // Child process for logger program
        close(loggerIn[1]);  // Close write end of logger pipe
        
        // Redirect stdin to read from pipe
        dup2(loggerIn[0], STDIN_FILENO);
        close(loggerIn[0]);
        
        // Execute logger program
        execlp("./logger", "logger", argv[1], NULL);
        
        // If we get here, exec failed
        cerr << "Failed to execute logger program" << endl;
        return 1;
    }
    
    // Parent process
    // Close unused pipe ends
    close(encryptIn[0]);   // Close read end of encrypt input pipe
    close(encryptOut[1]);  // Close write end of encrypt output pipe
    close(loggerIn[0]);    // Close read end of logger pipe
    
    // Store temporary local history of strings (inputed passwords encrypted & decrypted)
    vector<string> history;
    
    // Log start of program
    logMessage(loggerIn[1], "START", "Logging Started.");
    
    // Main Menu loop --------------------------------------------------------------------------------------------------
    bool running = true;
    while (running) 
    {
        // Display menu
        cout << "\n--- Encryption System Menu ---" << endl;
        cout << "1. [password] - Set encryption password" << endl;
        cout << "2. [encrypt] - Encrypt a string" << endl;
        cout << "3. [decrypt] - Decrypt a string" << endl;
        cout << "4. [history] - Show history" << endl;
        cout << "5. [quit] - Exit the program" << endl;
        cout << "Enter a command [__?]: ";
        
        string command;
        getline(cin, command);
        
        // Convert command to lowercase for case-insensitivity
        transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        if (command == "password") 
        {
            logMessage(loggerIn[1], "PASS", "entered password.");
            
            string password;
            if (!history.empty()) 
            {
                // Offer to use history
                cout << "Use a string from history? (y/n): ";
                string choice;
                getline(cin, choice);
                
                if (choice == "y" || choice == "Y") 
                {
                    // Display history
                    cout << "\n--- History ---" << endl;
                    cout << "0. Enter new string" << endl;
                    for (size_t i = 0; i < history.size(); ++i) 
                    {
                        cout << i + 1 << ". " << history[i] << endl;
                    }
                    cout << "Select a number: ";
                    
                    string selection;
                    getline(cin, selection);
                    
                    try 
                    {
                        int index = stoi(selection);
                        if (index > 0 && index <= static_cast<int>(history.size())) 
                        {
                            password = history[index - 1];
                        } 
                        else if (index != 0) 
                        {
                            cout << "Invalid selection. Using new string." << endl;
                        }
                    } 
                    catch (...) 
                    {
                        cout << "Invalid selection. Using new string." << endl;
                    }
                }
            }
            
            if (password.empty()) 
            {
                cout << "Enter password: ";
                getline(cin, password);
                
                // Check password (only letters allowed)
                if (!isValidInput(password)) 
                {
                    cout << "Error: Password must contain only letters and spaces." << endl;
                    logMessage(loggerIn[1], "ERROR", "Invalid password format.");
                    continue;
                }
            }
            
            // Send password to encryption program
            string response = sendCommand(encryptIn[1], encryptOut[0], "PASSKEY " + password);
            cout << "Encryption program response: " << response << endl;
            
            // Log the action but not the actual password
            logMessage(loggerIn[1], "PASS", "Password set.");
            
        } 
        else if (command == "encrypt") 
        {
            logMessage(loggerIn[1], "ENCRYPT", "encrypting string.");
            
            string inputString;
            if (!history.empty()) 
            {
                // Offer to use history
                cout << "Use a string from history? (y/n): ";
                string choice;
                getline(cin, choice);
                
                if (choice == "y" || choice == "Y") 
                {
                    // Display history
                    cout << "\n--- History ---" << endl;
                    cout << "0. Enter new string" << endl;
                    for (size_t i = 0; i < history.size(); ++i) 
                    {
                        cout << i + 1 << ". " << history[i] << endl;
                    }
                    cout << "Select number: ";
                    
                    string selection;
                    getline(cin, selection);
                    
                    try 
                    {
                        int index = stoi(selection);
                        if (index > 0 && index <= static_cast<int>(history.size())) 
                        {
                            inputString = history[index - 1];
                        } 
                        else if (index != 0) 
                        {
                            cout << "Invalid selection. Using new string." << endl;
                        }
                    } 
                    catch (...) 
                    {
                        cout << "Invalid selection. Using new string." << endl;
                    }
                }
            }
            
            if (inputString.empty()) 
            {
                cout << "Enter string to encrypt: ";
                getline(cin, inputString);
                
                // Validate input (only letters allowed)
                if (!isValidInput(inputString)) 
                {
                    cout << "Error: Input must contain only letters and spaces." << endl;
                    logMessage(loggerIn[1], "ERROR", "Invalid input format for encryption.");
                    continue;
                }
                
                // Add to history
                history.push_back(inputString);
            }
            
            // Send to encryption program
            string response = sendCommand(encryptIn[1], encryptOut[0], "ENCRYPT " + inputString);
            cout << "Result: " << response << endl;
            
            // Extract result if successful
            if (response.substr(0, 6) == "RESULT") 
            {
                string encryptedText = response.substr(7);  // Skip "RESULT "
                //dont: history.push_back(encryptedText);  // Add encrypted result to history
                logMessage(loggerIn[1], "RESULT", "Encrypted: " + inputString + " -> " + encryptedText+".");
            } 
            else 
            {
                logMessage(loggerIn[1], "ERROR", response.substr(6));  // Skip "ERROR "
            }
            
        } 
        else if (command == "decrypt") 
        {
            logMessage(loggerIn[1], "DECRYPT", "decrypting string.");
            
            string inputString;
            if (!history.empty()) 
            {
                // Offer to use history
                cout << "Use a string from history? (y/n): ";
                string choice;
                getline(cin, choice);
                
                if (choice == "y" || choice == "Y") 
                {
                    // Display history
                    cout << "\n--- History ---" << endl;
                    cout << "0. Enter new string" << endl;
                    for (size_t i = 0; i < history.size(); ++i) 
                    {
                        cout << i + 1 << ". " << history[i] << endl;
                    }
                    cout << "Select number: ";
                    
                    string selection;
                    getline(cin, selection);
                    
                    try 
                    {
                        int index = stoi(selection);
                        if (index > 0 && index <= static_cast<int>(history.size())) 
                        {
                            inputString = history[index - 1];
                        } 
                        else if (index != 0) 
                        {
                            cout << "Invalid selection. Using new string." << endl;
                        }
                    } 
                    catch (...) 
                    {
                        cout << "Invalid selection. Using new string." << endl;
                    }
                }
            }
            
            if (inputString.empty()) 
            {
                cout << "Enter string to decrypt: ";
                getline(cin, inputString);
                
                // Validate input (only letters allowed)
                if (!isValidInput(inputString)) 
                {
                    cout << "Error: Input must contain only letters and spaces." << endl;
                    logMessage(loggerIn[1], "ERROR", "Invalid input format for decryption.");
                    continue;
                }
                
                // Add to history
                history.push_back(inputString);
            }
            
            // Send to encryption program
            string response = sendCommand(encryptIn[1], encryptOut[0], "DECRYPT " + inputString);
            cout << "Result: " << response << endl;
            
            // Extract result if successful
            if (response.substr(0, 6) == "RESULT") 
            {
                string decryptedText = response.substr(7);  // Skip "RESULT "
                //dont: history.push_back(decryptedText);  // Add decrypted result to history
                logMessage(loggerIn[1], "RESULT", "Decrypted: " + inputString + " -> " + decryptedText+".");
            } 
            else 
            {
                logMessage(loggerIn[1], "ERROR", response.substr(6));  // Skip "ERROR "
            }
            
        } 
        else if (command == "history") 
        {
            logMessage(loggerIn[1], "HISTORY", "history requested.");
            
            if (history.empty()) 
            {
                cout << "History is empty." << endl;
                logMessage(loggerIn[1], "RESULT", "History is empty.");
            } 
            else 
            {
                cout << "\n--- History ---" << endl;
                for (size_t i = 0; i < history.size(); ++i) 
                {
                    cout << i + 1 << ". " << history[i] << endl;
                }
                logMessage(loggerIn[1], "RESULT", "History displayed.");
            }
            
        } 
        else if (command == "quit") 
        {
            logMessage(loggerIn[1], "QUIT", "quit command received.");
            
            // Send QUIT to encryption program and logger
            write(encryptIn[1], "QUIT\n", 5);
            write(loggerIn[1], "QUIT\n", 5);
            
            // Log end of program
            logMessage(loggerIn[1], "END", "Driver program ended. ");
            
            running = false;
            
        } 
        else 
        {
            cout << "Unknown command. Please try again." << endl;
            logMessage(loggerIn[1], "ERROR", "Unknown command: " + command);
        }
    }
    
    // Close all open pipe ends
    close(encryptIn[1]);
    close(encryptOut[0]);
    close(loggerIn[1]);
    
    // Wait for child processes to terminate
    waitpid(encryptPid, NULL, 0);
    waitpid(loggerPid, NULL, 0);
    
    cout << "Program terminated." << endl;
    
    return 0;
}

//driver.cpp
