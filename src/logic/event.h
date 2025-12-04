#ifndef EVENT_HEADER
#define EVENT_HEADER
#include <string>
#include <vector>
#include <QJsonObject>
#include "eventVisitor.h"

using std::string;

class Event{
    private:
        string name;
        string note;

    public:
        Event(string name, string note);
        virtual ~Event() = default;

        // Getters/Setters comuni
        string getName() const;
        string getNote() const;
        void setName(const string &updName);
        void setNote(const string &updNote);
        
        // Metodi virtuali
        virtual QJsonObject toJsonSpecific() const = 0;
        virtual void fromJsonSpecific(const QJsonObject& json) = 0;
        
        // Metodo clone polimorfismo
        virtual Event* clone() const = 0;
        
        // Visitor pattern
        virtual void accept(EventVisitor* visitor) = 0;
        
        // Metodi di match per filtri e ricerche
        virtual bool matchesCategory(const string& category) const = 0;
        virtual bool matchesSearch(const string& searchText) const;
};

#endif // EVENT_HEADER