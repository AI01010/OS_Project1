// logger.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstring>

int main(int argc, char* argv[]) 
{
    // Check command line arguments
    if (argc != 2) 
    {
        std::cerr << "Usage: " << argv[0] << " [logfile]" << std::endl;
        return 1;
    }

    // Open log file in append mode
    std::ofstream logFile(argv[1], std::ios::app);
    if (!logFile) 
    {
        std::cerr << "Error: Could not open log file " << argv[1] << std::endl;
        return 1;
    }

    std::string input;
    
    while (std::getline(std::cin, input)) 
    {
        // Check for QUIT command
        if (input == "QUIT") 
        {
            break;
        }
        
        // Get current time
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        
        // Format time using strftime
        char timeBuffer[80];
        std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", localTime);
        
        // Find the first space to separate action and message
        size_t spacePos = input.find(' ');
        std::string action, message;
        
        if (spacePos != std::string::npos) 
        {
            action = input.substr(0, spacePos);
            message = input.substr(spacePos + 1);
        } 
        else 
        {
            action = input;
            message = "";
        }
        
        // Write log entry
        logFile << timeBuffer << " [" << action << "] " << message << std::endl;
        
        // Flush to ensure immediate writing
        logFile.flush();
    }
    
    logFile.close();
    return 0;
}