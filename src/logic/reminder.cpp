#include "reminder.h"
using std::string;

Reminder::Reminder(string name, string note, string image, string longnote) :
    Event(name, note, image), longnote(longnote){}

string Reminder::getLongNote() const{
    return longnote;
}

void Reminder::setLongNote(const string &updLongNote){
    longnote = updLongNote;
}

QJsonObject Reminder::toJsonSpecific() const {
    QJsonObject json;
    json["type"] = "Reminder";
    json["longnote"] = QString::fromStdString(longnote);
    return json;
}

void Reminder::fromJsonSpecific(const QJsonObject& json) {
    longnote = json["longnote"].toString().toStdString();
}
    
Reminder* Reminder::clone() const {
    return new Reminder(getName(), getNote(), getImage(), longnote);
}

void Reminder::accept(EventVisitor* visitor) {
    if (visitor) {
        visitor->visit(this);
    }
}

bool Reminder::matchesCategory(const string& category) const {
    return category == "Tutti" || category == "Reminder";
}
