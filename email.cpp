#include <iostream>
#include "email.h"

const QString Email::smtpServer("smtp.virtualglass.org");
const QString Email::subjectPrefix("VirtualGlass ");
const QString Email::from("feedback@virtualglass.org");

Email::Email(QString to, QString subject) : to (to), subject (subject)
{
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
    //          1234567890123456789012345678901234567890123456789012345678901234567890123456
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
    message += "- the Virtual Glass team\r\n";
    message += "--VirtualGlassBoundary\r\n";
}

void Email::attachGlass(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    message += "Content-Type: application/glass; name=\"share.glass\"\r\n";
    message += "Content-Transfer-Encoding: 8bit\r\n";
    message += "Content-Disposition: attachment; filename=\"share.glass\"\r\n";
    message += "\r\n";
    message += file.readAll();
    message += "--VirtualGlassBoundary\r\n";
}

void Email::attachImage(QString filename, QString contentType)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    message += "Content-Type: image/" + contentType + "\r\n";
    message += "Content-Transfer-Encoding: base64\r\n";
    message += "Content-Disposition: inline\r\n";
    message += "\r\n";  // done at beginning of loop
    QString base64 = file.readAll().toBase64();
    for (int i = 0; i < base64.length(); i++) {
        message += base64[i].toAscii();
        if (i % 76 == 75)
            message += "\r\n";
    }
    if (base64.length() > 0 && base64.length() % 76 != 0)
        message += "\r\n";
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
