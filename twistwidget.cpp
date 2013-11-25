
#include <cstdio>

#include <QDoubleSpinBox>
#include <QSlider>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QEvent>

#include "cane.h"
#include "piece.h"
#include "twistwidget.h"

TwistWidget :: TwistWidget(Cane* cane, Piece* piece, unsigned int range, QWidget* parent) : QWidget(parent) 
{
	assert(cane == NULL || piece == NULL);
	this->cane = cane;
	this->piece  = piece;
	this->range = range;

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
	if (this->cane != NULL)
		connect(this->cane, SIGNAL(modified()), this, SLOT(updateEverything()));
	if (this->piece != NULL)
		connect(this->piece, SIGNAL(modified()), this, SLOT(updateEverything()));
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
	if (this->cane != NULL)
		slider->setValue(10 * this->cane->twist());
	else if (this->piece != NULL)
		slider->setValue(10 * this->piece->twist());
	slider->blockSignals(false);
	spin->blockSignals(true);
	if (this->cane != NULL)
		spin->setValue(this->cane->twist());
	else if (this->piece != NULL)
		spin->setValue(this->piece->twist());
	spin->blockSignals(false);
}

void TwistWidget :: sliderChangeEnded()
{
	emit valueChangeEnded();
}

void TwistWidget :: spinValueChanged(double v)
{
	if (this->cane != NULL)
		this->cane->setTwist(v);
	else if (this->piece != NULL)
		this->piece->setTwist(v);
	emit valueChangeEnded();
}

void TwistWidget :: sliderValueChanged(int v)
{
	if (this->cane != NULL)
		this->cane->setTwist(v * 0.1);
	else if (this->piece != NULL)
		this->piece->setTwist(v * 0.1);
}

void TwistWidget :: setCane(Cane* _cane)
{
	if (this->cane != NULL)
		disconnect(this->cane, SIGNAL(modified()), this, SLOT(updateEverything()));
	if (this->piece != NULL)
		disconnect(this->piece, SIGNAL(modified()), this, SLOT(updateEverything()));
	this->cane = _cane;
	connect(this->cane, SIGNAL(modified()), this, SLOT(updateEverything()));
	this->piece = NULL;
	updateEverything();
}

void TwistWidget :: setPiece(Piece* _piece)
{
	if (this->cane != NULL)
		disconnect(this->cane, SIGNAL(modified()), this, SLOT(updateEverything()));
	if (this->piece != NULL)
		disconnect(this->piece, SIGNAL(modified()), this, SLOT(updateEverything()));
	this->cane = NULL;
	this->piece = _piece;
	connect(this->piece, SIGNAL(modified()), this, SLOT(updateEverything()));
	updateEverything();
}

