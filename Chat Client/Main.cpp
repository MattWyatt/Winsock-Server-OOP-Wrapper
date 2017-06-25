#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

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

	void receiveThread()
	{
		bool quit = false;
		std::string buffer;

		while (quit != true)
		{
			buffer = receiveMessage();
			std::cout << "Host: " <<  buffer << std::endl;

			if (buffer == "disconnect")
			{
				quit = true;
				break;
			}
		}
	}

	void senderThread()
	{
		bool quit = false;
		std::string buffer;

		while (quit != true)
		{
			std::getline(std::cin, buffer);
			sendMessage(buffer);

			if (buffer == "disconnect")
			{
				quit = true;
				break;
			}
		}
	}

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
		std::cout << "connected to server" << std::endl;
	}

	void startReceiver()
	{
		std::thread receiver(&tcpClient::receiveThread, this);
		receiver.detach();
	}

	void startSender()
	{
		std::thread sender(&tcpClient::senderThread, this);
		sender.join();
	}

	void clientShutdown()
	{
		shutdown(clientSocket, NULL);
		closesocket(clientSocket);
		WSACleanup();
	}
};

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

	client.startReceiver();
	client.startSender();

	client.clientShutdown();

	std::cout << "Process terminated... Press enter to close" << std::endl;
	std::cin.ignore();
	return 0;
}