

#ifndef TWISTWIDGET_H
#define TWISTWIDGET_H

#include <QWidget>

class QDoubleSpinBox;
class QSlider;
class QPushButton;

class Cane;
class Piece;

class TwistWidget : public QWidget
{
	Q_OBJECT

	public:
		TwistWidget(Cane* c, Piece* p, unsigned int r, QWidget* parent=0);
		void setCane(Cane* c);
		void setPiece(Piece* p);

	protected:
		bool eventFilter(QObject* obj, QEvent* event);

	signals:
		void valueChanged();
		void valueChangeEnded();

	public slots:
		void updateEverything();

	private slots:
		void sliderValueChanged(int);
		void sliderChangeEnded();
		void spinValueChanged(double);

	private:
		QDoubleSpinBox* spin;
		QSlider* slider;
		Cane* cane;
		Piece* piece;
		unsigned int range;
};

#endif


