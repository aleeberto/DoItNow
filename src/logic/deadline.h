#ifndef DEADLINE_HEADER
#define DEADLINE_HEADER
#include "datable.h"
using std::string;

class Deadline : public Datable{
    private:
        bool postponable;
        int importance;

    public:
        Deadline(string name, string note, string image, int date, bool postponable, int importance);
        virtual ~Deadline() = default;
    
        // Getters/Setters specifici Deadline
        bool getPostponable() const;
        int getImportance() const;
        void setPostponable(const bool &updPostponable);
        void setImportance(const int &updImportance);
        
        // Implementazione metodi virtuali
        QJsonObject toJsonSpecific() const override;
        void fromJsonSpecific(const QJsonObject& json) override;
        Event* clone() const override;
        
        // Visitor pattern
        void accept(EventVisitor* visitor) override;
        
        // Polimorfismo per filtri
        bool matchesCategory(const string& category) const override;
};

#endif // DEADLINE_HEADER
