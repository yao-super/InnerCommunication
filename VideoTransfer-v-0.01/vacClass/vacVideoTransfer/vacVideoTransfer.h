#ifndef VACVIDEOTRANSFER_H
#define VACVIDEOTRANSFER_H

#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QThread>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
using namespace cv;
# define PACKAGE_SIZE 1024

typedef struct{
	unsigned char packagebuf[PACKAGE_SIZE];
	int flag;
	int pkgs;
	int size;
} DataPackage;

class vacVideoSender:public QThread
{
	Q_OBJECT
public:
	vacVideoSender(SOCKET& socket);
	~vacVideoSender();
signals:
	void SendQImage(QImage img);
private:
	sockaddr_in m_VideoRecvAddrS;
	int m_vAddrLen;
	DataPackage m_SendPackage;
	SOCKET m_VideoSocketSender;
	VideoCapture m_iCap;
	Mat m_ImgSrc;
	QImage m_chImageS;
	int m_nImWidthS;
	int m_nImHeightS; 
	int m_nBytePerLineS;	
	unsigned char *m_chImageDataS;
protected:
	void run();
};

class vacVideoReceiver:public QThread
{
	Q_OBJECT
public:
	vacVideoReceiver(SOCKET& socket);
	~vacVideoReceiver();
signals:
	void SendRomoteQImage(QImage img);
private:
	SOCKET m_VideoSocketReceiver;
	sockaddr_in m_VideoSendAddr;
	sockaddr_in m_VideoRecvAddrR;
	DataPackage m_RecvPackage;
	int m_sendaddrLen;

	QImage m_chImageR;
	int m_nImWidthR;
	int m_nImHeightR; 
	int m_nBytePerLineR;	
	unsigned char *m_chImageDataR;
	vector<unsigned char> m_Imgresult;
protected:
	void run();
};

class vacVideoTransfer:public QDialog
{
public:
	vacVideoTransfer(QWidget *parent = 0);
	~vacVideoTransfer();
	vacVideoSender *m_VideoSender;
	vacVideoReceiver *m_VideoReceiver;
private:
	SOCKET m_VideoSocket;                                         
	void InitalVideoSocket();
};

#endif  //VACVIDEOTRANSFER_H