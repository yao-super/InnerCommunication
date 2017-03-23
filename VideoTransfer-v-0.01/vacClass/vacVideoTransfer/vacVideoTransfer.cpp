#include "vacVideoTransfer.h"

vacVideoTransfer::vacVideoTransfer(QWidget *parent):QDialog(parent)
{

	InitalVideoSocket();
}

void vacVideoTransfer::InitalVideoSocket()
{
	WSADATA wsaData;

	if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
	{
		cout<<"WSAStartup failed!"<<endl;
		exit(-1);
	}
	
    m_VideoSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	//int imode=1;  
	//ioctlsocket(m_VideoSocket,FIONBIO,(u_long *)&imode); 
	if(m_VideoSocket == INVALID_SOCKET)
	{
		cout<<"socket failed!"<<endl;
		WSACleanup();
		exit(-1);
	}

	m_VideoSender = new vacVideoSender(m_VideoSocket);
	m_VideoSender->start();
	m_VideoReceiver = new vacVideoReceiver(m_VideoSocket);
	m_VideoReceiver->start();
	
}

vacVideoTransfer::~vacVideoTransfer()
{
	closesocket(m_VideoSocket);
	WSACleanup();
	delete m_VideoSender;
	delete m_VideoReceiver;
}

vacVideoSender::vacVideoSender(SOCKET& socket):m_VideoSocketSender(socket),m_nImWidthS(260),m_nImHeightS(200)
{
	m_VideoRecvAddrS.sin_family = AF_INET;
	m_VideoRecvAddrS.sin_port = htons(9999);
	m_VideoRecvAddrS.sin_addr.S_un.S_addr = inet_addr("192.168.10.110");
	m_vAddrLen = sizeof(m_VideoRecvAddrS);
	//m_iCap.open("D:/迅雷下载/七月与安生.HD1280超清国语中字.mp4");
	m_iCap.open(0);
	m_nBytePerLineS = (m_nImWidthS * 24 + 31) / 8;
	m_chImageDataS = new unsigned char[m_nBytePerLineS * m_nImHeightS];
}

void vacVideoSender::run()
{
	while (true)
	{
		//cout<<"senderid: "<<QThread::currentThreadId()<<endl;
		
		m_iCap>>m_ImgSrc;
		if(m_ImgSrc.empty())
		{
			return;
		}
		else
		{
			Mat imgSrcResize;
			cv::resize(m_ImgSrc,imgSrcResize,Size(260,200),0,0,CV_INTER_LINEAR);

			for (int ix = 0; ix < m_nImHeightS; ix++)
			{
			 for (int iy = 0; iy < m_nImWidthS; iy++)
				{
					m_chImageDataS[ix*m_nBytePerLineS + iy*3]     = imgSrcResize.at<Vec3b>(ix, iy)[2];  //r
					m_chImageDataS[ix*m_nBytePerLineS+ iy*3 + 1] = imgSrcResize.at<Vec3b>(ix, iy)[1];  //g
					m_chImageDataS[ix*m_nBytePerLineS + iy*3 + 2] = imgSrcResize.at<Vec3b>(ix, iy)[0]; //b
				}
			}
			m_chImageS= QImage(m_chImageDataS, m_nImWidthS, m_nImHeightS, m_nBytePerLineS, QImage::Format_RGB888);
		    emit SendQImage(m_chImageS);
		
			vector<unsigned char> buff;
			vector<int> param = vector<int>(2);
			param[0]=CV_IMWRITE_JPEG_QUALITY;
			param[1]=60;
			imencode(".jpg",imgSrcResize,buff,param); 

			int PackageNum = buff.size() / PACKAGE_SIZE;
			int LastPackageSize = buff.size() % PACKAGE_SIZE;
			PackageNum =  LastPackageSize ==0  ? PackageNum:PackageNum+1; 

			m_SendPackage.flag = 0; 
			m_SendPackage.pkgs = PackageNum; 

			for (int i=0;i<PackageNum;i++)
			{
				memset(&m_SendPackage.packagebuf,0,PACKAGE_SIZE);
				m_SendPackage.flag = i;
				m_SendPackage.size=(i == (PackageNum - 1))?LastPackageSize:PACKAGE_SIZE;
				memcpy(&m_SendPackage.packagebuf,&buff[i*PACKAGE_SIZE],m_SendPackage.size);
				int n = sendto(m_VideoSocketSender,(char *)&m_SendPackage,sizeof(m_SendPackage),0,(sockaddr *)&m_VideoRecvAddrS,m_vAddrLen);
				if(n == -1)
				{
					cout<<"sendVideo failed!"<<endl;
					closesocket(m_VideoSocketSender);
					continue;   
				}
			}

		}
	}
}


vacVideoSender::~vacVideoSender()
{   
	m_iCap.release();
	delete[] m_chImageDataS;
}

vacVideoReceiver::vacVideoReceiver(SOCKET& socket):m_VideoSocketReceiver(socket),m_nImWidthR(260),m_nImHeightR(200)
{
	m_sendaddrLen = sizeof(m_VideoSendAddr);
	m_VideoRecvAddrR.sin_family = AF_INET;
	m_VideoRecvAddrR.sin_port = htons(9999);
	m_VideoRecvAddrR.sin_addr.s_addr = htonl(INADDR_ANY);
	int retval = bind(m_VideoSocketReceiver,(sockaddr*)&m_VideoRecvAddrR,sizeof(m_VideoRecvAddrR));
	if(SOCKET_ERROR == retval)
	{
		cout<<"bind error !"<<endl;
		closesocket(m_VideoSocketReceiver);
		return;
	}
	m_nBytePerLineR = (m_nImWidthR * 24 + 31) / 8;
	m_chImageDataR = new unsigned char[m_nBytePerLineR * m_nImHeightR];
}

void vacVideoReceiver::run()
{
	string temp;
	string resuit;
	while (true)
	{
		//cout<<"receiverid: "<<QThread::currentThreadId()<<endl;
		int n = recvfrom(m_VideoSocketReceiver,(char*)(&m_RecvPackage),sizeof(m_RecvPackage),0,(sockaddr*)&m_VideoSendAddr,&m_sendaddrLen);
		if(n == -1)
		{
			cout<<"RecvVideo failed"<<endl;
			//closesocket(m_VideoSocketReceiver);
			//WSACleanup();
			return;
		}

		if (m_RecvPackage.flag!=0)
		continue;
		else
		{
			temp.assign((const char*)m_RecvPackage.packagebuf,m_RecvPackage.size);
			resuit+=temp;
			int Paknum = m_RecvPackage.pkgs;
			for(int i=1;i<Paknum;++i)
			{
				recvfrom(m_VideoSocketReceiver,(char*)(&m_RecvPackage),sizeof(m_RecvPackage),0,(sockaddr*)&m_VideoSendAddr,&m_sendaddrLen);
				if(i==m_RecvPackage.flag)
				{
					temp.assign((const char*)m_RecvPackage.packagebuf,m_RecvPackage.size);
					resuit+=temp;
				}
				else
				  break;
			}

			m_Imgresult.assign(resuit.begin(),resuit.end());
			Mat imgresult = imdecode(Mat(m_Imgresult),CV_LOAD_IMAGE_COLOR);

			resuit="";
			m_Imgresult.clear();
			for (int ix = 0; ix < m_nImHeightR; ix++)
			{
				for (int iy = 0; iy < m_nImWidthR; iy++)
				{
					m_chImageDataR[ix*m_nBytePerLineR + iy*3]     = imgresult.at<Vec3b>(ix, iy)[2];  //r
					m_chImageDataR[ix*m_nBytePerLineR+ iy*3 + 1] = imgresult.at<Vec3b>(ix, iy)[1];  //g
					m_chImageDataR[ix*m_nBytePerLineR + iy*3 + 2] = imgresult.at<Vec3b>(ix, iy)[0]; //b
				}
			}
			m_chImageR= QImage(m_chImageDataR, m_nImWidthR, m_nImHeightR, m_nBytePerLineR, QImage::Format_RGB888);
			emit SendRomoteQImage(m_chImageR);

		}
	}
}

vacVideoReceiver::~vacVideoReceiver()
{
	delete[] m_chImageDataR;
}



