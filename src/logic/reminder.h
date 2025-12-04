#ifndef REMINDER_HEADER
#define REMINDER_HEADER
#include "event.h"
using std::string;

class Reminder : public Event{
    private:
        string longnote;

    public:
        Reminder(string name, string note, string longnote);
        virtual ~Reminder() = default;
    
        // Getters/Setters specifici Anime

        string getLongNote() const;
        void setLongNote(const string &updLongNote);
        
        // Implementazione metodi virtuali
        QJsonObject toJsonSpecific() const override;
        void fromJsonSpecific(const QJsonObject& json) override;
        Event* clone() const override;
        
        // Visitor pattern
        void accept(EventVisitor* visitor) override;
        
        // Polimorfismo per filtri
        bool matchesCategory(const string& category) const override;
};

#endif // REMINDER_HEADER