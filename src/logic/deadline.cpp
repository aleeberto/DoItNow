#include "deadline.h"
using std::string;
    
Deadline::Deadline(string name, string note, string image, int date, bool postponable, int importance) :
    Datable(name, note, image, date), postponable(postponable), importance(importance){}

bool Deadline::getPostponable() const{
    return postponable;
}

int Deadline::getImportance() const{
    return importance;
}

void Deadline::setPostponable(const bool &updPostponable){
    postponable = updPostponable;
}

void Deadline::setImportance(const int &updImportance){
    importance = updImportance;
}

QJsonObject Deadline::toJsonSpecific() const {
    auto json = getDatableBaseJson();
    json["type"] = "Deadline";
    json["postponable"] = postponable;
    json["importance"] = importance;
    return json;
}

void Deadline::fromJsonSpecific(const QJsonObject& json) {
    setDatableBaseFromJson(json);
    postponable = json["postponable"].toBool();
    importance = json["importance"].toInt();
}

Event* Deadline::clone() const {
    return new Deadline(getName(), getNote(), getDate(), postponable, importance);
}

void Deadline::accept(EventVisitor* visitor) {
    if (visitor) {
        visitor->visit(this);
    }
}

bool Deadline::matchesCategory(const string& category) const {
    return category == "Tutti" || category == "Deadline";
}