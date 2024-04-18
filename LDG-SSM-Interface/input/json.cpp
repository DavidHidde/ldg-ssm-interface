#include "input/json.h"

#include <QFile>
#include <QJsonDocument>

/**
 * Read a JSON object from a file.
 *
 * @brief readJSONFromFile
 * @param file_name
 * @return [object, success]. Success is false if an error occured while reading.
 */
QPair<QJsonObject, bool> readJSONFromFile(QString file_name)
{
    QFile file_obj(file_name);
    if (!file_obj.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open " << file_name;
    }

    QTextStream file_text(&file_obj);
    QString json_string;
    json_string = file_text.readAll();
    QByteArray json_bytes = json_string.toLocal8Bit();
    auto json_doc = QJsonDocument::fromJson(json_bytes);

    if (json_doc.isNull()) {
        qDebug() << "Failed to create JSON doc for file" << file_name;
        return { {}, false };
    }
    if (!json_doc.isObject()) {
        qDebug() << "JSON is not an object for file" << file_name;
        return { {}, false };
    }

    QJsonObject json_obj = json_doc.object();

    if (json_obj.isEmpty()) {
        qDebug() << "JSON object is empty for file" << file_name;
        return { {}, false };
    }

    return { json_obj, true };
}
