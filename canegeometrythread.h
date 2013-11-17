
#ifndef CANEGEOMETRYTHREAD_H
#define CANEGEOMETRYTHREAD_H

#include <QThread>

class CaneEditorWidget;

class CaneGeometryThread : public QThread
{
	Q_OBJECT

	public:
		CaneGeometryThread(CaneEditorWidget* ppew);
		void run();
		CaneEditorWidget* ppew;

	signals:
		void finishedMesh(bool completed, unsigned int quality);
};

#endif




