#ifndef APPOINTMENT_HEADER
#define APPOINTMENT_HEADER
#include "datable.h"
using std::string;

class Appointment : public Datable{
    private:
        int hour;
        int durate;

    public:
        Appointment(string name, string note, int date, int hour, int durate);
        virtual ~Appointment() = default;
    
        // Getters/Setters specifici Appointment
        
        int getHour() const;
        int getDurate() const;
        void setHour(const int &updHour);
        void setDurate(const int &updDurate);
        
        // Implementazione metodi virtuali
        QJsonObject toJsonSpecific() const override;
        void fromJsonSpecific(const QJsonObject& json) override;
        Event* clone() const override;
        
        // Visitor pattern
        void accept(EventVisitor* visitor) override;
        
        // Polimorfismo per filtri
        bool matchesCategory(const string& category) const override;
};

#endif // APPOINTMENT_HEADER