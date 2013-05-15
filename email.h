#ifndef EMAIL_H
#define EMAIL_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>

// Email delivery service.

class Email : public QObject
{
	Q_OBJECT

	public:
		Email(QString to, QString subject);
		void attachGlass(QString filename);
		void attachImage(QString filename, QString contentType);
		void send();

		static const QString smtpServer;
		static const QString subjectPrefix;
		static const QString from;

	private:
		QString to;
		QString subject;
        QByteArray message;
};

#endif
