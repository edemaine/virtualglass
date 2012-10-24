#ifndef WRITEJSON_H
#define WRITEJSON_H

#include <json/json.h>
class writeJson{
public:
    void writejson(Json::Value, QTextStream);
}

#endif // WRITEJSON_H
