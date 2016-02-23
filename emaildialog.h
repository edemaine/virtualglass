
#include <QString>
#include <QDialog>

class QLineEdit;
class QVBoxLayout;
class QHBoxLayout;
class QSignalMapper;
class QToolButton;
class QPushButton;

class EmailDialog : public QDialog
{
	Q_OBJECT

	public:
		EmailDialog(QDialog* parent = 0);
		~EmailDialog();
		QString address();

	private slots:
		void keyClicked(QString);
		void backspaceClicked();
		void sendClicked();

	private:
		void setupLayout();

		QPushButton* sendButton;
		QLineEdit* lineEdit;
		QVBoxLayout* layout;
		QHBoxLayout* addressLayout;
		QSignalMapper* mapper;
		QHBoxLayout* kRow0Layout;
		QHBoxLayout* kRow1Layout;
		QHBoxLayout* kRow2Layout;
		QHBoxLayout* kRow3Layout;
		QHBoxLayout* kRow4Layout;
};




