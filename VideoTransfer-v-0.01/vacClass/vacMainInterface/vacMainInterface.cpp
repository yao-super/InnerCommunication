#include <QtGui>
#include <QApplication>

#include "vacMainInterface.h"
#include "vacVideoTransfer.h"
#include "vacMessageTransfer.h"

vacMainInterface::vacMainInterface()
{
	setFixedSize(660,450);
	setWindowTitle("InnerCommuntion");
	m_DisplayLabel = new QLabel("<strong><font size=7><i><font color=#8E236B>Designed By YY</font></i></font></strong>",this);
	//m_DisplayLabel = new QLabel("<strong><font size=7><i><font color=#8E236B>Designed For 徐涛涛</font></i></font></strong>",this);
	m_DisplayLabel->setGeometry(0,0,660,50);
	m_DisplayLabel->setStyleSheet("border:2px solid white;");

	m_ChatShowBrowser = new QTextBrowser(this);
	m_ChatShowBrowser->setGeometry(0,50,400,250);
	m_ChatShowBrowser->setFont(QFont( "Timers" , 15 ,  QFont::Bold) );
	QBrush myBrush;
	QPalette palettetext;
	myBrush = QBrush(Qt::red,Qt::DiagCrossPattern);
	palettetext.setBrush( QPalette::Text, myBrush);
	m_ChatShowBrowser->setPalette(palettetext);

	QPalette paletteback;
	paletteback = m_ChatShowBrowser->palette();
	paletteback.setBrush(QPalette::Base,QBrush(QColor(0,250,154,0)));
	m_ChatShowBrowser->setPalette(paletteback);	
	m_ChatShowBrowser->setStyleSheet("border:2px solid white;");

	m_ChatInputEdit = new QTextEdit(this);
	m_ChatInputEdit->setGeometry(0,330,400,150);
	m_ChatInputEdit->installEventFilter(this);
	m_ChatInputEdit->setFont(QFont( "Timers" , 15 ,  QFont::Bold) );
	m_ChatInputEdit->setPalette(palettetext);
    m_ChatInputEdit->setPalette(paletteback);	
	m_ChatInputEdit->setStyleSheet("border:2px solid white;");

	m_PlayButton = new QPushButton(this);
	
	QPalette pal;
	pal.setColor(QPalette::ButtonText, QColor(0,0,255));   
	m_PlayButton->setPalette(pal); 
	m_PlayButton->setText("视频"); 
	m_PlayButton->setFlat(true);
	m_PlayButton->setGeometry(0,300,40,30);
	connect(m_PlayButton,SIGNAL(clicked()),
		this,SLOT(NewVideoTransfer()));

	m_StopButton = new QPushButton(this);
	m_StopButton->setPalette(pal);
	m_StopButton->setText("停止");
	m_StopButton->setFlat(true);
	m_StopButton->setGeometry(40,300,40,30);
	connect(m_StopButton,SIGNAL(clicked()),
		this,SLOT(DestroyVideo()));

	m_VideoShowLabelRemote = new QLabel(this);
	m_VideoShowLabelRemote->setGeometry(400,50,260,200);
	m_VideoShowLabelRemote->setStyleSheet("border:2px solid white;");

	m_VideoShowLabelLacal=new QLabel(this);
	m_VideoShowLabelLacal->setGeometry(400,250,260,200);
	m_VideoShowLabelLacal->setStyleSheet("border:2px solid white;");	

	m_SendButton = new QPushButton(this);
	m_SendButton->setPalette(pal); 
	m_SendButton->setText("发送"); 
	m_SendButton->setFlat(true);
	m_SendButton->setGeometry(360,420,40,30);
    connect(m_SendButton,SIGNAL(clicked()),
		this,SLOT(DisplayMessage())); 
	m_MessageTransfer = new vacMessageTransfer;
	connect(m_MessageTransfer->m_MessageReceiver,SIGNAL(ReceiveMessages(QString)),
		this,SLOT(DisplayReceiveMessage(QString)),Qt::BlockingQueuedConnection);
	m_VideoTransfer = NULL;
}

void vacMainInterface::DisplayMessage()
{
	m_MessageTransfer->m_MessageSender->GetMessages(m_ChatInputEdit->toPlainText());
	m_MessageTransfer->m_MessageSender->SendMessage();
	m_ChatShowBrowser->insertPlainText(m_ChatInputEdit->toPlainText()+"\n");
	m_ChatInputEdit->clear();
}

void vacMainInterface::DestroyVideo()
{
	if(m_VideoTransfer)	
   {
	   m_VideoTransfer->m_VideoSender->terminate();
	   m_VideoTransfer->m_VideoReceiver->terminate();
	   delete m_VideoTransfer;
	   m_VideoTransfer=NULL;
   }
	m_VideoShowLabelRemote->clear();
	m_VideoShowLabelLacal->clear();
}
void vacMainInterface::DisplayReceiveMessage(QString s)
{
	m_ChatShowBrowser->insertPlainText(s+"\n");
}

void vacMainInterface::NewVideoTransfer()
{
	m_VideoTransfer = new vacVideoTransfer(this);
	connect(m_VideoTransfer->m_VideoSender,SIGNAL(SendQImage(QImage)),
		this,SLOT(QImageShow(QImage)));
	connect(m_VideoTransfer->m_VideoReceiver,SIGNAL(SendRomoteQImage(QImage)),
		this,SLOT(RemoteQImageShow(QImage)));
}



void vacMainInterface::QImageShow(QImage img)
{
	m_VideoShowLabelLacal->setPixmap(QPixmap::fromImage(img));
}

void vacMainInterface::RemoteQImageShow(QImage img)
{
	m_VideoShowLabelRemote->setPixmap(QPixmap::fromImage(img));
}
void vacMainInterface::paintEvent( QPaintEvent * e)
{
	QPainter p(this);
    p.drawPixmap(0, 0, width(), height(), QPixmap("E:/image/2.jpg"));
}

bool vacMainInterface::eventFilter(QObject *obj, QEvent *e)
{
	  Q_ASSERT(obj == m_ChatInputEdit);
	  if (e->type() == QEvent::KeyPress)
	  {
		  QKeyEvent *event = static_cast<QKeyEvent*>(e);
		  if (event->key() == Qt::Key_Return)
		  {
			  DisplayMessage(); //发送消息的槽
			  return true;
		  }
	  }
	  return false;
}

vacMainInterface::~vacMainInterface()
{
	if(m_VideoTransfer)
	{   
		m_VideoTransfer->m_VideoSender->terminate();
	    m_VideoTransfer->m_VideoReceiver->terminate();
		delete m_VideoTransfer;
	}
	
	delete m_MessageTransfer;
}