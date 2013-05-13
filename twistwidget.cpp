
#include <cstdio>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "twistwidget.h"

TwistWidget :: TwistWidget(float* _twist, unsigned int _range, QWidget* parent) : QWidget(parent)
{
	this->twist = _twist;
	this->range = _range;

	QHBoxLayout* layout = new QHBoxLayout(this);
	this->setLayout(layout);

	QLabel* nameLabel = new QLabel("Twist:", this);
	layout->addWidget(nameLabel);

	minus = new QPushButton("-", this);
	layout->addWidget(minus);

	spin = new QDoubleSpinBox(this);
	spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
	int neg_range = -1 * static_cast<int>(range);
	spin->setRange(neg_range, range);
	spin->setSingleStep(0.1);
	spin->setDecimals(1);
	layout->addWidget(spin, 1);

	plus = new QPushButton("+", this);
	layout->addWidget(plus);

	char buf[20];
	snprintf(buf, 15, "%d.0", neg_range);
	QLabel* leftLabel = new QLabel(buf, this);
	layout->addWidget(leftLabel);

	slider = new QSlider(Qt::Horizontal, this);
	slider->setRange(neg_range * 10, range * 10);
	slider->setSingleStep(1);
	slider->setTickInterval(range * 10);
	slider->setTickPosition(QSlider::TicksBothSides);
	layout->addWidget(slider, 10);

	snprintf(buf, 15, "%d.0", range);  
	QLabel* rightLabel = new QLabel(buf, this);
	layout->addWidget(rightLabel);	

	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));	
	connect(spin, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged(double)));	
	connect(minus, SIGNAL(clicked()), this, SLOT(minusClicked()));
	connect(plus, SIGNAL(clicked()), this, SLOT(plusClicked()));

	updateEverything();
}

void TwistWidget :: updateEverything()
{
	slider->blockSignals(true);
	slider->setValue(10 * *(this->twist));
	slider->blockSignals(false);
	spin->blockSignals(true);
	spin->setValue(*(this->twist));
	spin->blockSignals(false);
	minus->setEnabled(*(this->twist) > spin->minimum() + 0.1);
	plus->setEnabled(*(this->twist) < spin->maximum() - 0.1);
}

void TwistWidget :: spinValueChanged(double v)
{
	*(this->twist) = v;
	updateEverything();
	emit valueChanged();
}

void TwistWidget :: minusClicked()
{
	if (*(this->twist) > spin->minimum() + 0.1)
	{
		*(this->twist) -= 0.1;
		updateEverything();
		emit valueChanged();
	} 
}

void TwistWidget :: plusClicked()
{
	if (*(this->twist) < spin->maximum() - 0.1)
	{
		*(this->twist) += 0.1;
		updateEverything();
		emit valueChanged();
	} 
}

void TwistWidget :: sliderValueChanged(int v)
{
	*(this->twist) = v * 0.1;
	updateEverything();
	emit valueChanged();
}

void TwistWidget :: setTwist(float* _twist)
{
	this->twist = _twist;
	updateEverything();
}

