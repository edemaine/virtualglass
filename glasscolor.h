#ifndef GLASSCOLOR_H
#define GLASSCOLOR_H

#include "primitives.h"
#include <QString>

class GlassColor 
{
	public:
		GlassColor();
		GlassColor(Color _color, QString _name);
		Color* getColor();
		void setColor(Color _color);

		QString* getName();
		void setName(QString _name);

		GlassColor* copy() const;

	private:
		Color color;
		QString name;	
		 
};

GlassColor* deep_copy(const GlassColor* _gc);


#endif

