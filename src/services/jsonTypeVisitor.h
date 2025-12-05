#ifndef JSONTYPEVISITOR_HEADER
#define JSONTYPEVISITOR_HEADER

#include <QString>
#include "../logic/eventVisitor.h"

class JsonTypeVisitor : public EventVisitor {
private:
    QString eventType;
    
public:
    JsonTypeVisitor() = default;
    
    void visit(Appointment* appointment) override;
    void visit(Deadline* deadline) override;
    void visit(Recursive* recursive) override;
    void visit(Reminder* reminder) override;
    
    QString getEventType() const;
    void reset();
};

#endif
