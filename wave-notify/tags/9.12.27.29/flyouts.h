/*
 * This file is part of Google Wave Notifier.
 *
 * Google Wave Notifier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Google Wave Notifier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Google Wave Notifier.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _INC_FLYOUTS
#define _INC_FLYOUTS

#pragma once

class CUnreadWavesFlyout : public CFlyout
{
public:
	enum CHitTestType
	{
		None,
		UnreadWave,
		Inbox
	};

private:
	class CHitTest
	{
	private:
		CUnreadWave * m_lpWave;
		CHitTestType m_nType;

	public:
		CHitTest() {
			m_lpWave = NULL;
			m_nType = None;
		}
		CHitTest(CHitTestType nType, CUnreadWave * lpWave = NULL) {
			m_lpWave = lpWave;
			m_nType = nType;
		}
		CUnreadWave * GetWave() const { return m_lpWave; }
		CHitTestType GetType() const { return m_nType; }
		BOOL Equals(CHitTest & other) const { return m_nType == other.m_nType && m_lpWave == other.m_lpWave; }
	};

private:
	CUnreadWaveCollection * m_lpWaves;
	TRectVector m_vRects;
	RECT m_rcInbox;
	CHitTest m_vLastHitTest;

public:
	CUnreadWavesFlyout(CUnreadWaveCollection * lpWaves);
	~CUnreadWavesFlyout();

protected:
	LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	LRESULT OnPaint();
	LRESULT OnMouseMove(WORD x, WORD y);
	LRESULT OnLeftButtonUp(WORD x, WORD y);

	CHitTest HitTest(POINT pt) const;
	INT CalculateRects();
	void CalculateInboxBounds();
	void PaintBottom(CDC & dc, RECT & rcBounds);
	void PaintWaves(CDC & dc, RECT & rcBounds);
	void PaintNoWaves(CDC & dc, RECT & rcBounds);
};

#endif // _INC_FLYOUTS
