#include "stdafx.h"
#include "include.h"

CColorScheme::CColorScheme()
{
}

CColorScheme::~CColorScheme()
{
	for (TIntHandleMapIter iter = m_vBrushes.begin(); iter != m_vBrushes.end(); iter++)
	{
		DeleteObject(iter->second);
	}
	
	for (TIntHandleMapIter iter = m_vPens.begin(); iter != m_vPens.end(); iter++)
	{
		DeleteObject(iter->second);
	}
}

HBRUSH CColorScheme::GetBrush(INT nIndex)
{
	TIntHandleMapIter pos = m_vBrushes.find(nIndex);

	if (pos == m_vBrushes.end())
	{
		HANDLE hResult = CreateSolidBrush(GetColor(nIndex));

		m_vBrushes[nIndex] = hResult;

		return (HBRUSH)hResult;
	}
	else
	{
		return (HBRUSH)pos->second;
	}
}

HPEN CColorScheme::GetPen(INT nIndex)
{
	TIntHandleMapIter pos = m_vPens.find(nIndex);

	if (pos == m_vPens.end())
	{
		HANDLE hResult = CreatePen(PS_SOLID, 1, GetColor(nIndex));

		m_vPens[nIndex] = hResult;

		return (HPEN)hResult;
	}
	else
	{
		return (HPEN)pos->second;
	}
}

HSBREF CColorScheme::RgbToHsb(COLORREF crColor)
{
	RGBQUAD crFrom = ColorRefToRGBQuad(crColor);
	HSBQUAD crTo = { 0, 0, 0 };

	INT nMinRGB = min(min(crFrom.rgbRed, crFrom.rgbGreen), crFrom.rgbBlue);
	INT nMaxRGB = max(max(crFrom.rgbRed, crFrom.rgbGreen), crFrom.rgbBlue);

	INT nDelta = nMaxRGB - nMinRGB;

	crTo.hsbBrightness = nMaxRGB;

	if (crTo.hsbBrightness != 0)
	{
		crTo.hsbSaturation = RoundByte((255 * nDelta) / nMaxRGB);
	}
	else
	{
		crTo.hsbSaturation = 0;
	}

	FLOAT dHue;

	if (crTo.hsbBrightness != 0)
	{
		if (crFrom.rgbRed == nMaxRGB)
		{
			dHue = (FLOAT)(crFrom.rgbGreen - crFrom.rgbBlue) / (FLOAT)nDelta;
		}
		else if (crFrom.rgbGreen == nMaxRGB)
		{
			dHue = 2.0f + (FLOAT)(crFrom.rgbBlue - crFrom.rgbRed) / (FLOAT)nDelta;
		}
		else
		{
			dHue = 4.0f + (FLOAT)(crFrom.rgbRed - crFrom.rgbGreen) / (FLOAT)nDelta;
		}
	}
	else
	{
		dHue = -1.0f;
	}

	dHue *= 60.0f;

	if (dHue < 0.0f)
	{
		dHue += 360.0f;
	}

	crTo.hsbHue = RoundWord((INT)((dHue / 360.0f) * 65535.0f));

	return HSBQuadToHSBRef(crTo);
}

COLORREF CColorScheme::HsbToRgb(HSBREF crColor)
{
	HSBQUAD crFrom = HSBRefToHSBQuad(crColor);
	RGBQUAD crTo = { 0, 0, 0, 0 };

	FLOAT dHue = (FLOAT)(crFrom.hsbHue) / 10922.666667f;

	INT nMaxRGB = crFrom.hsbBrightness;

	if (crFrom.hsbBrightness == 0)
	{
		crTo.rgbRed = 0;
		crTo.rgbGreen = 0;
		crTo.rgbBlue = 0;
	}
	else
	{
		INT nDelta = (crFrom.hsbSaturation * nMaxRGB) / 255;

		if (dHue > 3.0f)
		{
			crTo.rgbBlue = nMaxRGB;

			if (dHue > 4.0f)
			{
				crTo.rgbGreen = RoundByte(nMaxRGB - nDelta);
				crTo.rgbRed = RoundByte(crTo.rgbGreen + (INT)((dHue - 4.0f) * (FLOAT)(nDelta)));
			}
			else
			{
				crTo.rgbRed = RoundByte(nMaxRGB - nDelta);
				crTo.rgbGreen = RoundByte(crTo.rgbRed - (INT)((dHue - 4.0f) * (FLOAT)(nDelta)));
			}
		}
		else if (dHue > 1.0f)
		{
			crTo.rgbGreen = nMaxRGB;

			if (dHue > 2.0f)
			{
				crTo.rgbRed = RoundByte(nMaxRGB - nDelta);
				crTo.rgbBlue = RoundByte(crTo.rgbRed + (INT)((dHue - 2.0f) * (FLOAT)(nDelta)));
			}
			else
			{
				crTo.rgbBlue = RoundByte(nMaxRGB - nDelta);
				crTo.rgbRed = RoundByte(crTo.rgbBlue - (INT)((dHue - 2.0f) * (FLOAT)(nDelta)));
			}
		}
		else
		{
			crTo.rgbRed = nMaxRGB;

			if (dHue > 0.0f)
			{
				crTo.rgbBlue = RoundByte(nMaxRGB - nDelta);
				crTo.rgbGreen = RoundByte(crTo.rgbBlue + (INT)(dHue * (FLOAT)(nDelta)));
			}
			else
			{
				crTo.rgbGreen = RoundByte(nMaxRGB - nDelta);
				crTo.rgbBlue = RoundByte(crTo.rgbGreen - (INT)(dHue * (FLOAT)(nDelta)));
			}
		}
	}

	return RGBQuadToColorRef(crTo);
}

HSBREF CColorScheme::AdjustColor(HSBREF crColor, FLOAT dSaturation, FLOAT dBrightness)
{
	dSaturation = min(max(dSaturation, -1.0f), 1.0f);
	dBrightness = min(max(dBrightness, -1.0f), 1.0f);

	HSBQUAD crFrom = HSBRefToHSBQuad(crColor);
	HSBQUAD crTo = { 0, 0, 0 };

	crTo.hsbHue = crFrom.hsbHue;
	crTo.hsbSaturation = RoundByte((INT)((FLOAT)crFrom.hsbSaturation * dSaturation));
	crTo.hsbBrightness = RoundByte((INT)((FLOAT)crFrom.hsbBrightness * dBrightness));

/*
	if (dSaturation > 0.0f)
	{
		INT nDelta = 255 - crFrom.hsbSaturation;

		crTo.hsbSaturation = RoundByte(
			crFrom.hsbSaturation + (INT)((FLOAT)nDelta * dSaturation)
		);
	}
	else
	{
		crTo.hsbSaturation = RoundByte(
			(INT)((FLOAT)crFrom.hsbSaturation * (1.0f - dSaturation))
		);
	}

	if (dBrightness > 0.0f)
	{
		INT nDelta = 255 - crFrom.hsbBrightness;

		crTo.hsbBrightness = RoundByte(
			crFrom.hsbBrightness + (INT)((FLOAT)nDelta * dBrightness)
		);
	}
	else
	{
		crTo.hsbBrightness = RoundByte(
			(INT)((FLOAT)crFrom.hsbBrightness * (1.0f - dBrightness))
		);
	}
*/
	return HSBQuadToHSBRef(crTo);
}

COLORREF RGBQuadToColorRef(RGBQUAD _Quad)
{
	return RGB(_Quad.rgbRed, _Quad.rgbGreen, _Quad.rgbBlue);
}

RGBQUAD ColorRefToRGBQuad(COLORREF _Cr)
{
	RGBQUAD _Quad;

	_Quad.rgbRed = GetRValue(_Cr);
	_Quad.rgbGreen = GetGValue(_Cr);
	_Quad.rgbBlue = GetBValue(_Cr);
	_Quad.rgbReserved = 0;

	return _Quad; 
}