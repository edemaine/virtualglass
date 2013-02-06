
#ifndef PIECEGEOMETRYTHREAD_H
#define PIECEGEOMETRYTHREAD_H

#include <QThread>

class PieceEditorWidget;

class PieceGeometryThread : public QThread
{
	Q_OBJECT

	public:
		PieceGeometryThread(PieceEditorWidget* pew);
		void run();
		PieceEditorWidget* pew;

	signals:
		void finishedMesh();
};

#endif




