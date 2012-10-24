#include <QTextStream>
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <writejson.h>

void writeJson(Json::Value root, QTextStream fileOutput){

    Json::StyledWriter writer;
    std::string outputConfig = writer.write( root );
    QString output = QString::fromStdString(outputConfig);
    fileOutput << QString(output);

}
