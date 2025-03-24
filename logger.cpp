// logger.cpp
// #include "logger.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>

using namespace std;

int main(int argc, char* argv[]) 
{
    // Check command line arguments
    if (argc != 2) 
    {
        cerr << "Usage: " << argv[0] << " [logfile]" << endl;
        return 1;
    }

    // Open log file
    ofstream logFile(argv[1], ios::app); //file arg[1] is the log file name
    if (!logFile) 
    {
        cerr << "Error: Could not open log file " << argv[1] << endl;
        return 1;
    }

    string input;
    
    while (getline(cin, input))  // Read input from stdin piped from driver
    {
        // Check for QUIT command
        if (input == "QUIT") {
            break;
        }
        
        // Find the first space to separate action and message
        size_t spacePosition = input.find(' ');
        if (spacePosition == string::npos) 
        {
            // If no space found, consider the whole input as the action with empty message
            spacePosition = input.length();
        }
        
        // split input into action and message
        string action = input.substr(0, spacePosition);
        string message = (spacePosition < input.length()) ? input.substr(spacePosition + 1) : ""; //check if empty message
        
        // Get current time
        time_t now = time(nullptr);
        tm* localTime = localtime(&now);
        
        // Format and write log entry
        logFile << put_time(localTime, "%Y-%m-%d %H:%M") 
                << " [" << action << "] " << message << endl;
        
    }
    
    logFile.close();
    return 0;
}