#include "jsonTypeVisitor.h"
#include "../logic/appointment.h"
#include "../logic/deadline.h"
#include "../logic/recursive.h"
#include "../logic/reminder.h"

void JsonTypeVisitor::visit(Appointment* appointment) {
    Q_UNUSED(appointment)
    eventType = "Appointment";
}

void JsonTypeVisitor::visit(Deadline* deadline) {
    Q_UNUSED(deadline)
    eventType = "Deadline";
}

void JsonTypeVisitor::visit(Recursive* recursive) {
    Q_UNUSED(recursive)
    eventType = "Recursive";
}

void JsonTypeVisitor::visit(Reminder* reminder) {
    Q_UNUSED(reminder)
    eventType = "Reminder";
}

QString JsonTypeVisitor::getEventType() const {
    return eventType;
}

void JsonTypeVisitor::reset() {
    eventType.clear();
}
