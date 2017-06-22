#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class tcpServer
{
private:
	WSADATA wsaData;

	ADDRINFO settings;
	ADDRINFO *result;

	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;

	std::string serverPort;
public:
	tcpServer(std::string port)
	{
		serverPort = port;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		ZeroMemory(&settings, sizeof(ADDRINFO));
		settings.ai_family = AF_INET;
		settings.ai_socktype = SOCK_STREAM;
		settings.ai_protocol = IPPROTO_TCP;
		settings.ai_flags = AI_PASSIVE;

		getaddrinfo(NULL, port.c_str(), &settings, &result);

		listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	}

	void startListening()
	{
		listen(listenSocket, SOMAXCONN);
	}

	void acceptConnection()
	{
		clientSocket = accept(listenSocket, NULL, NULL);
	}

	std::string receiveMessage()
	{
		const int bufferlength = 512;
		char buffer[bufferlength];
		recv(clientSocket, buffer, bufferlength, NULL);
		return std::string(buffer);
	}

	void sendMessage(std::string message)
	{
		send(clientSocket, message.c_str(), message.length() + 1, NULL);
	}

	void serverShutdown()
	{
		closesocket(clientSocket);
		closesocket(listenSocket);
		WSACleanup();
	}
};

int main()
{
	tcpServer server("27015");
	std::string messageBack;
	std::string messageReceived;

	server.startListening();

	server.acceptConnection();

	bool exit = false;
	while (exit != true)
	{
		messageReceived = server.receiveMessage();
		if (messageReceived == "disconnect")
		{
			break;
		}

		std::cout << "Client said: " << messageReceived << std::endl;

		std::cout << "Send back: ";
		std::getline(std::cin, messageBack);
		server.sendMessage(messageBack);
	}

	server.serverShutdown();
	std::cout << "Process terminated... Press enter to close" << std::endl;
	std::cin.ignore();
	return 0;
}