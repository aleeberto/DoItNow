#ifndef STYLEUTILS_HEADER
#define STYLEUTILS_HEADER

#include <QString>

class StyleUtils
{
public:
    enum class Theme {
        LIGHT,
        DARK
    };
    
    // Gestione del tema
    static void setTheme(Theme theme);
    static Theme getCurrentTheme();
    static void toggleTheme();
    
    struct Colors {
        // Light theme colors
        static constexpr const char* LIGHT_PRIMARY = "#2563eb";
        static constexpr const char* LIGHT_PRIMARY_HOVER = "#1d4ed8";
        static constexpr const char* LIGHT_PRIMARY_PRESSED = "#1e40af";
        static constexpr const char* LIGHT_SECONDARY = "#64748b";
        static constexpr const char* LIGHT_ACCENT = "#10b981";
        static constexpr const char* LIGHT_BACKGROUND = "#ffffff";
        static constexpr const char* LIGHT_SURFACE = "#f8fafc";
        static constexpr const char* LIGHT_TEXT = "#0f172a";
        static constexpr const char* LIGHT_TEXT_SECONDARY = "#64748b";
        static constexpr const char* LIGHT_BORDER = "#e2e8f0";
        static constexpr const char* LIGHT_DANGER = "#dc2626";
        static constexpr const char* LIGHT_DANGER_HOVER = "#b91c1c";
        static constexpr const char* LIGHT_DANGER_PRESSED = "#991b1b";
        static constexpr const char* LIGHT_WARNING = "#f59e0b";
        static constexpr const char* LIGHT_WARNING_HOVER = "#d97706";
        static constexpr const char* LIGHT_WARNING_PRESSED = "#b45309";
        static constexpr const char* LIGHT_DISABLED_BG = "#cbd5e1";
        static constexpr const char* LIGHT_DISABLED_TEXT = "#94a3b8";
        
        // Dark theme colors
        static constexpr const char* DARK_PRIMARY = "#3b82f6";
        static constexpr const char* DARK_PRIMARY_HOVER = "#2563eb";
        static constexpr const char* DARK_PRIMARY_PRESSED = "#1d4ed8";
        static constexpr const char* DARK_SECONDARY = "#94a3b8";
        static constexpr const char* DARK_ACCENT = "#34d399";
        static constexpr const char* DARK_BACKGROUND = "#0f172a";
        static constexpr const char* DARK_SURFACE = "#1e293b";
        static constexpr const char* DARK_TEXT = "#f1f5f9";
        static constexpr const char* DARK_TEXT_SECONDARY = "#cbd5e1";
        static constexpr const char* DARK_BORDER = "#334155";
        static constexpr const char* DARK_DANGER = "#ef4444";
        static constexpr const char* DARK_DANGER_HOVER = "#dc2626";
        static constexpr const char* DARK_DANGER_PRESSED = "#b91c1c";
        static constexpr const char* DARK_WARNING = "#fbbf24";
        static constexpr const char* DARK_WARNING_HOVER = "#f59e0b";
        static constexpr const char* DARK_WARNING_PRESSED = "#d97706";
        static constexpr const char* DARK_DISABLED_BG = "#475569";
        static constexpr const char* DARK_DISABLED_TEXT = "#64748b";
        
        // Getters dinamici
        static const char* PRIMARY();
        static const char* PRIMARY_HOVER();
        static const char* PRIMARY_PRESSED();
        static const char* SECONDARY();
        static const char* ACCENT();
        static const char* BACKGROUND();
        static const char* SURFACE();
        static const char* TEXT();
        static const char* TEXT_SECONDARY();
        static const char* BORDER();
        static const char* DANGER();
        static const char* DANGER_HOVER();
        static const char* DANGER_PRESSED();
        static const char* WARNING();
        static const char* WARNING_HOVER();
        static const char* WARNING_PRESSED();
        static const char* DISABLED_BG();
        static const char* DISABLED_TEXT();
    };

    // Dimensioni standard
    struct Dimensions {
        static constexpr int BUTTON_HEIGHT = 44;
        static constexpr int BUTTON_HEIGHT_LARGE = 48;
        static constexpr int BORDER_RADIUS = 8;
        static constexpr int BORDER_RADIUS_LARGE = 12;
        static constexpr int BORDER_RADIUS_SEARCH = 20;
        static constexpr int ICON_SIZE = 40;
        static constexpr int ICON_SIZE_LARGE = 30;
        static constexpr int PADDING_SMALL = 8;
        static constexpr int PADDING_MEDIUM = 12;
        static constexpr int PADDING_LARGE = 16;
        static constexpr int SPACING_SMALL = 4;
        static constexpr int SPACING_MEDIUM = 8;
        static constexpr int SPACING_LARGE = 12;
        static constexpr int SPACING_XLARGE = 15;
        static constexpr int MARGIN_SMALL = 2;
        static constexpr int MARGIN_MEDIUM = 5;
        static constexpr int MARGIN_LARGE = 10;
        static constexpr int LAYOUT_MARGIN = 5;
        static constexpr int FORM_MARGIN = 10;
        static constexpr int WIDGET_MARGIN = 15;
        static constexpr int SIDEBAR_WIDTH = 280;
    };

    // Font weights
    struct FontWeights {
        static constexpr int NORMAL = 400;
        static constexpr int MEDIUM = 500;
        static constexpr int SEMIBOLD = 600;
        static constexpr int BOLD = 700;
    };

    // Font sizes
    struct FontSizes {
        static constexpr int SMALL = 12;
        static constexpr int NORMAL = 14;
        static constexpr int LARGE = 16;
        static constexpr int XLARGE = 18;
    };

    // Metodi per stili dei pulsanti
    static QString getPrimaryButtonStyle();
    static QString getTopButtonStyle();
    static QString getDangerButtonStyle();
    static QString getWarningButtonStyle();
    static QString getIconButtonStyle();
    static QString getItemButtonStyle();
    
    // Metodi per stili degli input
    static QString getLineEditStyle();
    static QString getSearchBarStyle();
    static QString getComboBoxStyle();
    
    // Metodi per stili dei layout principali
    static QString getMainWindowStyle();
    static QString getSidebarStyle();
    static QString getContentAreaStyle();
    static QString getCardStyle();
    
    // Metodi per stili delle etichette
    static QString getTitleLabelStyle();
    static QString getSubtitleLabelStyle();
    static QString getBodyLabelStyle();

    // Stili specifici per categorie
    static QString getCategoryButtonStyle();
    
    // Stili per widget
    static QString getMediaCardStyle();
    static QString getScrollAreaStyle();

private:
    StyleUtils() = default;
    
    static Theme currentTheme;
    
    // Helper per creare stili
    static QString createButtonStyle(const QString& bgColor, const QString& textColor,
                                   const QString& hoverColor, const QString& pressedColor,
                                   const QString& borderColor = "none", bool withDisabled = true);
    
    static QString createInputStyle(const QString& bgColor, const QString& borderColor,
                                  const QString& textColor, const QString& focusColor,
                                  int borderRadius = Dimensions::BORDER_RADIUS);
};

#endif