#include "pch.h"
#include "Server.h"

namespace http::server {
    static httplib::Server server;

    static int FindPort()
    {
        // Initialize Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cout << "Failed to initialize Winsock." << std::endl;
            return -1;
        }

        // Create a socket
        const SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            std::cout << "Failed to create socket." << std::endl;
            return -1;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        // Start from a specific port number and find an available port
        int port = 42091;
        while (true) {
            serverAddr.sin_port = htons(port);

            if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
                std::cout << "Successfully bound to port " << port << std::endl;
                break;
            }
            port++; // Try the next port

            if (port > 65534) {
                std::cout << "Failed to find a port. Cannot initialize server" << std::endl;
                return -1;
            }
        }

        // Clean up
        closesocket(serverSocket);
        WSACleanup();

        return port;
    }

    void Get(const std::string& pattern, httplib::Server::Handler handler)
    {
        server.Get(pattern, handler);
    }

    void SetLogger(httplib::Logger logger)
    {
        server.set_logger(logger);
    }

    bool StartServer()
    {
        const int port = FindPort();
        if (port > 0) {
            std::cout << "Initializing server on port " << port << std::endl;
            server.listen("0.0.0.0", port);
            return true;
        }

        return false;
    }

    void StopServer()
    {
        server.stop();
    }
}
