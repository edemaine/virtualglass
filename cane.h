

#ifndef CANE_H
#define CANE_H

#include <QMetaType>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "caneshape.h"
#include "primitives.h"
#include "constants.h"
#include "yaml-cpp/yaml.h"
#include <QColor>

using std::vector;

class Cane
{
public:
	Cane();
	Cane(int type);
	int height();
	void reset();
	void setShape(CaneShape* shape);
	void shallowCopy(Cane* dest);
	void pullIntuitive(float twistFactor, float stretchFactor);
	void pullLinear(float twistFactor, float stretchFactor);
	void flatten(float rectangle_ratio, float rectangle_theta, float flatness);
        void createFlatten();
        void createPull();
	void createBundle();
	void add(Cane* addl);
	void deleteCane(int subcane);
	void moveCane(int subcane, float delta_x, float delta_y, float delta_z);
	void moveCaneTo(int subcane, float x, float y, float z);
	int leafNodes();
	Cane* deepCopy();
	void setColor(Color color);
	void setColor(float r, float g, float b, float a);
	void setColor(QColor color);
        void setColor(Color color, QString name);
        void setColor(float r, float g, float b, float a, QString name);
        void setColor(QColor color, QString name);
        void setName(QString name);
        QString toValidBrandName(QString name);
        bool matchesBrandColorName(QString name);
	Cane* getBaseCane();
	Cane* getTopBundleNode();
	std::string yamlRepresentation();
	QColor qcolor();
	QString typeName();
	QString typeAmt(int type, int index);
        QString colorName();

	int type;
	CaneShape shape;
	float amts[MAX_AMT_TYPES];
	int subcaneCount;
	Point subcaneLocations[MAX_SUBCANE_COUNT]; // z is center displacement
	Cane* subcanes[MAX_SUBCANE_COUNT];
	Color color;
        QString brandColorName;
	int libraryIndex;

public slots:
	void changeLibraryIndex(int index);

};

Q_DECLARE_METATYPE(Cane)
Q_DECLARE_METATYPE(Cane*)

#endif


