#ifndef VACMESSAGETRANSFER_H
#define VACMESSAGETRANSFER_H

#include <iostream>
#include <WinSock2.h>
#include <QDialog>
#include <QWidget>
#include <QThread>
#include <string>
#pragma comment(lib,"ws2_32.lib")
#define BUF_SIZE_SENDER   128
#define BUF_SIZE_RECEIVER 65535
using namespace std;


class vacMessageSender : public QThread
{
public:
	vacMessageSender(SOCKET& socket);
	~vacMessageSender();
	void GetMessages(QString s);
	void SendMessage();
private:
	sockaddr_in m_recvAddrS;
	int m_AddrLen;
	char* m_chBuffSender;
	SOCKET m_SocketSender;
	string m_SenderMessage;
};

class vacMessageReceiver : public QThread
{
	Q_OBJECT
public:
	vacMessageReceiver(SOCKET& socket);
	~vacMessageReceiver();
signals:
	void ReceiveMessages(QString s);
private:
	sockaddr_in m_sendAddr;
	int len;
	sockaddr_in m_recvAddrR;
	SOCKET m_SocketReceiver;
	char* m_chBuffReceiver;
protected:
	void run();
};

class vacMessageTransfer : public QDialog
{
	Q_OBJECT
public:
	vacMessageTransfer(QWidget *parent = 0);
	~vacMessageTransfer();
	
	vacMessageSender *m_MessageSender;
	vacMessageReceiver *m_MessageReceiver;
private:
	SOCKET m_Socket;
	void InitalSocket();
	void NewTransferObject();
};

#endif  //VACMESSAGETRANSFER_H