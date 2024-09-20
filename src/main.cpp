// Definitions
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define BUFFER_SIZE 40000

// Includes
#include <iostream>
#include <string>
#include <fstream>
#include <winsock2.h>
using namespace std;

int main() {
    cout << "HTTP Server using Win32 API\n\n";

    // Variables
    SOCKET wsocket;                 // Store socket at program initialization
    SOCKET new_wsocket;             // Used to accept data from client requests
    WSADATA wsaData;                // Store data from WSAStartup
    struct sockaddr_in server;      // Server Struct
    int serverSize;                 // Used to store size of server struct after binding
    int bytes;                      // Store No. of bytes from each request
    string serverMessage;           // Final message that will be passed back to the client
    string response;                // HTML that will be sent back to the client
    string responseFilePath;        // File path of HTML file to be served
    string line;                    // Hold each line read in
    int bytesSent;                  // Bytes sent to client on each send
    int totalBytesSent;             // Total amount of bytes sent per response

    // Allows the program to take advantage of the Winsock DLL
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        cout << "WSA Startup Failed.\n";
        exit(-1);
    }

    // Initialize a socket and error check
    wsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET means use IPv4, SOCK_STREAM means it is a stream socket, IPPROTO_TCP means use the TCP protocol  
    if (wsocket == INVALID_SOCKET) {
        cout << "Unable to make a socket.\n";
        exit(-2);
    }

    cout << "Socket Creation Successful.\n";

    // Bind newly made socket to an address + port
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");    // localhost
    server.sin_port = htons(8080);                      // Port
    serverSize = sizeof(server);

    if(bind(wsocket, (SOCKADDR * )&server, serverSize) == SOCKET_ERROR) {     // Error Check
        cout << "Unable to bind socket.\n";
        exit(-3);
    }

    cout << "Socket Binding Successful.\n";

    // Start listening to the address, error check
    if (listen(wsocket, 20)) {
        cout << "Failed to listen.\n";
        exit(-4);
    }

    cout << "Listening on 127.0.0.1:8080\n\n";

    // Build HTML response
    response = "";
    responseFilePath = "testHTML/index.html";    // Replace with own HTML files
    ifstream file(responseFilePath);

    if (!file.is_open()) {
        cout << "Failed to open file: " << responseFilePath << "\n" << endl;
        exit(-4);
    }

    while (getline(file, line)) {
        response.append(line);
    }

    file.close();

    // Continuely listen for incoming requests
    bytes = 0;
    while (1) {
        // Accept Request
        new_wsocket = accept(wsocket, (SOCKADDR*)&server, &serverSize);
        if(new_wsocket == INVALID_SOCKET) {
            cout << "Failed to accept.\n";
        }

        // Read request
        char buf[BUFFER_SIZE] = {0};
        bytes = recv(new_wsocket, buf, BUFFER_SIZE, 0);
        if (bytes < 0) { // Check if any data was received
            cout << "Unable to read client request.\n";
        }
        
        // Message formatting
        serverMessage = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        serverMessage.append(to_string(response.size()));
        serverMessage.append("\n\n");
        serverMessage.append(response);

        // Send response to client
        bytesSent = 0;
        totalBytesSent = 0;
        while (totalBytesSent < serverMessage.size()) {
            bytesSent = send(new_wsocket, serverMessage.c_str(), serverMessage.size(), 0); // Need to turn into C style string as winsock2 is a C header file
            if (bytesSent < 0) {
                cout << "Could not send a response.\n";
            }

            cout << "Sent ";
            cout << to_string(bytesSent);
            cout << " Bytes.\n";

            totalBytesSent += bytesSent;
        }

        cout << "Message Sent.\n";

        // Close new_wsocket for reuse next iter
        closesocket(new_wsocket);
    }

    // Cleanup
    closesocket(wsocket);
    WSACleanup();

    return 0;
}