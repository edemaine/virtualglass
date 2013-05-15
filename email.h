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
		Email(QString to, QString subject);
		void attachGlass(QBuffer& buffer);
		void attachImage(QBuffer& filename, QString contentType);
		void send();

		static const QString subjectPrefix;
		static const QString from;

	signals:
		void success(QString to);
		void failure(QString error);

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
};

#endif
