#include "stdafx.h"
#include "include.h"
#include "theming.h"

#define HUE_ROOT	29 // orange
//#define HUE_ROOT	212 // blue
#define HUE_RED		0

CThemeScheme * CThemeScheme::m_lpSchemeActive = NULL;
CThemeScheme * CThemeScheme::m_lpSchemeActiveLight = NULL;
CThemeScheme * CThemeScheme::m_lpSchemeInactive = NULL;
CThemeScheme * CThemeScheme::m_lpSchemeInactiveLight = NULL;
CThemeScheme * CThemeScheme::m_lpSchemeRed = NULL;

HFONT CThemeScheme::m_hFont = NULL;

HSBREF CThemeScheme::GetHsbColor(INT nIndex)
{
	switch ((ThemeColor)nIndex)
	{
	case ThemeColorBorderBaseBottom:
		return AdjustColor(m_crBaseColor, 0.67f, 0.78f);

	case ThemeColorBorderBaseLeft:
		return AdjustColor(m_crBaseColor, 0.68f, 0.81f);

	case ThemeColorBorderBaseRight:
		return AdjustColor(m_crBaseColor, 0.67f, 0.78f);

	case ThemeColorBorderBaseTop:
		return AdjustColor(m_crBaseColor, 0.69f, 0.84f);

	case ThemeColorButtonBorderDarkBottom:
		return AdjustColor(m_crBaseColor, 0.71f, 0.55f);

	case ThemeColorButtonBorderDarkTop:
		return AdjustColor(m_crBaseColor, 0.61f, 0.58f);

	case ThemeColorButtonBorderLightBottom:
		return AdjustColor(m_crBaseColor, 0.52f, 0.94f);

	case ThemeColorButtonBorderLightTop:
		return AdjustColor(m_crBaseColor, 0.40f, 0.97f);

	case ThemeColorButtonDownBorderDarkBottom:
		return AdjustColor(m_crBaseColor, 0.33f, 0.78f);

	case ThemeColorButtonDownBorderDarkTop:
		return AdjustColor(m_crBaseColor, 0.25f, 0.78f);

	case ThemeColorButtonIconActive:
		return AdjustColor(m_crBaseColor, 0.00f, 1.00f);

	case ThemeColorButtonIconActiveBorder:
		return AdjustColor(m_crBaseColor, 0.00f, 0.00f);

	case ThemeColorButtonIconInactive:
		return AdjustColor(m_crBaseColor, 0.26f, 1.00f);

	case ThemeColorButtonIconInactiveBorder:
		return AdjustColor(m_crBaseColor, 0.63f, 0.35f);

	case ThemeColorClientBorder:
		return AdjustColor(m_crBaseColor, 0.13f, 0.97f);

	case ThemeColorGradientBottom:
		return AdjustColor(m_crBaseColor, 0.69f, 0.94f);

	case ThemeColorGradientButtonBottom:
		return AdjustColor(m_crBaseColor, 0.67f, 0.93f);

	case ThemeColorGradientDownTop:
		return AdjustColor(m_crBaseColor, 0.41f, 0.94f);

	case ThemeColorGradientDownButtonBottom:
		return AdjustColor(m_crBaseColor, 0.45f, 0.94f);

	case ThemeColorGradientTop:
		return AdjustColor(m_crBaseColor, 0.60f, 0.97f);

	case ThemeColorGrayBorderBottom:
		return AdjustColor(m_crBaseColor, 0.04f, 0.27f);

	case ThemeColorGrayBorderLeft:
		return AdjustColor(m_crBaseColor, 0.01f, 0.35f);

	case ThemeColorGrayBorderRight:
		return AdjustColor(m_crBaseColor, 0.04f, 0.27f);

	case ThemeColorGrayBorderTop:
		return AdjustColor(m_crBaseColor, 0.01f, 0.35f);

	case ThemeColorInnerBorderBottom:
		return AdjustColor(m_crBaseColor, 0.67f, 0.58f);

	case ThemeColorInnerBorderLeft:
		return AdjustColor(m_crBaseColor, 0.69f, 0.61f);

	case ThemeColorInnerBorderRight:
		return AdjustColor(m_crBaseColor, 0.67f, 0.58f);

	case ThemeColorInnerBorderTop:
		return AdjustColor(m_crBaseColor, 0.69f, 0.61f);

	case ThemeColorOuterBorderBottom:
		return AdjustColor(m_crBaseColor, 0.67f, 0.87f);

	case ThemeColorOuterBorderLeft:
		return AdjustColor(m_crBaseColor, 0.69f, 0.94f);

	case ThemeColorOuterBorderRight:
		return AdjustColor(m_crBaseColor, 0.67f, 0.87f);

	case ThemeColorOuterBorderTop:
		return AdjustColor(m_crBaseColor, 0.47f, 0.97f);

	default:
		FAIL("Theme color type not specified");
	}
}

CThemeScheme * CThemeScheme::GetScheme(ThemeScheme nScheme)
{
	switch (nScheme)
	{
	case ThemeSchemeActive:
		if (m_lpSchemeActive == NULL)
		{
			m_lpSchemeActive = new CThemeScheme(HSB(HUE_ROOT, 100, 100));
		}
		return m_lpSchemeActive;

	case ThemeSchemeActiveLight:
		if (m_lpSchemeActiveLight == NULL)
		{
			m_lpSchemeActiveLight = new CThemeScheme(HSB(HUE_ROOT, 62, 100));
		}
		return m_lpSchemeActiveLight;

	case ThemeSchemeInactive:
		if (m_lpSchemeInactive == NULL)
		{
			m_lpSchemeInactive = new CThemeScheme(HSB(HUE_ROOT, 62, 100));
		}
		return m_lpSchemeInactive;

	case ThemeSchemeInactiveLight:
		if (m_lpSchemeInactiveLight == NULL)
		{
			m_lpSchemeInactiveLight = new CThemeScheme(HSB(HUE_ROOT, 37, 100));
		}
		return m_lpSchemeInactiveLight;

	case ThemeSchemeRed:
		if (m_lpSchemeRed == NULL)
		{
			m_lpSchemeRed = new CThemeScheme(HSB(HUE_RED, 100, 100));
		}
		return m_lpSchemeRed;

	default:
		FAIL("Theme scheme not found");
	}
}

void CThemeScheme::Cleanup()
{
	if (m_lpSchemeActive != NULL)
	{
		delete m_lpSchemeActive;

		m_lpSchemeActive = NULL;
	}
	if (m_lpSchemeActiveLight != NULL)
	{
		delete m_lpSchemeActiveLight;

		m_lpSchemeActiveLight = NULL;
	}
	if (m_lpSchemeInactive != NULL)
	{
		delete m_lpSchemeInactive;
		
		m_lpSchemeInactive = NULL;
	}
	if (m_lpSchemeInactiveLight != NULL)
	{
		delete m_lpSchemeInactiveLight;

		m_lpSchemeInactiveLight = NULL;
	}
	if (m_lpSchemeRed != NULL)
	{
		delete m_lpSchemeRed;

		m_lpSchemeRed = NULL;
	}
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
	}
}

HFONT CThemeScheme::GetFont()
{
	if (m_hFont == NULL)
	{
		m_hFont = CreateFontIndirectEx(GetMessageBoxFont(FALSE), FW_BOLD, FALSE, FALSE);
	}

	return m_hFont;
}
