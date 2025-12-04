#include "formWidgetVisitor.h"
#include "../../logic/film.h"
#include "../../logic/serieTv.h"
#include "../../logic/anime.h"
#include "../../logic/libro.h"
#include "../../logic/manga.h"
#include "../../logic/cd.h"
#include "../../services/styleUtils.h"

using FT = FormWidgetVisitor::FieldType;

// Definizione delle configurazioni statiche
const QVector<FormWidgetVisitor::FieldConfig> FormWidgetVisitor::FILM_FIELDS = {
    {"Titolo", "Inserisci titolo"},
    {"Immagine", "Inserisci percorso immagine", FT::IMAGE},
    {"Anno", "Inserisci anno", FT::INTEGER},
    {"Regista", "Inserisci regista"},
    {"Attore Protagonista", "Inserisci attore protagonista"},
    {"Durata (min)", "Inserisci durata in minuti", FT::POSITIVE_INTEGER}
};

const QVector<FormWidgetVisitor::FieldConfig> FormWidgetVisitor::SERIES_FIELDS = {
    {"Titolo", "Inserisci titolo"},
    {"Immagine", "Inserisci percorso immagine", FT::IMAGE},
    {"Anno", "Inserisci anno", FT::INTEGER},
    {"Numero Episodi", "Inserisci numero episodi", FT::POSITIVE_INTEGER},
    {"Numero Stagioni", "Inserisci numero stagioni", FT::POSITIVE_INTEGER},
    {"Durata Media Episodio (min)", "Inserisci durata media episodio", FT::POSITIVE_INTEGER},
    {"In Corso (true/false)", "true o false", FT::BOOLEAN},
    {"Ideatore", "Inserisci ideatore"},
    {"Casa Produttrice", "Inserisci casa produttrice"}
};

const QVector<FormWidgetVisitor::FieldConfig> FormWidgetVisitor::ANIME_FIELDS = {
    {"Titolo", "Inserisci titolo"},
    {"Immagine", "Inserisci percorso immagine", FT::IMAGE},
    {"Anno", "Inserisci anno", FT::INTEGER},
    {"Numero Episodi", "Inserisci numero episodi", FT::POSITIVE_INTEGER},
    {"Numero Stagioni", "Inserisci numero stagioni", FT::POSITIVE_INTEGER},
    {"Durata Media Episodio (min)", "Inserisci durata media episodio", FT::POSITIVE_INTEGER},
    {"In Corso (true/false)", "true o false", FT::BOOLEAN},
    {"Disegnatore", "Inserisci disegnatore"},
    {"Studio Animazione", "Inserisci studio animazione"}
};

const QVector<FormWidgetVisitor::FieldConfig> FormWidgetVisitor::BOOK_FIELDS = {
    {"Titolo", "Inserisci titolo"},
    {"Immagine", "Inserisci percorso immagine", FT::IMAGE},
    {"Anno", "Inserisci anno", FT::INTEGER},
    {"Scrittore", "Inserisci scrittore"},
    {"Anno Stampa", "Inserisci anno stampa", FT::INTEGER},
    {"Casa Editrice", "Inserisci casa editrice"}
};

const QVector<FormWidgetVisitor::FieldConfig> FormWidgetVisitor::MANGA_FIELDS = {
    {"Titolo", "Inserisci titolo"},
    {"Immagine", "Inserisci percorso immagine", FT::IMAGE},
    {"Anno", "Inserisci anno", FT::INTEGER},
    {"Scrittore", "Inserisci scrittore"},
    {"Illustratore", "Inserisci illustratore"},
    {"Numero Libri", "Inserisci numero libri", FT::POSITIVE_INTEGER},
    {"Concluso (true/false)", "true o false", FT::BOOLEAN}
};

const QVector<FormWidgetVisitor::FieldConfig> FormWidgetVisitor::CD_FIELDS = {
    {"Titolo", "Inserisci titolo"},
    {"Immagine", "Inserisci percorso immagine", FT::IMAGE},
    {"Anno", "Inserisci anno", FT::INTEGER},
    {"Artista", "Inserisci artista"},
    {"Numero Tracce", "Inserisci numero tracce", FT::POSITIVE_INTEGER},
    {"Durata Media Tracce (sec)", "Inserisci durata media tracce", FT::POSITIVE_INTEGER}
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
    if (!value.isEmpty()) {
        lineEdit->setText(value);
    }
    
    QPushButton* browseButton = new QPushButton(imageContainer);
    browseButton->setIcon(QIcon("../resources/icon/image.png"));
    browseButton->setIconSize(QSize(StyleUtils::Dimensions::ICON_SIZE_LARGE, 
                                     StyleUtils::Dimensions::ICON_SIZE_LARGE));
    browseButton->setToolTip("Seleziona immagine");
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

// Implementazioni dei metodi visit usando il template
void FormWidgetVisitor::visit(Film* film) {
    visitGeneric(film, FILM_FIELDS);
}

void FormWidgetVisitor::visit(SerieTv* serieTv) {
    visitGeneric(serieTv, SERIES_FIELDS);
}

void FormWidgetVisitor::visit(Anime* anime) {
    visitGeneric(anime, ANIME_FIELDS);
}

void FormWidgetVisitor::visit(Libro* libro) {
    visitGeneric(libro, BOOK_FIELDS);
}

void FormWidgetVisitor::visit(Manga* manga) {
    visitGeneric(manga, MANGA_FIELDS);
}

void FormWidgetVisitor::visit(Cd* cd) {
    visitGeneric(cd, CD_FIELDS);
}

// Metodi di estrazione dei valori specifici per tipo
QStringList FormWidgetVisitor::extractValues(Film* film) {
    return {
        QString::fromStdString(film->getTitolo()),
        QString::fromStdString(film->getImmagine()),
        QString::number(film->getAnno()),
        QString::fromStdString(film->getRegista()),
        QString::fromStdString(film->getAttoreProtagonista()),
        QString::number(film->getDurata())
    };
}

QStringList FormWidgetVisitor::extractValues(SerieTv* serieTv) {
    return {
        QString::fromStdString(serieTv->getTitolo()),
        QString::fromStdString(serieTv->getImmagine()),
        QString::number(serieTv->getAnno()),
        QString::number(serieTv->getNumEpisodi()),
        QString::number(serieTv->getNumStagioni()),
        QString::number(serieTv->getDurataMediaEp()),
        serieTv->getInCorso() ? "true" : "false",
        QString::fromStdString(serieTv->getIdeatore()),
        QString::fromStdString(serieTv->getCasaProduttrice())
    };
}

QStringList FormWidgetVisitor::extractValues(Anime* anime) {
    return {
        QString::fromStdString(anime->getTitolo()),
        QString::fromStdString(anime->getImmagine()),
        QString::number(anime->getAnno()),
        QString::number(anime->getNumEpisodi()),
        QString::number(anime->getNumStagioni()),
        QString::number(anime->getDurataMediaEp()),
        anime->getInCorso() ? "true" : "false",
        QString::fromStdString(anime->getDisegnatore()),
        QString::fromStdString(anime->getStudioAnimazione())
    };
}

QStringList FormWidgetVisitor::extractValues(Libro* libro) {
    return {
        QString::fromStdString(libro->getTitolo()),
        QString::fromStdString(libro->getImmagine()),
        QString::number(libro->getAnno()),
        QString::fromStdString(libro->getScrittore()),
        QString::number(libro->getAnnoStampa()),
        QString::fromStdString(libro->getCasaEditrice())
    };
}

QStringList FormWidgetVisitor::extractValues(Manga* manga) {
    return {
        QString::fromStdString(manga->getTitolo()),
        QString::fromStdString(manga->getImmagine()),
        QString::number(manga->getAnno()),
        QString::fromStdString(manga->getScrittore()),
        QString::fromStdString(manga->getIllustratore()),
        QString::number(manga->getNumLibri()),
        manga->getConcluso() ? "true" : "false"
    };
}

QStringList FormWidgetVisitor::extractValues(Cd* cd) {
    return {
        QString::fromStdString(cd->getTitolo()),
        QString::fromStdString(cd->getImmagine()),
        QString::number(cd->getAnno()),
        QString::fromStdString(cd->getArtista()),
        QString::number(cd->getNumTracce()),
        QString::number(cd->getDurataMedTracce())
    };
}