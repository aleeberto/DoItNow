#include "datable.h"
using std::string;

Datable::Datable(string name, string note, string image, int date) : 
    Event(name, note, image), date(date) {}

int Datable::getDate() const{
    return date;
}

void Datable::setDate(const int &updDate){
    date = updDate;
}

QJsonObject Datable::getDatableBaseJson() const {
    QJsonObject json;
    json["date"] = date;
    return json;
}

void Datable::setDatableBaseFromJson(const QJsonObject& json) {
    date = json["date"].toInt();
}