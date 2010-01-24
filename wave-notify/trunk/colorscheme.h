#ifndef _INC_COLORSCHEME
#define _INC_COLORSCHEME

#define HSBQuadToHSBRef(_Quad)	(*(HSBREF*)(&(_Quad)))
#define HSBRefToHSBQuad(_Cr)	(*(HSBQUAD*)(&(_Cr)))

COLORREF RGBQuadToColorRef(RGBQUAD _Quad);
RGBQUAD ColorRefToRGBQuad(COLORREF _Cr);

typedef DWORD HSBREF;

#define HSB(h, s, b)		((HSBREF)((((WORD)((h) * 182.044f)) << 16) | (((BYTE)((s) * 2.55f)) << 8) | ((BYTE)((b) * 2.55f))))

typedef struct tagHSBQUAD {
  BYTE    hsbBrightness;
  BYTE    hsbSaturation;
  WORD    hsbHue;
} HSBQUAD;

class CColorScheme
{
private:
	TIntHandleMap m_vBrushes;
	TIntHandleMap m_vPens;

public:
	CColorScheme();
	virtual ~CColorScheme();

	virtual HSBREF GetHsbColor(INT nIndex) = 0;
	COLORREF GetColor(INT nIndex) {
		return HsbToRgb(GetHsbColor(nIndex));
	}
	HBRUSH GetBrush(INT nIndex);
	HPEN GetPen(INT nIndex);

	static HSBREF RgbToHsb(COLORREF crColor);
	static COLORREF HsbToRgb(HSBREF crColor);
	static HSBREF AdjustColor(HSBREF crColor, FLOAT dSaturation, FLOAT dBrightness);

private:
	static BYTE RoundByte(INT nValue) {
		return min(max(nValue, 0), 255);
	}
	static WORD RoundWord(INT nValue) {
		return min(max(nValue, 0), 65535);
	}
};

#endif // _INC_COLORSCHEME
