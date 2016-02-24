
#ifndef EMAIL_H
#define EMAIL_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QByteArray>
#include <QSslSocket>

class QBuffer;

// Email delivery service.

class Email : public QObject
{
	Q_OBJECT

	public:
		Email();
		void send(QString to, QString subject, QBuffer& glassFile, QBuffer& image, QString imageType);
		bool sending();

		QList<QString> CCs;

		static const QString subjectPrefix;
		static const QString from;

	signals:
		void success(QString to);
		void failure(QString error);
		void showMessage(const QString& message, unsigned int timeout);
	
	private slots:
		void socketReadyRead();
		void socketErrorReceived(QAbstractSocket::SocketError error);
		void socketDisconnected();

	private:
		QString to;
		QString subject;
		QByteArray message;
		QSslSocket socket;
		enum {Init, Helo, From, To1, To2, Data, Body, Quit} state;
		QList<QString>::iterator ccit;
};

#endif
