
#ifndef CANEFILE_H
#define CANEFILE_H

#include <QtGui>
#include <vector>
#include <fstream>
#include <sstream>
#include "cane.h"
#include "yaml-cpp/yaml.h"
	
using std::vector;
using std::istringstream;

vector<Cane*> loadCanesFromFile(QString fileName);
void saveCanesToFile(QString fileName, vector<Cane*> canes);
void parseCaneFromYAML(const YAML::Node& node, Cane* cane);

#endif


