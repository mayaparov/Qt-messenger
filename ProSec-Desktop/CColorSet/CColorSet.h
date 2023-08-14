#include <QBrush>
#include <QPen>

class CColorSet
{

public:
    CColorSet();
    ~CColorSet();

    QColor m_ColorZero;

    QColor m_ColorWhite;
    QColor m_ColorCherry;
    QColor m_ColorBlacky;
    QColor m_ColorSemiWhite;

    QColor m_PurpleBackgroundColor;
    QColor m_PurpleBackgroundLightColor;

    QBrush m_PurplebackgroundColorBrush;
    QBrush m_PurplebackgroundLightColorBrush;
    QBrush m_BlackyBackgroundColorBrush;

    QPen m_WhiteColorPen;
    QPen m_ColorCherryPen;
    QPen m_ColorZeroPen;
    QPen m_SemiWhiteColorPen;

private:

};
