#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H
#include<QString>
#include<QMap>
#include<QFile>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonParseError>
#include<QJsonValue>
class WeatherTool
{

private:
    static QMap<QString,QString> mCityMap;

    static void initCityMap(){
        //读取文件
        QString filePath = ":/citycode.json";

        QFile file(filePath);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray Json = file.readAll();
        file.close();
        if(Json.data()==NULL)
        {
            qDebug()<<"Json file is NULL";
        }

        //解析文件
        QJsonParseError err;
        QJsonDocument Doc = QJsonDocument::fromJson(Json,&err);
        if(err.error != QJsonParseError::NoError)
        {
            return;
        }
        QJsonObject json_object = Doc.object();

        QJsonArray json_array = json_object.value("city_code").toArray();
        for(int i = 0;i<json_array.size();i++)
        {
            QJsonArray  city_array = json_array[i].toObject().value("citys").toArray();
            for(int j = 0 ;j<city_array.size();j++)
            {
               QString city = city_array[j].toObject().value("city").toString();
               QString code = city_array[j].toObject().value("code").toString();
               mCityMap.insert(city,code);
            }

        }



    }
public:
    static QString getCityCode(QString cityName){
        if(mCityMap.isEmpty())
        {
            initCityMap();
        }
        QMap<QString,QString>::iterator it = mCityMap.find(cityName);
        if(it == mCityMap.end())
        {
            it = mCityMap.find(cityName + "市");
        }
        if(it != mCityMap.end())
        {
            return it.value();
        }
        return "";
    }
};
QMap<QString,QString> WeatherTool::mCityMap;
#endif // WEATHERTOOL_H
