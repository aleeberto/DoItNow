#include "recursive.h"
using std::string;

Recursive::Recursive(string name, string note, int date, string recurrence) :
    Datable(name, note, date), recurrence(recurrence){}

string Recursive::getRecurrence() const{
    return recurrence;
}

void Recursive::setRecurrence(const string &updRecurrence){
    recurrence = updRecurrence;
}

QJsonObject Recursive::toJsonSpecific() const {
    auto json = getDatableBaseJson();
    json["type"] = "Recursive";
    json["recurrence"] = QString::fromStdString(recurrence);
    return json;
}

void Recursive::fromJsonSpecific(const QJsonObject& json) {
    setDatableBaseFromJson(json);
    recurrence = json["recurrence"].toString().toStdString();
}

Event* Recursive::clone() const {
    return new Recursive(getName(), getDate(), getNote(), recurrence);
}

void Recursive::accept(EventVisitor* visitor) {
    if (visitor) {
        visitor->visit(this);
    }
}

bool Recursive::matchesCategory(const string& category) const {
    return category == "Tutti" || category == "Recursive";
}