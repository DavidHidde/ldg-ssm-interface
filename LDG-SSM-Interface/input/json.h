#ifndef JSON_H
#define JSON_H

#include <QJsonObject>

QPair<QJsonObject, bool> readJSONFromFile(QString file_name);

#endif // JSON_H
