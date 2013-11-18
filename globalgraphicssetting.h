
#ifndef GLOBALGRAPHICSETTING_H
#define GLOBALGRAPHICSETTING_H 

namespace GlobalGraphicsSetting
{
	// enums are designed to match what they mean for the numeric 
	// quality values used in MeshInternal and generateMesh()
	// quality values go from 1 to 10
	// what these mean in absolute terms are determined in mesh.cpp
	// approximately: 1 is bad, 10 is good, 5 is ok.
	enum Setting
	{
		BLANK=0,
		VERY_LOW=1,
		MEDIUM=5,
		HIGH=10,
		VERY_HIGH=15,
	};
}

#endif

