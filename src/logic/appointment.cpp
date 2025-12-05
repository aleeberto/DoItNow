#include "appointment.h"
using std::string;

Appointment::Appointment(string name, string note, string image, int date, int hour, int minute, int durate) :
    Datable(name, note, image, date), hour(hour), minute(minute), durate(durate) {}

int Appointment::getHour() const{
    return hour;
}

int Appointment::getMinute() const{
    return minute;
}

int Appointment::getDurate() const{
    return durate;
}

void Appointment::setHour(const int &updHour){
    hour = updHour;
}

void Appointment::setMinute(const int &updMinute){
    minute = updMinute;
}

void Appointment::setDurate(const int &updDurate){
    durate = updDurate;
}

QJsonObject Appointment::toJsonSpecific() const {
    auto json = getDatableBaseJson();
    json["type"] = "Appointment";
    json["hour"] = hour;
    json["minute"] = minute;
    json["durate"] = durate;
    return json;
}

void Appointment::fromJsonSpecific(const QJsonObject& json) {
    setDatableBaseFromJson(json);
    hour = json["hour"].toInt();
    minute = json.contains("minute") ? json["minute"].toInt() : 0;  // Retrocompatibilità
    durate = json["durate"].toInt();
}

Event* Appointment::clone() const {
    return new Appointment(getName(), getNote(), getImage(), getDate(), hour, minute, durate);
}

void Appointment::accept(EventVisitor* visitor) {
    if (visitor) {
        visitor->visit(this);
    }
}

bool Appointment::matchesCategory(const string& category) const {
    return category == "Tutti" || category == "Appointment";
}