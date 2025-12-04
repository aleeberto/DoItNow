#ifndef MEDIAWIDGETVISITOR_HEADER
#define MEDIAWIDGETVISITOR_HEADER

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <functional>
#include "../../logic/mediaVisitor.h"
#include "../../services/uiService.h"

class MediaWidgetVisitor : public MediaVisitor {
private:
    QWidget* parentWidget;
    UIService* uiService;
    QWidget* resultWidget;
    
public:
    explicit MediaWidgetVisitor(QWidget* parent, UIService* ui);
    
    void visit(Film* film) override;
    void visit(SerieTv* serieTv) override;
    void visit(Anime* anime) override;
    void visit(Libro* libro) override;
    void visit(Manga* manga) override;
    void visit(Cd* cd) override;
    
    QWidget* getResultWidget() const;
    
private:
    // Factory methods per ridurre duplicazione
    QWidget* createBaseWidget(Media* media);
    QHBoxLayout* createMainLayout(QWidget* widget);
    QLabel* createImageLabel(Media* media);
    QWidget* createDetailsWidget();
    QWidget* createButtonsWidget(Media* media);
    
    // Helper methods per elementi comuni
    void addCommonInfo(QVBoxLayout* layout, Media* media);
    QLabel* createTypeLabel(const QString& type);
    QLabel* createDetailLabel(const QString& text);
    QWidget* createSeriesInfoWidget(int numStagioni, int numEpisodi);
    void addActionButtons(QVBoxLayout* layout, Media* media);
    
    // Template per la struttura comune dei widget
    template<typename MediaType>
    void createMediaWidget(MediaType* media, const QString& typeName,
                          const std::function<void(QVBoxLayout*, MediaType*)>& addSpecificDetails);
};

#endif // MEDIAWIDGETVISITOR_HEADER