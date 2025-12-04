#ifndef DATABLE_HEADER
#define DATABLE_HEADER
#include "event.h"
using std::string;

class Datable : public Event{
    private:
        int date;
    
    public:
        Datable(string name, string note, int date);
        virtual ~Datable() = default;

        // Getters/Setters comuni agli eventi con data
        int getDate() const;
        void setDate(const int &updDate);
        
    protected:
        // Metodi helper per le classi derivate
        QJsonObject getDatableBaseJson() const;
        void setDatableBaseFromJson(const QJsonObject& json);
};

#endif // DATABLE_HEADER