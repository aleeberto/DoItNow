#include "mediaWidgetVisitor.h"
#include "../../logic/film.h"
#include "../../logic/serieTv.h"
#include "../../logic/anime.h"
#include "../../logic/libro.h"
#include "../../logic/manga.h"
#include "../../logic/cd.h"
#include "../../services/styleUtils.h"

MediaWidgetVisitor::MediaWidgetVisitor(QWidget* parent, UIService* ui) 
    : parentWidget(parent), uiService(ui), resultWidget(nullptr) {}

QWidget* MediaWidgetVisitor::getResultWidget() const {
    return resultWidget;
}

QWidget* MediaWidgetVisitor::createBaseWidget(Media* media) {
    Q_UNUSED(media)
    QWidget* widget = new QWidget(parentWidget);
    widget->setStyleSheet(StyleUtils::getMediaCardStyle());
    return widget;
}

QHBoxLayout* MediaWidgetVisitor::createMainLayout(QWidget* widget) {
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(StyleUtils::Dimensions::MARGIN_LARGE, 
                              StyleUtils::Dimensions::MARGIN_LARGE, 
                              StyleUtils::Dimensions::MARGIN_LARGE, 
                              StyleUtils::Dimensions::MARGIN_LARGE);
    return layout;
}

QLabel* MediaWidgetVisitor::createImageLabel(Media* media) {
    QLabel* imgLabel = new QLabel();
    imgLabel->setPixmap(uiService->loadMediaImage(media->getImmagine()));
    imgLabel->setAlignment(Qt::AlignTop);
    return imgLabel;
}

QWidget* MediaWidgetVisitor::createDetailsWidget() {
    QWidget* detailsWidget = new QWidget();
    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsWidget);
    detailsLayout->setAlignment(Qt::AlignTop);
    return detailsWidget;
}

QWidget* MediaWidgetVisitor::createButtonsWidget(Media* media) {
    QWidget* buttonsWidget = new QWidget();
    QVBoxLayout* buttonsLayout = new QVBoxLayout(buttonsWidget);
    buttonsLayout->setAlignment(Qt::AlignTop);
    buttonsLayout->setSpacing(StyleUtils::Dimensions::MARGIN_LARGE);

    addActionButtons(buttonsLayout, media);
    buttonsLayout->addStretch();
    
    return buttonsWidget;
}

void MediaWidgetVisitor::addCommonInfo(QVBoxLayout* layout, Media* media) {
    QLabel* titleLabel = new QLabel(uiService->formatMediaTitle(media));
    titleLabel->setStyleSheet(StyleUtils::getTitleLabelStyle());
    
    QLabel* yearLabel = new QLabel(uiService->formatMediaYear(media));
    yearLabel->setStyleSheet(StyleUtils::getBodyLabelStyle());
    
    layout->addWidget(titleLabel);
    layout->addWidget(yearLabel);
}

QLabel* MediaWidgetVisitor::createTypeLabel(const QString& type) {
    QLabel* typeLabel = new QLabel("Tipo: " + type);
    typeLabel->setStyleSheet(StyleUtils::getSubtitleLabelStyle());
    return typeLabel;
}

QLabel* MediaWidgetVisitor::createDetailLabel(const QString& text) {
    QLabel* label = new QLabel(text);
    label->setStyleSheet(StyleUtils::getBodyLabelStyle());
    return label;
}

QWidget* MediaWidgetVisitor::createSeriesInfoWidget(int numStagioni, int numEpisodi) {
    QWidget* seriesInfoWidget = new QWidget();
    QHBoxLayout* seriesInfoLayout = new QHBoxLayout(seriesInfoWidget);
    seriesInfoLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* stagioniLabel = createDetailLabel("Stagioni: " + QString::number(numStagioni));
    QLabel* episodiLabel = createDetailLabel("Episodi: " + QString::number(numEpisodi));
    
    seriesInfoLayout->addWidget(stagioniLabel);
    seriesInfoLayout->addWidget(episodiLabel);
    seriesInfoLayout->addStretch();
    
    return seriesInfoWidget;
}

void MediaWidgetVisitor::addActionButtons(QVBoxLayout* layout, Media* media) {
    QPushButton* editButton = new QPushButton();
    editButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);
    editButton->setToolTip("Modifica questo elemento");
    editButton->setIcon(QIcon("../resources/icon/edit.png"));
    editButton->setStyleSheet(StyleUtils::getItemButtonStyle());

    QPushButton* deleteButton = new QPushButton();
    deleteButton->setFixedHeight(StyleUtils::Dimensions::BUTTON_HEIGHT);
    deleteButton->setToolTip("Elimina questo elemento");
    deleteButton->setIcon(QIcon("../resources/icon/delete.png"));
    deleteButton->setStyleSheet(StyleUtils::getItemButtonStyle());

    editButton->setProperty("media", QVariant::fromValue(static_cast<void*>(media)));
    deleteButton->setProperty("media", QVariant::fromValue(static_cast<void*>(media)));
    editButton->setObjectName("editButton");
    deleteButton->setObjectName("deleteButton");

    layout->addWidget(editButton);
    layout->addWidget(deleteButton);
}

// Template
template<typename MediaType>
void MediaWidgetVisitor::createMediaWidget(MediaType* media, const QString& typeName,
                                          const std::function<void(QVBoxLayout*, MediaType*)>& addSpecificDetails) {
    resultWidget = createBaseWidget(media);
    QHBoxLayout* mainLayout = createMainLayout(resultWidget);

    QLabel* imgLabel = createImageLabel(media);
    QWidget* detailsWidget = createDetailsWidget();
    QVBoxLayout* detailsLayout = static_cast<QVBoxLayout*>(detailsWidget->layout());

    addCommonInfo(detailsLayout, media);
    detailsLayout->addWidget(createTypeLabel(typeName));
    
    addSpecificDetails(detailsLayout, media);
    detailsLayout->addStretch();

    QWidget* buttonsWidget = createButtonsWidget(media);

    mainLayout->addWidget(imgLabel);
    mainLayout->addWidget(detailsWidget, 1);
    mainLayout->addWidget(buttonsWidget);
}

void MediaWidgetVisitor::visit(Film* film) {
    createMediaWidget<Film>(film, "Film", [this](QVBoxLayout* layout, Film* f) {
        layout->addWidget(createDetailLabel("Regista: " + QString::fromStdString(f->getRegista())));
        layout->addWidget(createDetailLabel("Protagonista: " + QString::fromStdString(f->getAttoreProtagonista())));
        layout->addWidget(createDetailLabel("Durata: " + QString::number(f->getDurata()) + " min"));
    });
}

void MediaWidgetVisitor::visit(SerieTv* serieTv) {
    createMediaWidget<SerieTv>(serieTv, "Serie TV", [this](QVBoxLayout* layout, SerieTv* s) {
        layout->addWidget(createSeriesInfoWidget(s->getNumStagioni(), s->getNumEpisodi()));
        layout->addWidget(createDetailLabel("Durata episodio: " + QString::number(s->getDurataMediaEp()) + " min"));
        layout->addWidget(createDetailLabel("Stato: " + QString(s->getInCorso() ? "In corso" : "Conclusa")));
        layout->addWidget(createDetailLabel("Ideatore: " + QString::fromStdString(s->getIdeatore())));
        layout->addWidget(createDetailLabel("Casa produttrice: " + QString::fromStdString(s->getCasaProduttrice())));
    });
}

void MediaWidgetVisitor::visit(Anime* anime) {
    createMediaWidget<Anime>(anime, "Anime", [this](QVBoxLayout* layout, Anime* a) {
        layout->addWidget(createSeriesInfoWidget(a->getNumStagioni(), a->getNumEpisodi()));
        layout->addWidget(createDetailLabel("Durata episodio: " + QString::number(a->getDurataMediaEp()) + " min"));
        layout->addWidget(createDetailLabel("Stato: " + QString(a->getInCorso() ? "In corso" : "Concluso")));
        layout->addWidget(createDetailLabel("Disegnatore: " + QString::fromStdString(a->getDisegnatore())));
        layout->addWidget(createDetailLabel("Studio: " + QString::fromStdString(a->getStudioAnimazione())));
    });
}

void MediaWidgetVisitor::visit(Libro* libro) {
    createMediaWidget<Libro>(libro, "Libro", [this](QVBoxLayout* layout, Libro* l) {
        layout->addWidget(createDetailLabel("Scrittore: " + QString::fromStdString(l->getScrittore())));
        layout->addWidget(createDetailLabel("Anno di stampa: " + QString::number(l->getAnnoStampa())));
        layout->addWidget(createDetailLabel("Casa Editrice: " + QString::fromStdString(l->getCasaEditrice())));
    });
}

void MediaWidgetVisitor::visit(Manga* manga) {
    createMediaWidget<Manga>(manga, "Manga", [this](QVBoxLayout* layout, Manga* m) {
        layout->addWidget(createDetailLabel("Scrittore: " + QString::fromStdString(m->getScrittore())));
        layout->addWidget(createDetailLabel("Illustratore: " + QString::fromStdString(m->getIllustratore())));
        layout->addWidget(createDetailLabel("Volumi: " + QString::number(m->getNumLibri())));
        layout->addWidget(createDetailLabel("Stato: " + QString(m->getConcluso() ? "Concluso" : "In corso")));
    });
}

void MediaWidgetVisitor::visit(Cd* cd) {
    createMediaWidget<Cd>(cd, "CD", [this](QVBoxLayout* layout, Cd* c) {
        layout->addWidget(createDetailLabel("Artista: " + QString::fromStdString(c->getArtista())));
        layout->addWidget(createDetailLabel("Tracce: " + QString::number(c->getNumTracce())));
        layout->addWidget(createDetailLabel("Durata media: " + QString::number(c->getDurataMedTracce()) + " sec"));
    });
}