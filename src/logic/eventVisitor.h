#ifndef EVENTVISITOR_HEADER
#define EVENTVISITOR_HEADER

// Forward declarations
class Recursive;
class Appointment;
class Deadline;
class Reminder;

class EventVisitor {
public:
    virtual ~EventVisitor() = default;
    
    virtual void visit(Recursive* recursive) = 0;
    virtual void visit(Appointment* appointment) = 0;
    virtual void visit(Deadline* deadline) = 0;
    virtual void visit(Reminder* reminder) = 0;
};

#endif // EVENTVISITOR_HEADER