#include <iostream>
#include "email.h"

const QString Email::smtpServer("smtp.virtualglass.org");
const QString Email::subjectPrefix("VirtualGlass ");
const QString Email::from("feedback@virtualglass.org");

Email::Email(QString to, QString subject) : to (to), subject (subject)
{
    message += "From: " + from.toAscii() + "\r\n";
    message += "To: " + to.toAscii() + "\r\n";
    message += "Subject: " + subjectPrefix + subject.toAscii() + "\r\n";
    message += "Content-Type: multipart/mixed; boundary=VirtualGlassBoundary\r\n";
    message += "MIME-Version: 1.0\r\n";
    message += "\r\n";
    message += "This is a VirtualGlass message with multiple parts in MIME format.\r\n";
    message += "--VirtualGlassBoundary\r\n";
    message += "Content-Type: text/plain\r\n";
    message += "\r\n";
    message += "VirtualGlass files attached.\r\n";
    message += "\r\n";
    message += "Download our software from http://virtualglass.org/\r\n";
    message += "--VirtualGlassBoundary\r\n";
}

void Email::attachGlass(QString filename)
{
	QFile file(filename);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
    message += "Content-Type: application/glass\r\n";
    message += "Content-Transfer-Encoding: 8bit\r\n";
    message += "\r\n";
    message += file.readAll();
    message += "--VirtualGlassBoundary\r\n";
}

void Email::attachImage(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    message += "Content-Type: application/glass\r\n";
    message += "Content-Transfer-Encoding: base64\r\n";
    message += "\r\n";  // done at beginning of loop
/* TODO: TEST AND INCLUDE THIS CODE!
    QString base64 = file.readAll().toBase64();
    for (int i = 0; i < base64.length(); i++) {
        message.append(base64[i]);
        if (i % 76 == 75)
            message += "\r\n";
    }
    if (base64.length() > 0 && base64.length() % 76 != 0)
        message += "\r\n";
*/
    message += "--VirtualGlassBoundary\r\n";
    file.close();
}

void Email::send()
{
    //follow https://github.com/nicholassmith/Qt-SMTP
    QFile file("./tmp.msg");
    file.open(QIODevice::WriteOnly);
    file.write(message);
    file.close();
}
