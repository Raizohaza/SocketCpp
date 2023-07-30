// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include "math.h"
#include "iostream"
#include <string>
#include <thread>
#include <vector>

#define _CRT_SECURE_NO_WARNINGS

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CWinApp theApp;

using namespace std;

struct Ticket
{
	int amount = 0;
	int price = 0;
};

struct Trip
{
	char name[50];
	Ticket A;
	Ticket B;
	Ticket C;
};

struct Info
{
	Trip info[100];
	int NumofInfo = 0;
};

struct Booking
{
	char name[50];
	char type;
	int soluong;
};

struct MyStruct
{
	CSocket* sockClients;
	int id;
	Info thongtinve;
};

bool stop = false;

void Fileread(string filename, Info &thongtinve)
{
	int n = 0;
	errno_t err;
	FILE* f;
	err = freopen_s(&f, "input.txt", "r", stdin);
	if (f == NULL)
		return;
	cin >> n;
	//cin.ignore();

	//thongtinve.info = new Trip[n];
	thongtinve.NumofInfo = n;

	for (int i = 0; i < n; i++)
	{
		cin.ignore();
		
		fgets(thongtinve.info[i].name, sizeof(thongtinve.info[i].name), stdin);

		cin >> thongtinve.info[i].A.amount;
		cin >> thongtinve.info[i].A.price;

		cin >> thongtinve.info[i].B.amount;
		cin >> thongtinve.info[i].B.price;

		cin >> thongtinve.info[i].C.amount;
		cin >> thongtinve.info[i].C.price;
	}
	fclose(f);
}

void BuyTicket(CSocket* sockClients, int id,Info &thongtinve)
{
	int kt;
	Booking data;
	int i = id;
	do
	{
		//nhan thong tin tu client
		sockClients->Receive((char*)&data, sizeof(data), 0);

		cout << "Nhap Ten chuyen tau: " << data.name;

		cout << "Nhap loai tau(A, B, C): " << data.type << endl;

		cout << "Nhap so luong ve muon mua: " << data.soluong << endl;

		int total = 0;
		int flag = 0;

		for (int j = 0; j < thongtinve.NumofInfo; j++)
		{
			if (strcmp(data.name, thongtinve.info[j].name) == 0)
				switch (data.type)
				{
				case 'A':
					flag = thongtinve.info[j].A.amount;
					if (data.soluong > 0 && data.soluong <= flag)
					{
						total = data.soluong * thongtinve.info[j].A.price;
						thongtinve.info[j].A.amount -= data.soluong;
					}
					else
						total = -1;
					break;
				case 'B':
					flag = thongtinve.info[j].B.amount;
					if (data.soluong > 0 && data.soluong <= flag)
					{
						total = data.soluong * thongtinve.info[j].B.price;
						thongtinve.info[j].B.amount -= data.soluong;
					}
					else
						total = -1;
					break;
				case 'C':
					flag = thongtinve.info[j].C.amount;
					if (data.soluong > 0 && data.soluong <= flag)
					{
						total = data.soluong * thongtinve.info[j].C.price;
						thongtinve.info[j].C.amount -= data.soluong;
					}
					else
						total = -1;
					break;
				default:
					total = -1;
					break;
				}
			if (flag != 0)
				break;
		}

		sockClients->Send((char*)&total, sizeof(total));
		if (total == -1)
			sockClients->Send((char*)&flag, sizeof(flag));
		sockClients->Receive((char*)&kt, sizeof(kt), 0);
		if(kt == 89 || kt == 121)
			//Send so luong ve cho client
			sockClients->Send((char*)&thongtinve, sizeof(thongtinve), 0);
	} while (kt == 89 || kt == 121);
	//stop = true;
}

void HandleClient(CSocket* sockClient, int id, Info &thongtinve)
{
	// Process client requests as before, using the provided BuyTicket function
	BuyTicket(sockClient, id, thongtinve);

	// Close the client socket after processing is done
	sockClient->Close();
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		CSocket server;
		const unsigned int port = 1234;
		int i;
		AfxSocketInit(NULL);

		server.Create(port);
		server.Listen(5);

		//Nhap so luong client
		cout << "\n Nhap so luong Client: ";
		int num_client;

		cin >> num_client;
		cout << "\n Dang lang nghe ket noi tu Client...\n";

		Info thongtinve;
		string filename = "input.txt";
		Fileread(filename, thongtinve);

		//Tao mang chua cac socket client
		CSocket* sockClients = new CSocket[num_client];
		std::vector<std::thread> clientThreads;

		for (i = 0; i < num_client; i++) {
			server.Accept(sockClients[i]);
			printf("Da tiep nhan client %d/%d\n", i + 1, num_client);

			//Bao cho client biet minh la client thu may
			sockClients[i].Send((char*)&i, sizeof(int), 0);
			//Send so luong ve cho client
			sockClients[i].Send((char*)&thongtinve, sizeof(thongtinve), 0);
			clientThreads.emplace_back(HandleClient, &sockClients[i], i, thongtinve);
		}

		int count = 0;
		//// Wait for all client threads to finish
		for (size_t i = 0; i < clientThreads.size(); i++) {
			if (clientThreads[i].joinable()) {
				printf_s("thread %d", clientThreads.size());
				clientThreads[i].join();
			}
		}

		delete[] sockClients;
		getchar();
		server.Close();
		printf_s("thread %d", clientThreads.size());
		return nRetCode;
	}
	getchar();
	return nRetCode;
}
