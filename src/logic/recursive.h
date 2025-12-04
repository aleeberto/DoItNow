#ifndef RECURSIVE_HEADER
#define RECURSIVE_HEADER
#include "datable.h"
using std::string;

class Recursive : public Datable{
    private:
        string recurrence;

    public:
        Recursive(string name, string note, string image, int date, string recurrence);
        virtual ~Recursive() = default;
    
        // Getters/Setters specifici Anime
        string getRecurrence() const;
        void setRecurrence(const string &updRecurrence);
        
        // Implementazione metodi virtuali
        QJsonObject toJsonSpecific() const override;
        void fromJsonSpecific(const QJsonObject& json) override;
        Event* clone() const override;
        
        // Visitor pattern
        void accept(EventVisitor* visitor) override;
        
        // Polimorfismo per filtri
        bool matchesCategory(const string& category) const override;
};

#endif // RECURSIVE_HEADER