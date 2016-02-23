
#include <vector>
#include <string>
#include <QLabel>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>

#include "emaildialog.h"

using std::vector;
using std::string;

EmailDialog :: EmailDialog(QDialog* parent) : QDialog(parent)
{
	setModal(true);

	setupLayout();

	setWindowTitle("Email your design");
	lineEdit->setText("");
}

void EmailDialog :: setupLayout()
{
	layout = new QVBoxLayout(this);
	this->setLayout(layout);

	addressLayout = new QHBoxLayout(this);
	addressLayout->addStretch(1);
	addressLayout->addWidget(new QLabel("Email address:", this));
	lineEdit = new QLineEdit(this);
	lineEdit->setFixedWidth(200);
	addressLayout->addWidget(lineEdit);

	QPushButton* sendButton = new QPushButton("Send", this);
	addressLayout->addWidget(sendButton);
	connect(sendButton, SIGNAL(clicked()), this, SLOT(sendClicked()));

	addressLayout->addStretch(1);
	layout->addLayout(addressLayout);
	
	const QString kRow0Keys[] = {"!", "@", "#", "$", "%", "^", QString("&&"), "*", "_", "-", "."};
	const QString kRow1Keys[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
	const QString kRow2Keys[] = {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"};
	const QString kRow3Keys[] = {"A", "S", "D", "F", "G", "H", "J", "K", "L"};
	const QString kRow4Keys[] = {"Z", "X", "C", "V", "B", "N", "M"};

	int keySize = 50;
	QToolButton* keyButton;
	mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(QString)), this, SLOT(keyClicked(QString)));

	kRow0Layout = new QHBoxLayout(this);
	layout->addLayout(kRow0Layout);
	kRow0Layout->addStretch(1);
	for (int i = 0; i < 11; ++i)
	{
		keyButton = new QToolButton(this);
		keyButton->setText(kRow0Keys[i]);
		keyButton->setFixedSize(keySize, keySize);
		kRow0Layout->addWidget(keyButton);
		mapper->setMapping(keyButton, kRow0Keys[i]);
		connect(keyButton, SIGNAL(clicked()), mapper, SLOT(map()));
	} 
	kRow0Layout->addStretch(1);

	kRow1Layout = new QHBoxLayout(this);
	layout->addLayout(kRow1Layout);
	kRow1Layout->addStretch(1);
	for (int i = 0; i < 10; ++i)
	{
		keyButton = new QToolButton(this);
		keyButton->setText(kRow1Keys[i]);
		keyButton->setFixedSize(keySize, keySize);
		kRow1Layout->addWidget(keyButton);
		mapper->setMapping(keyButton, kRow1Keys[i]);
		connect(keyButton, SIGNAL(clicked()), mapper, SLOT(map()));
	} 
	kRow1Layout->addStretch(1);

	kRow2Layout = new QHBoxLayout(this);
	layout->addLayout(kRow2Layout);
	kRow2Layout->addStretch(1);
	for (int i = 0; i < 10; ++i)
	{
		keyButton = new QToolButton(this);
		keyButton->setText(kRow2Keys[i]);
		keyButton->setFixedSize(keySize, keySize);
		kRow2Layout->addWidget(keyButton);
		mapper->setMapping(keyButton, kRow2Keys[i]);
		connect(keyButton, SIGNAL(clicked()), mapper, SLOT(map()));
	} 
	kRow2Layout->addStretch(1);

	kRow3Layout = new QHBoxLayout(this);
	layout->addLayout(kRow3Layout);
	kRow3Layout->addStretch(1);
	for (int i = 0; i < 9; ++i)
	{
		keyButton = new QToolButton(this);
		keyButton->setText(kRow3Keys[i]);
		keyButton->setFixedSize(keySize, keySize);
		kRow3Layout->addWidget(keyButton);
		mapper->setMapping(keyButton, kRow3Keys[i]);
		connect(keyButton, SIGNAL(clicked()), mapper, SLOT(map()));
	} 
	kRow3Layout->addStretch(1);

	kRow4Layout = new QHBoxLayout(this);
	layout->addLayout(kRow4Layout);
	kRow4Layout->addStretch(1);
	for (int i = 0; i < 7; ++i)
	{
		keyButton = new QToolButton(this);
		keyButton->setText(kRow4Keys[i]);
		keyButton->setFixedSize(keySize, keySize);
		kRow4Layout->addWidget(keyButton);
		mapper->setMapping(keyButton, kRow4Keys[i]);
		connect(keyButton, SIGNAL(clicked()), mapper, SLOT(map()));
	} 
	QToolButton* backspaceButton = new QToolButton(this);
	backspaceButton->setFixedSize(2 * keySize, keySize);
	backspaceButton->setText("Backspace");
	kRow4Layout->addWidget(backspaceButton);	
	connect(backspaceButton, SIGNAL(clicked()), this, SLOT(backspaceClicked()));
	kRow4Layout->addStretch(1);
}

EmailDialog :: ~EmailDialog()
{
	delete addressLayout->itemAt(0)->widget(); // label
	delete lineEdit;	
	delete addressLayout;

	while (kRow0Layout->count())
		delete kRow0Layout->takeAt(0)->widget();
	delete kRow0Layout;
	
	while (kRow1Layout->count())
		delete kRow1Layout->takeAt(0)->widget();
	delete kRow1Layout;

	while (kRow2Layout->count())
		delete kRow2Layout->takeAt(0)->widget();
	delete kRow2Layout;

	while (kRow3Layout->count())
		delete kRow3Layout->takeAt(0)->widget();
	delete kRow3Layout;

	while (kRow4Layout->count())
		delete kRow4Layout->takeAt(0)->widget();
	delete kRow4Layout;

	delete mapper;
	delete layout;
}

void EmailDialog :: sendClicked()
{
	accept();
}

void EmailDialog :: keyClicked(QString s)
{
	lineEdit->setText(lineEdit->text() + s);
}

void EmailDialog :: backspaceClicked()
{
	QString text = lineEdit->text();
	if (text.size() == 0)
		return;
	
	text = text.remove(text.size()-1, 1);
	lineEdit->setText(text);
}

QString EmailDialog :: address()
{
	return lineEdit->text();
}

