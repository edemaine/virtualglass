
#include "glassfilewriter.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>

#include <QTextStream>
#include <QFile>

#include "glasscolor.h"
#include "pullplan.h"
#include "piece.h"
#include "globalglass.h"

QString GlassFileWriter :: writeJson(Json::Value root)
{
        Json::StyledWriter writer;
        std::string outputConfig = writer.write(root);
        QString output = QString::fromStdString(outputConfig);
        return output;
}

void GlassFileWriter :: save(QString filename, vector<GlassColor*> colors, vector<PullPlan*> plans, vector<Piece*> pieces)
{
        QFile saveFile(filename);
        saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream fileOutput(&saveFile);
        //read version and date from version.txt; write this into json file (root0)
        //first line; versionNo and date
        ifstream readHdl;
        //date has always 12 characters
        char date[11];
        char versionNo[4];

        readHdl.open(":/version.txt");
        readHdl.getline(versionNo,4,'\n');
        readHdl.getline(date,11,'\n');
        readHdl.close();

        Json::Value root;
        Json::Value nested_value;
        nested_value["Number"] = versionNo;
        nested_value["Date"] = date;
        root["Build information"] = nested_value;
        map<Piece*,int> pieceMap;
        map<PullPlan*,int> caneMap;
        map<GlassColor*, int> colorMap;
        vector<PullPlan*> caneVec;
        vector<GlassColor*> colorVec;

        colorMap[GlobalGlass::color()] = 0;

        buildCaneMap(&caneVec, &colorVec, &pieces);
        writeColors(&root, &colorMap, colorVec, &colors);
        writeCanes(&root, &caneMap, colorMap, caneVec, &plans);
        writePieces(&root, &pieceMap, &caneMap, colorMap, &pieces);

        fileOutput << writeJson(root);
        saveFile.close();
}

void GlassFileWriter::buildCaneTree(PullPlan* plan, vector<PullPlan*>* caneVec, vector<GlassColor*>* colorVec)
{
        if(std::find(caneVec->begin(), caneVec->end(), plan) != caneVec->end())
                return;

        for(unsigned int i = 0; i < plan->getCasingCount(); i++){
                if (std::find(colorVec->begin(), colorVec->end(), plan->getCasingColor(i)) == colorVec->end())
                        colorVec->push_back(plan->getCasingColor(i));
        }
        if(plan->subs.size() == 0)
	{
                if (std::find(caneVec->begin(), caneVec->end(), plan) == caneVec->end())
                        caneVec->push_back(plan);
        }
        else
	{
                for(unsigned int i = 0; i< plan->subs.size(); i++)
		{
                        SubpullTemplate subplan = plan->subs.at(i);
                        //go down and build tree
                        buildCaneTree(subplan.plan, caneVec, colorVec);
                        if (std::find(caneVec->begin(), caneVec->end(), subplan.plan) == caneVec->end())
                                caneVec->push_back(subplan.plan);
                }
        }
}

void GlassFileWriter::buildCaneMap(vector<PullPlan*>* caneVec, vector<GlassColor*>* colorVec, vector<Piece*>* pieces)
{
        for (unsigned int j = 0; j < pieces->size(); ++j)
	{
                Piece* piece = (*pieces)[j];

                for(unsigned int i = 0; i < piece->pickup->subs.size(); i++){
                        PullPlan* plan = piece->pickup->subs.at(i).plan;

                        if (std::find(caneVec->begin(), caneVec->end(), plan) == caneVec->end())
                        {
                                if (std::find(colorVec->begin(), colorVec->end(), piece->pickup->overlayGlassColor) == colorVec->end())
                                        colorVec->push_back(piece->pickup->overlayGlassColor);
                                if (std::find(colorVec->begin(), colorVec->end(), piece->pickup->casingGlassColor) == colorVec->end())
                                        colorVec->push_back(piece->pickup->casingGlassColor);
                                if (std::find(colorVec->begin(), colorVec->end(), piece->pickup->underlayGlassColor) == colorVec->end())
                                        colorVec->push_back(piece->pickup->underlayGlassColor);

                                buildCaneTree(plan, caneVec, colorVec);
                                if (std::find(caneVec->begin(), caneVec->end(), plan) == caneVec->end())
                                        caneVec->push_back(plan);
                        }
                }
        }
}

void GlassFileWriter::writeCanes(Json::Value *root, map<PullPlan*, int>* caneMap, map<GlassColor*, int> colorMap, vector<PullPlan*> caneVec, 
	vector<PullPlan*>* plans)
{
	// for each plan
        for (unsigned int i = 0; i < plans->size(); ++i)
        {
                PullPlan* plan = (*plans)[i];

		// if plan is in caneVec, or is a global glass 
                if(((std::find(caneVec.begin(), caneVec.end(), plan)) != caneVec.end()) 
			|| (plan == GlobalGlass::circlePlan()) || (plan==GlobalGlass::squarePlan()))
		{
                        if((plan == GlobalGlass::circlePlan()) || (plan==GlobalGlass::squarePlan()))
                                (*caneMap)[plan] = 0;
                        else
                                (*caneMap)[plan] = i+1;
                }
        }

        Json::Value *pullplan_nested = new Json::Value;
        map<PullPlan*, int>::iterator position;
        for (position = caneMap->begin(); position != caneMap->end();++position){
                PullPlan* plan = position->first;
                int i = position->second;
                //check dependency from piece to plan to save only dependent canes
                Json::Value *value5 = new Json::Value;
                *value5 = PullTemplate::enumToString(plan->getTemplateType());
                Json::Value *value7 = new Json::Value;
                *value7 = (plan->getCasingCount());
                Json::Value *value8 = new Json::Value;
                *value8 = (plan->getTwist());

                Json::Value *nested_value = new Json::Value;

                map<GlassColor*, int>::iterator iter;
                for (unsigned int k = 0; k< plan->getCasingCount(); k++){
                        Json::Value *nested_value2 = new Json::Value;

                        bool color = false;
                        for(iter=colorMap.begin(); iter != colorMap.end(); iter++){
                                (*nested_value2)["CasingColor"] = colorMap.find(plan->getCasingColor(k))->second;
                                color = true;
                        }
                        if(color==false)
                                (*nested_value2)["CasingColor"] = 0;

                        (*nested_value2)["CasingShape"] = plan->getCasingShape(k);
                        (*nested_value2)["CasingThickness"] = plan->getCasingThickness(k);
                        std::stringstream casingSstr;
                        casingSstr << k<<"_Casing";
                        string casing = casingSstr.str();
                        (*nested_value)[casing] = (*nested_value2);
                }
                Json::Value *nested_value2 = new Json::Value;
                for(unsigned int i = 0; i < plan->getParameterCount(); i++){
                        TemplateParameter pullTemplPara;
                        plan->getParameter(i, &pullTemplPara);
                        (*nested_value2)[(pullTemplPara.name)] = pullTemplPara.value;
                }
                if(plan->getParameterCount() != 0)
                        (*nested_value)["PullTemplateParameters"] = *nested_value2;

                (*nested_value)["PullTemplate"] = *value5;
                (*nested_value)["Casing count"] = *value7;
                (*nested_value)["Twists"] = *value8;

                std::stringstream *pullplannrSstr = new std::stringstream;
                *pullplannrSstr  <<i<< "_Cane";
                string pullPlanNr = (*pullplannrSstr).str();

                if(plan->subs.size() !=0)
                {
                        Json::Value *nested_value3 = new Json::Value;
                        for(unsigned int j = 0; j < plan->subs.size(); j++){
                                Json::Value *nested_value2 = new Json::Value;
                                SubpullTemplate templ = plan->subs.at(j);

                                (*nested_value2)["Shape"] = templ.shape;
                                (*nested_value2)["Diameter"] = templ.diameter;
                                (*nested_value2)["Cane"] = caneMap->find(templ.plan)->second;

                                (*nested_value2)["X"] = templ.location[0];
                                (*nested_value2)["Y"] = templ.location[1];
                                (*nested_value2)["Z"] = templ.location[2];

                                std::stringstream *Sstr = new std::stringstream;
                                        *Sstr <<j << "_Cane";
                                string name = Sstr->str();

                                (*nested_value3)[name] = (*nested_value2);
                        }
                        (*nested_value)["SubPullPlans"] = (*nested_value3);
                }
                (*pullplan_nested)[pullPlanNr] = *nested_value;
                i++;
        }
        (*root)["Canes"] = (*pullplan_nested);
}

void GlassFileWriter::writeColors(Json::Value* root, map<GlassColor*, int>* colorMap, vector<GlassColor*> colorVec, 
	vector<GlassColor*>* colors)
{
        Json::Value *color_nested = new Json::Value;
        for (unsigned int i = 0; i < colors->size(); ++i)
	{
                GlassColor* color = (*colors)[i];

                if((std::find(colorVec.begin(), colorVec.end(), color)) != colorVec.end()){

                        (*colorMap)[color] = i+1; //clear ==0

                        Json::Value *nested_value = new Json::Value;

			// this is weird b/c we use "color" as shorthand for a GlassColor 
			// in most places for convenience, and only occasionally reference 
			// the actual RGB color inside a GlassColor (like here)
                        (*nested_value)["R"] = color->getColor()->r;
                        (*nested_value)["G"] = color->getColor()->g;
                        (*nested_value)["B"] = color->getColor()->b; 
                        (*nested_value)["Alpha"] = color->getColor()->a;

                        std::stringstream *colorSstr = new std::stringstream;
                        *colorSstr  <<i+1<<"_" << (color->getName())->toStdString();
                        string colorName = colorSstr->str();

                        (*color_nested)[colorName] = (*nested_value);
                }
        }
        (*root)["Colors"] = (*color_nested);
}

void GlassFileWriter::writePieces(Json::Value* root, map<Piece*, int>* pieceMap, map<PullPlan*, int>* caneMap, 
	map<GlassColor*, int> colorMap, vector<Piece*>* pieces)
{
        Json::Value *piece_nested = new Json::Value;

	for (unsigned int i = 0; i < pieces->size(); ++i)
	{
		Piece* piece = (*pieces)[i];
		(*pieceMap)[piece] = i;
	}

        map<Piece*, int>::iterator position;
        for(position = (*pieceMap).begin(); position != (*pieceMap).end(); ++position){
                Json::Value *nested_value = new Json::Value;
                Piece* piece = position->first;
                int i = position->second;

                (*nested_value)["OverlayGlassColor"] = colorMap.find((*piece).pickup->overlayGlassColor)->second;
                (*nested_value)["UnderlayGlassColor"] = colorMap.find((*piece).pickup->underlayGlassColor)->second;
                (*nested_value)["CasingGlassColor"] = colorMap.find((*piece).pickup->casingGlassColor)->second;

                Json::Value *nested_value3 = new Json::Value;
                Json::Value *nested_value5 = new Json::Value;
                for(unsigned int i = 0; i< piece->pickup->getParameterCount(); i++){
                        TemplateParameter pickTemplPara;
                        piece->pickup->getParameter(i, &pickTemplPara);
                        (*nested_value5)[(pickTemplPara.name)] = pickTemplPara.value;
                }
                (*nested_value3)["SubPickupTemplateParameters"] =  (*nested_value5);

                Json::Value *nested_value4 = new Json::Value;
                for(unsigned int i = 0; i < piece->getParameterCount(); i++){
                        TemplateParameter pieceTemplPara;
                        piece->getParameter(i,&pieceTemplPara);
                        std::stringstream *Sstr = new std::stringstream;
                        *Sstr <<i << "_" <<pieceTemplPara.name;
                        string name = Sstr->str();
                        (*nested_value4)[name] = pieceTemplPara.value;
                }
                (*nested_value)["PieceTemplateParameters"] = (*nested_value4);

                for(unsigned int j = 0; j < piece->pickup->subs.size(); j++){
                        Json::Value *nested_value2 = new Json::Value;
                        SubpickupTemplate templ = (*piece).pickup->subs.at(j);
                        map<PullPlan*, int>::iterator iter;
                        (*nested_value2)["Cane"] = 0;
                        for(iter = caneMap->begin(); iter != caneMap->end(); iter++){
                                if(iter->first==templ.plan)
                                        (*nested_value2)["Cane"] = iter->second;
                        }

                        (*nested_value2)["Length"] = templ.length;
                        (*nested_value2)["Orientation"] = templ.orientation;
                        (*nested_value2)["Shape"] = templ.shape;
                        (*nested_value2)["Width"] = templ.width;
                        (*nested_value2)["X"] = templ.location[0];
                        (*nested_value2)["Y"] = templ.location[1];
                        (*nested_value2)["Z"] = templ.location[2];

                        std::stringstream *Sstr = new std::stringstream;
                        *Sstr <<j << "_Cane";
                        string name = Sstr->str();

                        (*nested_value3)[name] = (*nested_value2);

                        (*nested_value3)["SubPickupTemplate"] =(PickupTemplate::enumToString(piece->pickup->getTemplateType()));
                }
                (*nested_value)["SubPickups"] = (*nested_value3);
                (*nested_value)["PieceTemplate"] = PieceTemplate::enumToString(piece->getTemplateType());
                std::stringstream *Sstr = new std::stringstream;
                *Sstr << i<<"_Piece";
                string name = Sstr->str();
                (*piece_nested)[name]=(*nested_value);
        }
        (*root)["Pieces"] = (*piece_nested);
}



