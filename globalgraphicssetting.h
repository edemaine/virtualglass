
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
		VERY_LOW=1,
		LOW=4,
		MEDIUM=7,
		HIGH=10,	
	};

	enum Setting get();
	void set(enum Setting s);	
}

#endif

