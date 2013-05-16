
#include <QBuffer>

#include "email.h"

// SMTP code based on https://github.com/nicholassmith/Qt-SMTP
// by Nicholas Smith, 2012 (MIT License)

const QString Email::subjectPrefix("VirtualGlass ");
const QString Email::from("feedback@virtualglass.org");

const char *smtpServer = "smtp.virtualglass.org";
const int smtpPort = 465;
const char *HELO = "HELO GUI.VirtualGlass.org\r\n";

Email::Email() : socket (this)
{
	connect(&socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
	connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), 
		this, SLOT(socketErrorReceived(QAbstractSocket::SocketError)));
	connect(&socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
}

void Email::send(QString to, QString subject, QBuffer& glassFile, QBuffer& imageFile, QString imageType)
{
	// kill any previous send() that didn't finish
	socket.abort();
	message.clear();

	this->to = to;
	this->subject = subject;	

	message += "From: " + from.toAscii() + "\r\n";
	message += "To: " + to.toAscii() + "\r\n";
	message += "Cc: " + from.toAscii() + "\r\n";
	message += "Subject: " + subjectPrefix + subject.toAscii() + "\r\n";
	message += "Content-Type: multipart/mixed; boundary=VirtualGlassBoundary\r\n";
	message += "MIME-Version: 1.0\r\n";
	message += "\r\n";
	message += "This is a VirtualGlass message with multiple parts in MIME format.\r\n";
	message += "--VirtualGlassBoundary\r\n";
	message += "Content-Type: text/plain; format=flowed\r\n";
	message += "Content-Disposition: inline\r\n";
	message += "\r\n";
	message += "A VirtualGlass designer (probably you) has shared an example of \r\n";
	message += "computer-aided design of blown glass.  You will find attached an image of \r\n";
	message += "the design, along with the .glass file which you can open in the \r\n";
	message += "VirtualGlass software.\r\n";
	message += "\r\n";
	message += "The VirtualGlass software lets anyone, with any or no glass experience, \r\n";
	message += "design and visualize glass cane, and to get a sense of what that cane will \r\n";
	message += "look like on a blown piece, all on your computer (without lifting a blow \r\n";
	message += "pipe!). The goal is to allow the exploration of cane designs, especially \r\n";
	message += "new cane designs, and settle on a nice-looking design before going through \r\n";
	message += "the effort of making it.\r\n";
	message += "\r\n";
	message += "For more information, and to download the freely available VirtualGlass \r\n";
	message += "software, visit http://virtualglass.org/\r\n";
	message += "\r\n";
	message += "- the VirtualGlass team\r\n";
	message += "--VirtualGlassBoundary\r\n";

	glassFile.open(QIODevice::ReadOnly | QIODevice::Text);
	message += "Content-Type: application/glass; name=\"shared-design.glass\"\r\n";
	message += "Content-Transfer-Encoding: 8bit\r\n";
	message += "Content-Disposition: attachment; filename=\"shared-design.glass\"\r\n";
	message += "\r\n";
	message += glassFile.readAll();
	message += "--VirtualGlassBoundary\r\n";
	glassFile.close();

	imageFile.open(QIODevice::ReadOnly); 
	message += "Content-Type: image/" + imageType + "\r\n";
	message += "Content-Transfer-Encoding: base64\r\n";
	message += "Content-Disposition: inline\r\n";
	message += "\r\n";  // done at beginning of loop
	QString base64 = imageFile.readAll().toBase64();
	for (int i = 0; i < base64.length(); i++) 
	{
		message += base64[i].toAscii();
		if (i % 76 == 75)
			message += "\r\n";
	}
	if (base64.length() > 0 && base64.length() % 76 != 0)
		message += "\r\n";
	message += "--VirtualGlassBoundary\r\n";
	imageFile.close();

	state = Init;
	socket.connectToHost(smtpServer, smtpPort);
}

void Email::socketReadyRead()
{
	QString received;
	do
	{
		received = socket.readLine();
	}
	while (socket.canReadLine() && received[3] != ' ');

	switch (state) 
	{
		case Init:
			if (received[0] != '2')
				emit failure("SMTP opening: " + received);
			else 
			{
				socket.write(HELO);
				state = Helo;
			}
			break;
		case Helo:
			if (received[0] != '2')
				emit failure("SMTP HELO response: " + received);
			else 
			{
				socket.write("MAIL FROM: <" + from.toAscii() + ">\r\n");
				state = From;
			}
			break;
		case From:
			if (received[0] != '2')
			emit failure("SMTP MAIL FROM response: " + received);
			else 
			{
				socket.write("RCPT TO: <" + to.toAscii() + ">\r\n");
				state = To1;
			}
			break;
		case To1:
			if (received[0] != '2')
				emit failure("SMTP MAIL TO response: " + received);
			else 
			{
				socket.write("RCPT TO: <" + from.toAscii() + ">\r\n");
				state = To2;
			}
			break;
		case To2:
			if (received[0] != '2')
				emit failure("SMTP MAIL TO response: " + received);
			else 
			{
				socket.write("DATA\r\n");
				state = Data;
			}
			break;
		case Data:
			if (received[0] != '3')
				emit failure("SMTP DATA response: " + received);
			else 
			{
				socket.write(message + ".\r\n");
				state = Body;
			}
			break;
		case Body:
			if (received[0] != '2')
				emit failure("SMTP body response: " + received);
			else 
			{
				socket.write("QUIT\r\n");
				state = Quit;
			}
			break;
		case Quit:
			// Ignore QUIT failure -- can close socket anyway.
			//if (received[0] != '2')
			//    emit failure("SMTP QUIT response: " + received);
			socket.close();
			break;
	}
}

void Email::socketErrorReceived(QAbstractSocket::SocketError error)
{
	emit failure("SMTP socket error: " + error);
}

void Email::socketDisconnected()
{
	if (state == Quit)
		emit success(to);
	else
		emit failure("SMTP closed connection");
}
