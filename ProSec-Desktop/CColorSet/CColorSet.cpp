#include "CColorSet.h"

CColorSet::CColorSet()
{
    m_ColorZero = { 0, 0, 0, 0};
    m_ColorZeroPen = {m_ColorZero, 0};

    m_ColorWhite = { 255, 255, 255 };
    m_WhiteColorPen = {m_ColorWhite, 12};

    m_ColorBlacky = { 13, 11, 33 };
    m_BlackyBackgroundColorBrush = { m_ColorBlacky, Qt::SolidPattern};

    m_ColorCherry = { 232, 7, 89 };
    m_ColorCherryPen = {m_ColorCherry, 12};

    //m_PurpleBackgroundColor = { 29, 28, 44 };
    m_PurpleBackgroundColor = { 46, 44, 70 };
    m_PurplebackgroundColorBrush = { m_PurpleBackgroundColor, Qt::SolidPattern };

    m_PurpleBackgroundLightColor = { 47, 43, 70 };
    m_PurplebackgroundLightColorBrush = { m_PurpleBackgroundLightColor, Qt::SolidPattern };

    m_ColorSemiWhite = { 160, 160, 160 };
    m_SemiWhiteColorPen = { m_ColorSemiWhite, 1};
}

CColorSet::~CColorSet()
{


}
