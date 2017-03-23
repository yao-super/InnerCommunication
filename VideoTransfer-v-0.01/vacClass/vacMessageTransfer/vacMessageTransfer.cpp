#include "vacMessageTransfer.h"

vacMessageTransfer::vacMessageTransfer(QWidget *parent):QDialog(parent)
{

	InitalSocket();
	NewTransferObject();
}

void vacMessageTransfer::InitalSocket()
{
	WSADATA wsaData;

	if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		cout<<"WSAStartup failed!"<<endl;
		exit(1);
	}

	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (m_Socket == INVALID_SOCKET)
	{
		cout<<"socket failed!"<<endl;
		WSACleanup();
		exit(1);
	}

}

void vacMessageTransfer::NewTransferObject()
{
	m_MessageSender = new vacMessageSender(m_Socket);
	m_MessageReceiver = new vacMessageReceiver(m_Socket);
	m_MessageReceiver->start();
}

void vacMessageSender::GetMessages(QString s)
{
	m_SenderMessage = s.toStdString();
}

vacMessageTransfer::~vacMessageTransfer()
{
	closesocket(m_Socket);
	WSACleanup();
}


vacMessageSender::vacMessageSender(SOCKET& socket):m_SocketSender(socket)
{
	m_chBuffSender = new char[BUF_SIZE_SENDER];
	m_recvAddrS.sin_family = AF_INET;
	m_recvAddrS.sin_port = htons(8888);
	m_recvAddrS.sin_addr.S_un.S_addr = inet_addr("192.168.10.110");
	m_AddrLen = sizeof(m_recvAddrS);
}

void vacMessageSender::SendMessage()
{
	   memset(m_chBuffSender,0,BUF_SIZE_SENDER);
	   strcpy_s(m_chBuffSender, m_SenderMessage.size() + 1, m_SenderMessage.c_str());
	   int n = sendto(m_SocketSender,m_chBuffSender,BUF_SIZE_SENDER,0,(sockaddr *)&m_recvAddrS,m_AddrLen);
	   if(n==-1)
	   {
		   cout<<"sendMessage failed"<<endl;
		   return;
	   }
}

vacMessageSender::~vacMessageSender()
{
	delete[] m_chBuffSender;
}


vacMessageReceiver::vacMessageReceiver(SOCKET& socket):m_SocketReceiver(socket)
{
	m_chBuffReceiver = new char[BUF_SIZE_RECEIVER];
	len = sizeof(m_sendAddr);
	m_recvAddrR.sin_family = AF_INET;
	m_recvAddrR.sin_port = htons(8888);
	m_recvAddrR.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret = bind(m_SocketReceiver,(sockaddr*)&m_recvAddrR,sizeof(m_recvAddrR));
	if (SOCKET_ERROR == ret)
	{
		cout<<"bind error!"<<endl;
		closesocket(m_SocketReceiver);
		WSACleanup();
		exit(-1);
	}
}

void vacMessageReceiver::run()
{
	while (true)
	{   
		memset(m_chBuffReceiver,0,BUF_SIZE_RECEIVER);
		int m = recvfrom(m_SocketReceiver,m_chBuffReceiver,BUF_SIZE_RECEIVER,0,(sockaddr*)&m_sendAddr,&len);
        if ( m==-1)
		{
			cout<<"recvMessage failed"<<endl;
			//closesocket(m_SocketReceiver);
			//WSACleanup();
			//exit(-1);
			continue;
		}
		//cout << "Message received " << m_chBuffReceiver << endl; 
		string tempstr(m_chBuffReceiver);
		emit(ReceiveMessages(QString::fromStdString(tempstr)));
	}
}

vacMessageReceiver::~vacMessageReceiver()
{
	delete[] m_chBuffReceiver;
}