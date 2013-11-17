
#ifndef CANEGEOMETRYTHREAD_H
#define CANEGEOMETRYTHREAD_H

#include <QThread>

class PullPlanEditorWidget;

class CaneGeometryThread : public QThread
{
	Q_OBJECT

	public:
		CaneGeometryThread(PullPlanEditorWidget* ppew);
		void run();
		PullPlanEditorWidget* ppew;

	signals:
		void finishedMesh(bool completed, unsigned int quality);
};

#endif




