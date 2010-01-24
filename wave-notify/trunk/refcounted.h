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

#ifndef _INC_REFCOUNTED
#define _INC_REFCOUNTED

class CRefCounted
{
private:
	INT m_nRef;

public:
	CRefCounted() : m_nRef(1) { }
	virtual ~CRefCounted() {
		ASSERT(m_nRef == 0);
	}

	void AddRef() { m_nRef++; }
	void Release() {
		ASSERT(m_nRef > 0);
		if (--m_nRef == 0)
			delete this;
	}
};

#endif // _INC_REFCOUNTED
