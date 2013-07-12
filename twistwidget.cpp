
#include <QDoubleSpinBox>
#include <QSlider>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QEvent>

#include <cstdio>

#include "twistwidget.h"

TwistWidget :: TwistWidget(float* _twist, unsigned int _range, QWidget* parent) : QWidget(parent)
{
	this->twist = _twist;
	this->range = _range;

	QHBoxLayout* layout = new QHBoxLayout(this);
	this->setLayout(layout);

	QLabel* nameLabel = new QLabel("Twist:", this);
	layout->addWidget(nameLabel);

	spin = new QDoubleSpinBox(this);
	int neg_range = -1 * static_cast<int>(range);
	spin->setRange(neg_range, range);
	spin->setSingleStep(0.1);
	spin->setDecimals(1);
	spin->installEventFilter(this);
	layout->addWidget(spin, 1);

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
	connect(slider, SIGNAL(sliderReleased()), this, SLOT(sliderChangeEnded()));	
	connect(spin, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged(double)));	

	updateEverything();
}

bool TwistWidget :: eventFilter(QObject* obj, QEvent* event)
{
	// Goal is to stop the spinboxes from eating undo/redo commands
	// for their own text editing purpose. These events should instead
	// go up the chain to our own undo/redo implementation.
	if (obj == spin && event->type() == QEvent::ShortcutOverride)
	{
		event->ignore();
		return true;
	}
	return false;
}

void TwistWidget :: updateEverything()
{
	slider->blockSignals(true);
	slider->setValue(10 * *(this->twist));
	slider->blockSignals(false);
	spin->blockSignals(true);
	spin->setValue(*(this->twist));
	spin->blockSignals(false);
}

void TwistWidget :: sliderChangeEnded()
{
	emit valueChangeEnded();
}

void TwistWidget :: spinValueChanged(double v)
{
	*(this->twist) = v;
	updateEverything();
	emit valueChanged();
	emit valueChangeEnded();
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

