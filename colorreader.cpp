
#include <QStringList>
#include <QString>
#include "constants.h"
#include "colorreader.h"

QString lineToColorName(QString line)
{
	if (line.at(0) != '[') // fail
		return "error";
	// line1 is caneName
	line.remove(0,1);
	if (line.lastIndexOf(']') == -1) // fail
		return "error";
	line.remove(line.lastIndexOf(']'), 1);
	line = line.trimmed();
	return line;
}

QString shortColorName(QString name)
{
	return name.split(' ')[0];
}

Color lineToColorRGB(QString line)
{
	Color color;
	color.r = color.g = color.b = color.a = 0.0;

	assert(line.at(0) == '-');
	if (line.at(0) != '-') // fail
		return color; 
	line.remove(0,1);
	line = line.trimmed();
	QStringList colorData = line.split(',');

	if (colorData.length() < 4) // fail
		return color; 

	color.r = MIN(MAX(colorData[0].toInt(), 0), 255) / 255.0;
	color.g = MIN(MAX(colorData[1].toInt(), 0), 255) / 255.0;
	color.b = MIN(MAX(colorData[2].toInt(), 0), 255) / 255.0;
	color.a = MIN(MAX(colorData[3].toInt(), 0), 255) / 255.0;

	return color;
}



