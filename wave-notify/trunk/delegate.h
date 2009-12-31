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

#ifndef _INC_DELEGATE
#define _INC_DELEGATE

#pragma once

namespace Internal {

	template<typename T>
	class CTypedDelegate;

	class CDelegate
	{
	private:
		INT m_nRef;

	protected:
		CDelegate() { m_nRef = 1; }

	public:
		void AddRef() { m_nRef++; }
		void RemoveRef() { if (--m_nRef == 0) delete this; }

		template<typename T>
		static CDelegate * Create(T * lpInstance, void (T::*lpMethod)()) {
			return new CTypedDelegate<T>(lpInstance, lpMethod);
		}

		virtual void Invoke() = 0;
		virtual BOOL Equals(CDelegate * lpOther) = 0;
	};

	template<typename T>
	class CTypedDelegate : public CDelegate
	{
	private:
		T * m_lpInstance;
		void (T::*m_lpMethod)();

	private:
		CTypedDelegate(T * lpInstance, void (T::*lpMethod)()) {
			m_lpInstance = lpInstance;
			m_lpMethod = lpMethod;
		}

	public:
		void Invoke() { (m_lpInstance->*m_lpMethod)(); }

	protected:
		BOOL Equals(CDelegate * lpOther) {
			return (
				m_lpInstance == ((CTypedDelegate<T> *)lpOther)->m_lpInstance &&
				m_lpMethod == ((CTypedDelegate<T> *)lpOther)->m_lpMethod
			);
		}

	private:
		friend class CDelegate;
	};

}

class Delegate
{
private:
	Internal::CDelegate * m_lpDelegate;

public:
	Delegate(Internal::CDelegate * lpDelegate) {
		m_lpDelegate = lpDelegate;
	}
	Delegate(const Delegate & vDelegate) {
		m_lpDelegate = vDelegate.m_lpDelegate;
		m_lpDelegate->AddRef();
	}

	virtual ~Delegate() {
		m_lpDelegate->RemoveRef();
	}

	void Invoke() { m_lpDelegate->Invoke(); }

	void operator()() { Invoke(); }
	bool operator==(const Delegate & _Other) const { return m_lpDelegate->Equals(_Other.m_lpDelegate) != FALSE; }
	bool operator!=(const Delegate & _Other) const { return !(*this == _Other); }
	Delegate & operator=(const Delegate & _Other) {
		m_lpDelegate->RemoveRef();
		m_lpDelegate = _Other.m_lpDelegate;
		m_lpDelegate->AddRef();
		return *this;
	}
};

typedef vector<Delegate> TDelegateVector;
typedef TDelegateVector::iterator TDelegateVectorIter;
typedef TDelegateVector::const_iterator TDelegateVectorConstIter;

class Event
{
private:
	TDelegateVector m_vDelegates;

public:
	Event() { }
	virtual ~Event() { }

	void operator+=(const Delegate & vDelegate) {
		if (FindDelegate(vDelegate) == m_vDelegates.end())
			m_vDelegates.push_back(vDelegate);
	}
	void operator-=(const Delegate & vDelegate) {
		TDelegateVectorIter pos = FindDelegate(vDelegate);
		if (pos != m_vDelegates.end())
			m_vDelegates.erase(pos);
	}
	bool operator==(LPVOID * lpOther) const {
		ASSERT(lpOther == NULL);
		return m_vDelegates.empty();
	}
	bool operator!=(LPVOID * lpOther) const { return !(*this == lpOther); }
	void operator()() {
		ASSERT(!m_vDelegates.empty());
		for (TDelegateVectorIter iter = m_vDelegates.begin(); iter != m_vDelegates.end(); iter++)
			(*iter)();
	}

private:
	TDelegateVectorIter FindDelegate(Delegate vDelegate) {
		for (TDelegateVectorIter iter = m_vDelegates.begin(); iter != m_vDelegates.end(); iter++) {
			if (*iter == vDelegate)
				return iter;
		}
		return m_vDelegates.end();
	}

};

template<typename T>
Delegate AddressOf(T * lpInstance, void (T::*lpMethod)()) {
	return Delegate(Internal::CDelegate::Create<T>(lpInstance, lpMethod));
}

#endif // _INC_DELEGATE
