#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <string>

int main()
{
    std::cout << "Attempting to create a server\n";

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    const int BUFFER_SIZE = 30720;

    // create socket
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket\n";
        WSACleanup();
        return 1;
    }

    // bind socket to address
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8080);
    if (bind(listen_socket, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket to address\n";
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    // listen to address
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen to address\n";
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Listening on 127.0.0.1:8080\n";

    while (true) {
        // accept client request
        sockaddr_in client_address;
        int client_address_size = sizeof(client_address);
        SOCKET client_socket = accept(listen_socket, reinterpret_cast<sockaddr*>(&client_address), &client_address_size);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Failed to accept client request\n";
            continue;
        }

        // read request data
        char buffer[BUFFER_SIZE] = { 0 };
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            std::cerr << "Failed to read client request\n";
            closesocket(client_socket);
            continue;
        }

        std::string response = "<html><h1>Hello world</h1></html>";
        std::string server_message = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-length: " + std::to_string(response.size()) + "\n\n" + response;

        // send response to client
        int bytes_sent = 0;
        int total_bytes_sent = 0;
        while (total_bytes_sent < server_message.size()) {
            bytes_sent = send(client_socket, server_message.c_str() + total_bytes_sent, server_message.size() - total_bytes_sent, 0);
            if (bytes_sent < 0) {
                std::cerr << "Failed to send response\n";
                closesocket(client_socket);
                break;
            }
            total_bytes_sent += bytes_sent;
        }
        std::cout << "Sent response to client\n";

        closesocket(client_socket);
    }

    closesocket(listen_socket);
    WSACleanup();

    return 0;
}