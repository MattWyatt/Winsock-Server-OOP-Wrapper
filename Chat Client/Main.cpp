#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class tcpClient
{
private:
	WSADATA wsaData;

	ADDRINFO settings;
	ADDRINFO *result;

	SOCKET clientSocket = INVALID_SOCKET;

	std::string serverPort;
	std::string serverIP;

public:
	tcpClient(std::string ip, std::string port)
	{
		serverIP = ip;
		serverPort = port;

		WSAStartup(MAKEWORD(2, 2), &wsaData);

		ZeroMemory(&settings, sizeof(ADDRINFO));
		settings.ai_family = AF_UNSPEC;
		settings.ai_socktype = SOCK_STREAM;
		settings.ai_protocol = IPPROTO_TCP;

		getaddrinfo(ip.c_str(), port.c_str(), &settings, &result);

		clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	}

	void serverConnect()
	{
		connect(clientSocket, result->ai_addr, result->ai_addrlen);
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

	void clientShutdown()
	{
		shutdown(clientSocket, NULL);
		closesocket(clientSocket);
		WSACleanup();
	}
};


std::string prompt(std::string message)
{
	std::string buffer;
	std::cout << message;
	std::getline(std::cin, buffer);
	return buffer;
}

void run_client()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	ADDRINFO hints;
	ADDRINFO *result;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	getaddrinfo(prompt("Server IP: ").c_str(), prompt("Server Port: ").c_str(), &hints, &result);

	SOCKET client = INVALID_SOCKET;
	client = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	connect(client, result->ai_addr, result->ai_addrlen);
	std::string message = "hello, i'm client.";
	send(client, message.c_str(), strlen(message.c_str()) + 1, 0);


	shutdown(client, SD_SEND /*SD_BOTH*/); // finish send

	const int buff_len = 512;
	char recv_buff[buff_len];
	recv(client, recv_buff, buff_len, 0);

	closesocket(client);
}

int main()
{
	std::string ip;
	std::string port;
	std::string message;
	std::string messageReceived;

	std::cout << "Server IP: ";
	std::getline(std::cin, ip);

	std::cout << "Server Port: ";
	std::getline(std::cin, port);

	tcpClient client(ip, port);
	client.serverConnect();

	while (true)
	{
		std::cout << "Send Message: ";
		std::getline(std::cin, message);
		client.sendMessage(message);

		messageReceived = client.receiveMessage();
		std::cout << "Server Responded: " << messageReceived << std::endl;
	}

	client.clientShutdown();

	std::cout << "Process terminated... Press enter to close" << std::endl;
	std::cin.ignore();
	return 0;
}