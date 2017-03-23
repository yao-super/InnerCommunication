#ifndef VACMAININTERFACE_H
#define VACMAININTERFACE_H
#include <QMainWindow>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <string>
class QLabel;
class QTextEdit;
class vacVideoTransfer;
class QPushButton;
class QTextBrowser;
class vacMessageTransfer;
class vacMainInterface : public QMainWindow
{
	Q_OBJECT 
public:
	vacMainInterface();
	~vacMainInterface();
public slots:
	void QImageShow(QImage img);
	void NewVideoTransfer();
	void DisplayMessage();
	void DisplayReceiveMessage(QString s);
	void RemoteQImageShow(QImage img);
	void DestroyVideo();
private:
	QTextBrowser *m_ChatShowBrowser;
	QLabel *m_DisplayLabel;
	QLabel *m_VideoShowLabelLacal;
	QLabel *m_VideoShowLabelRemote;
	QTextEdit *m_ChatInputEdit;
    vacVideoTransfer *m_VideoTransfer;
	vacMessageTransfer *m_MessageTransfer;
	QPushButton *m_PlayButton;
	QPushButton *m_SendButton;
	QPushButton *m_StopButton;
protected:
    void paintEvent(QPaintEvent * e );
	bool eventFilter(QObject *obj, QEvent *e);
};

#endif // VACMAININTERFACE_H