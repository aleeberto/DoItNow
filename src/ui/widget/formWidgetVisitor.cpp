#include "formWidgetVisitor.h"
#include "../../logic/appointment.h"
#include "../../logic/deadline.h"
#include "../../logic/recursive.h"
#include "../../logic/reminder.h"
#include "../../services/styleUtils.h"

using FT = FormWidgetVisitor::FieldType;

// Definizione delle configurazioni statiche
const QVector<FormWidgetVisitor::FieldConfig> FormWidgetVisitor::APPOINTMENT_FIELDS = {
    {"Nome", "Inserisci nome"},
    {"Note", "Inserisci note"},
    {"Data", "YYYYMMDD (es: 20250115)", FT::INTEGER},
    {"Ora", "HH (0-23)", FT::POSITIVE_INTEGER},
    {"Durata (min)", "Durata in minuti", FT::POSITIVE_INTEGER},
    {"Immagine", "Inserisci percorso immagine o lascia vuoto", FT::IMAGE}
};

const QVector<FormWidgetVisitor::FieldConfig> FormWidgetVisitor::DEADLINE_FIELDS = {
    {"Nome", "Inserisci nome"},
    {"Note", "Inserisci note"},
    {"Data", "YYYYMMDD (es: 20250115)", FT::INTEGER},
    {"Posticipabile", "true o false", FT::BOOLEAN},
    {"Importanza", "1-5", FT::POSITIVE_INTEGER},
    {"Immagine", "Inserisci percorso immagine o lascia vuoto", FT::IMAGE}
};

const QVector<FormWidgetVisitor::FieldConfig> FormWidgetVisitor::RECURSIVE_FIELDS = {
    {"Nome", "Inserisci nome"},
    {"Note", "Inserisci note"},
    {"Data", "YYYYMMDD (es: 20250115)", FT::INTEGER},
    {"Ricorrenza", "daily, weekly, monthly, yearly"},
    {"Immagine", "Inserisci percorso immagine o lascia vuoto", FT::IMAGE}
};

const QVector<FormWidgetVisitor::FieldConfig> FormWidgetVisitor::REMINDER_FIELDS = {
    {"Nome", "Inserisci nome"},
    {"Note breve", "Inserisci note brevi"},
    {"Note lunghe", "Inserisci note dettagliate"},
    {"Immagine", "Inserisci percorso immagine o lascia vuoto", FT::IMAGE}
};

FormWidgetVisitor::FormWidgetVisitor(QWidget* parent) 
    : parentWidget(parent), resultWidget(nullptr) {}

QWidget* FormWidgetVisitor::getResultWidget() const {
    return resultWidget;
}

QWidget* FormWidgetVisitor::createStandardForm(const QVector<FieldConfig>& fields, const QStringList& values) {
    QWidget* widget = new QWidget(parentWidget);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(StyleUtils::Dimensions::FORM_MARGIN, 
                              StyleUtils::Dimensions::FORM_MARGIN, 
                              StyleUtils::Dimensions::FORM_MARGIN, 
                              StyleUtils::Dimensions::FORM_MARGIN);
    layout->setSpacing(StyleUtils::Dimensions::SPACING_XLARGE + 2);

    for (int i = 0; i < fields.size(); ++i) {
        const FieldConfig& field = fields[i];
        QString value = (i < values.size()) ? values[i] : QString();
        
        QLabel* label = new QLabel(field.label + ":", widget);
        label->setStyleSheet(StyleUtils::getSubtitleLabelStyle());
        
        QWidget* fieldWidget = createFieldWidget(field, value);
        
        layout->addWidget(label);
        layout->addWidget(fieldWidget);
        
        if (field.type == FieldType::IMAGE) {
            layout->addSpacing(StyleUtils::Dimensions::SPACING_MEDIUM);
        }
    }
    
    layout->addStretch();
    return widget;
}

QWidget* FormWidgetVisitor::createFieldWidget(const FieldConfig& config, const QString& value) {
    if (config.type == FieldType::IMAGE) {
        return createImageFieldWidget(config.placeholder, value);
    } else if (config.type == FieldType::DATE) {
        return createDateFieldWidget(value);
    } else if (config.type == FieldType::TIME) {
        return createTimeFieldWidget(value);
    } else {
        QLineEdit* lineEdit = createStandardLineEdit(config.placeholder);
        if (!value.isEmpty()) {
            lineEdit->setText(value);
        }
        return lineEdit;
    }
}

QLineEdit* FormWidgetVisitor::createStandardLineEdit(const QString& placeholder) {
    QLineEdit* lineEdit = new QLineEdit(parentWidget);
    lineEdit->setPlaceholderText(placeholder);
    lineEdit->setStyleSheet(StyleUtils::getLineEditStyle());
    lineEdit->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);
    return lineEdit;
}

QWidget* FormWidgetVisitor::createImageFieldWidget(const QString& placeholder, const QString& value) {
    QWidget* imageContainer = new QWidget(parentWidget);
    QHBoxLayout* imageLayout = new QHBoxLayout(imageContainer);
    imageLayout->setContentsMargins(0, 0, 0, 0);
    imageLayout->setSpacing(StyleUtils::Dimensions::SPACING_MEDIUM);
    
    QLineEdit* lineEdit = createStandardLineEdit(placeholder);
    if (!value.isEmpty() && value != "default") {
        lineEdit->setText(value);
    }
    
    QPushButton* browseButton = new QPushButton(imageContainer);
    browseButton->setIcon(QIcon("../resources/icon/image.png"));
    browseButton->setIconSize(QSize(StyleUtils::Dimensions::ICON_SIZE_LARGE, 
                                     StyleUtils::Dimensions::ICON_SIZE_LARGE));
    browseButton->setToolTip("Seleziona immagine (lascia vuoto per usare quella di default)");
    browseButton->setFixedSize(StyleUtils::Dimensions::BUTTON_HEIGHT, StyleUtils::Dimensions::BUTTON_HEIGHT);
    browseButton->setStyleSheet(StyleUtils::getItemButtonStyle());
    
    QObject::connect(browseButton, &QPushButton::clicked, [lineEdit]() {
        QString fileName = QFileDialog::getOpenFileName(
            lineEdit,
            "Seleziona Immagine",
            QDir::homePath(),
            "Immagini (*.png *.jpg *.jpeg *.gif *.bmp)"
        );
        if (!fileName.isEmpty()) {
            lineEdit->setText(fileName);
        }
    });
    
    imageLayout->addWidget(lineEdit);
    imageLayout->addWidget(browseButton);
    
    imageContainer->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);
    imageContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    return imageContainer;
}

QWidget* FormWidgetVisitor::createDateFieldWidget(const QString& value) {
    QLineEdit* lineEdit = createStandardLineEdit("YYYYMMDD (es: 20250115)");
    if (!value.isEmpty()) {
        lineEdit->setText(value);
    }
    return lineEdit;
}

QWidget* FormWidgetVisitor::createTimeFieldWidget(const QString& value) {
    QLineEdit* lineEdit = createStandardLineEdit("HH (0-23)");
    if (!value.isEmpty()) {
        lineEdit->setText(value);
    }
    return lineEdit;
}

// Implementazioni dei metodi visit
void FormWidgetVisitor::visit(Appointment* appointment) {
    visitGeneric(appointment, APPOINTMENT_FIELDS);
}

void FormWidgetVisitor::visit(Deadline* deadline) {
    visitGeneric(deadline, DEADLINE_FIELDS);
}

void FormWidgetVisitor::visit(Recursive* recursive) {
    visitGeneric(recursive, RECURSIVE_FIELDS);
}

void FormWidgetVisitor::visit(Reminder* reminder) {
    visitGeneric(reminder, REMINDER_FIELDS);
}

// Metodi di estrazione dei valori
QStringList FormWidgetVisitor::extractValues(Appointment* appointment) {
    return {
        QString::fromStdString(appointment->getName()),
        QString::fromStdString(appointment->getNote()),
        QString::number(appointment->getDate()),
        QString::number(appointment->getHour()),
        QString::number(appointment->getDurate()),
        QString::fromStdString(appointment->getImage())
    };
}

QStringList FormWidgetVisitor::extractValues(Deadline* deadline) {
    return {
        QString::fromStdString(deadline->getName()),
        QString::fromStdString(deadline->getNote()),
        QString::number(deadline->getDate()),
        deadline->getPostponable() ? "true" : "false",
        QString::number(deadline->getImportance()),
        QString::fromStdString(deadline->getImage())
    };
}

QStringList FormWidgetVisitor::extractValues(Recursive* recursive) {
    return {
        QString::fromStdString(recursive->getName()),
        QString::fromStdString(recursive->getNote()),
        QString::number(recursive->getDate()),
        QString::fromStdString(recursive->getRecurrence()),
        QString::fromStdString(recursive->getImage())
    };
}

QStringList FormWidgetVisitor::extractValues(Reminder* reminder) {
    return {
        QString::fromStdString(reminder->getName()),
        QString::fromStdString(reminder->getNote()),
        QString::fromStdString(reminder->getLongNote()),
        QString::fromStdString(reminder->getImage())
    };
}