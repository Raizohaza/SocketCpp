// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"
#include "stdio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

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
		CSocket client;
		char sAdd[1000];
		unsigned int port = 1234; //Cung port voi server
		AfxSocketInit(NULL);

		//1. Tao socket
		client.Create();

		// Nhap dic chi IP cua server
		printf("\n Nhap dia chi IP cua server: ");
		gets_s(sAdd);
		
		if (client.Connect(CA2W(sAdd),port))
		{
			printf("\n Client da ket noi toi server");
			//Nhan tu server, cho biet day la client thu may
			int id;
			Info thongtinve;
			client.Receive((char*)&id,sizeof(id),0);
			

			printf(" \nDay la client thu %d\n",id+1);
			cout << "---------------------------------------\n";

			int kt;
			do
			{
				client.Receive((char*)&thongtinve, sizeof(thongtinve), 0);
				printf("Danh sach cac chuyen tau:\n");
				for (int i = 0; i < thongtinve.NumofInfo; i++)
				{
					cout << (thongtinve.info[i].name);

					cout << "A: So luong: " << thongtinve.info[i].A.amount;
					cout << " ,gia: " << thongtinve.info[i].A.price << " USD/ve" << endl;

					cout << "B: So luong: " << thongtinve.info[i].B.amount;
					cout << " ,gia: " << thongtinve.info[i].B.price << " USD/ve" << endl;

					cout << "C: So luong: " << thongtinve.info[i].C.amount;
					cout << " ,gia: " << thongtinve.info[i].C.price << " USD/ve" << endl;

					cout << endl;
				}

				cout << "---------------------------------------";

				//Data gui ve server
				Booking	data;

				cout << "\nNhap Ten chuyen tau ";
				fgets(data.name, sizeof(data.name), stdin);

				char chr;
				cout << "Nhap loai tau(A, B, C): ";
				cin >> chr;
				data.type = toupper(chr);

				cout << "Nhap so luong ve muon mua: ";
				cin >> data.soluong;

				client.Send((char*)&data, sizeof(data), 0);

				int success;
				int flag;
				client.Receive((char*)&success, sizeof(success), 0);

				if (success != -1)
					cout << "So tien can thanh toan: " << success;
				else
				{
					client.Receive((char*)&flag, sizeof(flag), 0);
					cout << "So luong ve muon mua khong kha dung!\nSo luong ve hien co: " << flag;
				}
				cout << "\nBan muon tiep tuc mua ve [Y/N]";
				getchar();
				kt = getchar();
				putchar(kt);
				cin.ignore();
				system("cls");

				client.Send((char*)&kt, sizeof(kt), 0);
			} while (kt == 89 || kt == 121);
			
			client.Close();
		}
		else
			printf("Khong connect duoc toi server....");
		
	}
	getchar();
	return nRetCode;
}
