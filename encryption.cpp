// encryption.cpp
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

// Function to apply Vigenère cipher (works for encryption and decryption)
string vigenere(const string& text, const string& key, bool encrypt) 
{
    if (key.empty()) 
    {
        return "";
    }
    
    string result;
    size_t keyIndex = 0;
    
    for (char c : text) 
    {
        if (isalpha(c)) 
        {
            // Convert all char to uppercase for consistency
            char base = isupper(c) ? 'A' : 'a';
            char upperC = toupper(c) - 'A';
            char upperKey = toupper(key[keyIndex % key.length()]) - 'A';
            
            char shift;
            if (encrypt) 
            {
                // encryption: (plaintext + key) % 26 --> roll over with mod 26
                shift = (upperC + upperKey) % 26;
            } 
            else 
            {
                // decryption: (ciphertext - key + 26) % 26  <-- reverse the shift
                shift = (upperC - upperKey + 26) % 26;
            }
            
            // Preserve the original case
            result += (isupper(c) ? 'A' + shift : 'a' + shift);
            keyIndex++;
        } 
        else 
        {
            // Non-alphabetic characters remain unchanged
            result += c;
        }
    }
    
    return result;
}

int main() 
{
    string passkey;
    string line;
    
    while (getline(cin, line)) // Read input from stdin piped from driver
    {
        // Find the first space to separate command and argument
        size_t spacePos = line.find(' ');
        // split input into command and argument
        string command = (spacePos != string::npos) ? line.substr(0, spacePos) : line;
        string argument = (spacePos != string::npos) ? line.substr(spacePos + 1) : "";
        
        // Convert command to uppercase for case-insensitive comparison
        transform(command.begin(), command.end(), command.begin(), ::toupper);
        
        if (command == "QUIT") 
        {
            break;
        } 
        else if (command == "PASS" || command == "PASSKEY") 
        {
            passkey = argument; // Set the passkey
            cout << "RESULT" << endl;
        } 
        else if (command == "ENCRYPT") 
        {
            if (passkey.empty()) 
            {
                cout << "ERROR Password not set" << endl;
            } 
            else 
            {
                // Check if input contains only letters
                bool validInput = true;
                for (char c : argument) 
                {
                    if (!isalpha(c) && !isspace(c)) 
                    {
                        validInput = false;
                        break;
                    }
                }
                
                if (!validInput) 
                {
                    cout << "ERROR Input must contain only letters" << endl;
                } 
                else 
                {
                    string result = vigenere(argument, passkey, true); // Encrypting
                    cout << "RESULT " << result << endl;
                }
            }
        } 
        else if (command == "DECRYPT") 
        {
            if (passkey.empty()) 
            {
                cout << "ERROR Password not set" << endl;
            } 
            else 
            {
                // Check if input contains only letters
                bool validInput = true;
                for (char c : argument) 
                {
                    if (!isalpha(c) && !isspace(c)) 
                    {
                        validInput = false;
                        break;
                    }
                }
                
                if (!validInput) 
                {
                    cout << "ERROR Input must contain only letters" << endl;
                } 
                else 
                {
                    string result = vigenere(argument, passkey, false); // Decrypting
                    cout << "RESULT " << result << endl;
                }
            }
        } 
        else 
        {
            cout << "ERROR Unknown command" << endl;
        }
    }
    
    return 0;
}