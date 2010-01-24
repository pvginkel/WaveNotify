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

// Delegate

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

		virtual void Invoke() = 0;
		virtual BOOL Equals(CDelegate * lpOther) = 0;
	};

	template<typename T>
	class CTypedDelegate : public CDelegate
	{
	private:
		T * m_lpInstance;
		void (T::*m_lpMethod)();

	public:
		CTypedDelegate(T * lpInstance, void (T::*lpMethod)()) {
			ASSERT(lpInstance != NULL);

			m_lpInstance = lpInstance;
			m_lpMethod = lpMethod;
		}

	public:
		void Invoke() { (m_lpInstance->*m_lpMethod)(); }

	protected:
		BOOL Equals(CDelegate * lpOther) {
			ASSERT(lpOther != NULL);

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
		ASSERT(lpDelegate != NULL);

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
	bool operator==(const Delegate & _Other) const {
		return m_lpDelegate->Equals(_Other.m_lpDelegate) != FALSE;
	}
	bool operator!=(const Delegate & _Other) const {
		return !(*this == _Other);
	}
	Delegate & operator=(const Delegate & _Other) {
		m_lpDelegate->RemoveRef();
		m_lpDelegate = _Other.m_lpDelegate;
		m_lpDelegate->AddRef();
		return *this;
	}
};

class Event
{
private:
	typedef vector<Delegate> TDelegateVector;
	typedef TDelegateVector::iterator TDelegateVectorIter;
	typedef TDelegateVector::const_iterator TDelegateVectorConstIter;

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
		// This may only be used for the == NULL and != NULL
		// constructions.
		ASSERT(lpOther == NULL);
		return m_vDelegates.empty();
	}
	bool operator!=(LPVOID * lpOther) const { return !(*this == lpOther); }
	void operator()() {
		ASSERT(!m_vDelegates.empty());
		
		// Delegates vector is copied here because there are some times
		// that delete the this inside the timer.
		TDelegateVector vCopy(m_vDelegates.begin(), m_vDelegates.end());

		for (TDelegateVectorIter iter = vCopy.begin(); iter != vCopy.end(); iter++)
			iter->Invoke();
	}

private:
	TDelegateVectorIter FindDelegate(Delegate vDelegate) {
		return find(m_vDelegates.begin(), m_vDelegates.end(), vDelegate);
	}

};

template<typename T>
Delegate AddressOf(T * lpInstance, void (T::*lpMethod)()) {
	ASSERT(lpInstance != NULL);

	return Delegate(new Internal::CTypedDelegate<T>(lpInstance, lpMethod));
}

// DelegateT

namespace Internal {

	template<typename T, typename TP>
	class CTypedDelegateT;

	template<typename TP>
	class CDelegateT
	{
	private:
		INT m_nRef;

	protected:
		CDelegateT() { m_nRef = 1; }

	public:
		void AddRef() { m_nRef++; }
		void RemoveRef() { if (--m_nRef == 0) delete this; }

		virtual void Invoke(TP _Param) = 0;
		virtual BOOL Equals(CDelegateT<TP> * lpOther) = 0;
	};

	template<typename T, typename TP>
	class CTypedDelegateT : public CDelegateT<TP>
	{
	private:
		T * m_lpInstance;
		void (T::*m_lpMethod)(TP);

	public:
		CTypedDelegateT(T * lpInstance, void (T::*lpMethod)(TP)) {
			ASSERT(lpInstance != NULL);

			m_lpInstance = lpInstance;
			m_lpMethod = lpMethod;
		}

	public:
		void Invoke(TP _Param) { (m_lpInstance->*m_lpMethod)(_Param); }

	protected:
		BOOL Equals(CDelegateT<TP> * lpOther) {
			ASSERT(lpOther != NULL);

			return (
				m_lpInstance == ((CTypedDelegateT<T, TP> *)lpOther)->m_lpInstance &&
				m_lpMethod == ((CTypedDelegateT<T, TP> *)lpOther)->m_lpMethod
			);
		}

	private:
		friend class CDelegateT<TP>;
	};

}

template<typename TP>
class DelegateT
{
private:
	Internal::CDelegateT<TP> * m_lpDelegate;

public:
	DelegateT(Internal::CDelegateT<TP> * lpDelegate) {
		ASSERT(lpDelegate != NULL);

		m_lpDelegate = lpDelegate;
	}
	DelegateT(const DelegateT<TP> & vDelegate) {
		m_lpDelegate = vDelegate.m_lpDelegate;
		m_lpDelegate->AddRef();
	}

	virtual ~DelegateT() {
		m_lpDelegate->RemoveRef();
	}

	void Invoke(TP _Param) { m_lpDelegate->Invoke(_Param); }

	void operator()() { Invoke(); }
	bool operator==(const DelegateT<TP> & _Other) const { return m_lpDelegate->Equals(_Other.m_lpDelegate) != FALSE; }
	bool operator!=(const DelegateT<TP> & _Other) const { return !(*this == _Other); }
	DelegateT<TP> & operator=(const DelegateT<TP> & _Other) {
		m_lpDelegate->RemoveRef();
		m_lpDelegate = _Other.m_lpDelegate;
		m_lpDelegate->AddRef();
		return *this;
	}
};

template<typename TP>
class EventT : public Event
{
private:
	typedef vector< DelegateT<TP> > TDelegateVector;
	typename typedef TDelegateVector::iterator TDelegateVectorIter;
	typename typedef TDelegateVector::const_iterator TDelegateVectorConstIter;

private:
	TDelegateVector m_vDelegates;

public:
	EventT() { }
	virtual ~EventT() { }

	void operator+=(const DelegateT<TP> & vDelegate) {
		if (FindDelegate(vDelegate) == m_vDelegates.end())
			m_vDelegates.push_back(vDelegate);
	}
	void operator-=(const DelegateT<TP> & vDelegate) {
		TDelegateVectorIter pos = FindDelegate(vDelegate);
		if (pos != m_vDelegates.end())
			m_vDelegates.erase(pos);
	}
	bool operator==(LPVOID * lpOther) const {
		ASSERT(lpOther == NULL);

		// This may only be used for the == NULL and != NULL
		// constructions.
		ASSERT(lpOther == NULL);
		return m_vDelegates.empty();
	}
	bool operator!=(LPVOID * lpOther) const { return !(*this == lpOther); }
	void operator()(TP _Param) {
		ASSERT(!m_vDelegates.empty());
		
		// Delegates vector is copied here because there are some times
		// that delete the this inside the timer.
		TDelegateVector vCopy(m_vDelegates.begin(), m_vDelegates.end());

		for (TDelegateVectorIter iter = vCopy.begin(); iter != vCopy.end(); iter++)
			iter->Invoke(_Param);
	}

private:
	TDelegateVectorIter FindDelegate(DelegateT<TP> vDelegate) {
		for (TDelegateVectorIter iter = m_vDelegates.begin(); iter != m_vDelegates.end(); iter++) {
			if (*iter == vDelegate)
				return iter;
		}
		return m_vDelegates.end();
	}

};

template<typename T, typename TP>
DelegateT<TP> AddressOfT(T * lpInstance, void (T::*lpMethod)(TP)) {
	ASSERT(lpInstance != NULL);

	return DelegateT<TP>(new Internal::CTypedDelegateT<T, TP>(lpInstance, lpMethod));
}

// DelegateT2

namespace Internal {

	template<typename T, typename TP1, typename TP2>
	class CTypedDelegateT2;

	template<typename TP1, typename TP2>
	class CDelegateT2
	{
	private:
		INT m_nRef;

	protected:
		CDelegateT2() { m_nRef = 1; }

	public:
		void AddRef() { m_nRef++; }
		void RemoveRef() { if (--m_nRef == 0) delete this; }

		virtual void Invoke(TP1 _Param1, TP2 _Param2) = 0;
		virtual BOOL Equals(CDelegateT2<TP1, TP2> * lpOther) = 0;
	};

	template<typename T, typename TP1, typename TP2>
	class CTypedDelegateT2 : public CDelegateT2<TP1, TP2>
	{
	private:
		T * m_lpInstance;
		void (T::*m_lpMethod)(TP1, TP2);

	public:
		CTypedDelegateT2(T * lpInstance, void (T::*lpMethod)(TP1, TP2)) {
			ASSERT(lpInstance != NULL);

			m_lpInstance = lpInstance;
			m_lpMethod = lpMethod;
		}

	public:
		void Invoke(TP1 _Param1, TP2 _Param2) { (m_lpInstance->*m_lpMethod)(_Param1, _Param2); }

	protected:
		BOOL Equals(CDelegateT2<TP1, TP2> * lpOther) {
			ASSERT(lpOther != NULL);

			return (
				m_lpInstance == ((CTypedDelegateT2<T, TP1, TP2> *)lpOther)->m_lpInstance &&
				m_lpMethod == ((CTypedDelegateT2<T, TP1, TP2> *)lpOther)->m_lpMethod
			);
		}

	private:
		friend class CDelegateT2<TP1, TP2>;
	};

}

template<typename TP1, typename TP2>
class DelegateT2
{
private:
	Internal::CDelegateT2<TP1, TP2> * m_lpDelegate;

public:
	DelegateT2(Internal::CDelegateT2<TP1, TP2> * lpDelegate) {
		ASSERT(lpDelegate != NULL);

		m_lpDelegate = lpDelegate;
	}
	DelegateT2(const DelegateT2<TP1, TP2> & vDelegate) {
		m_lpDelegate = vDelegate.m_lpDelegate;
		m_lpDelegate->AddRef();
	}

	virtual ~DelegateT2() {
		m_lpDelegate->RemoveRef();
	}

	void Invoke(TP1 _Param1, TP2 _Param2) { m_lpDelegate->Invoke(_Param1, _Param2); }

	void operator()() { Invoke(); }
	bool operator==(const DelegateT2<TP1, TP2> & _Other) const { return m_lpDelegate->Equals(_Other.m_lpDelegate) != FALSE; }
	bool operator!=(const DelegateT2<TP1, TP2> & _Other) const { return !(*this == _Other); }
	DelegateT2<TP1, TP2> & operator=(const DelegateT2<TP1, TP2> & _Other) {
		m_lpDelegate->RemoveRef();
		m_lpDelegate = _Other.m_lpDelegate;
		m_lpDelegate->AddRef();
		return *this;
	}
};

template<typename TP1, typename TP2>
class EventT2 : public Event
{
private:
	typedef vector< DelegateT2<TP1, TP2> > TDelegateVector;
	typename typedef TDelegateVector::iterator TDelegateVectorIter;
	typename typedef TDelegateVector::const_iterator TDelegateVectorConstIter;

private:
	TDelegateVector m_vDelegates;

public:
	EventT2() { }
	virtual ~EventT2() { }

	void operator+=(const DelegateT2<TP1, TP2> & vDelegate) {
		if (FindDelegate(vDelegate) == m_vDelegates.end())
			m_vDelegates.push_back(vDelegate);
	}
	void operator-=(const DelegateT2<TP1, TP2> & vDelegate) {
		TDelegateVectorIter pos = FindDelegate(vDelegate);
		if (pos != m_vDelegates.end())
			m_vDelegates.erase(pos);
	}
	bool operator==(LPVOID * lpOther) const {
		ASSERT(lpOther == NULL);

		// This may only be used for the == NULL and != NULL
		// constructions.
		ASSERT(lpOther == NULL);
		return m_vDelegates.empty();
	}
	bool operator!=(LPVOID * lpOther) const { return !(*this == lpOther); }
	void operator()(TP1 _Param1, TP2 _Param2) {
		ASSERT(!m_vDelegates.empty());
		
		// Delegates vector is copied here because there are some times
		// that delete the this inside the timer.
		TDelegateVector vCopy(m_vDelegates.begin(), m_vDelegates.end());

		for (TDelegateVectorIter iter = vCopy.begin(); iter != vCopy.end(); iter++)
			iter->Invoke(_Param1, _Param2);
	}

private:
	TDelegateVectorIter FindDelegate(DelegateT2<TP1, TP2> vDelegate) {
		for (TDelegateVectorIter iter = m_vDelegates.begin(); iter != m_vDelegates.end(); iter++) {
			if (*iter == vDelegate)
				return iter;
		}
		return m_vDelegates.end();
	}

};

template<typename T, typename TP1, typename TP2>
DelegateT2<TP1, TP2> AddressOfT2(T * lpInstance, void (T::*lpMethod)(TP1, TP2)) {
	ASSERT(lpInstance != NULL);

	return DelegateT2<TP1, TP2>(new Internal::CTypedDelegateT2<T, TP1, TP2>(lpInstance, lpMethod));
}

#endif // _INC_DELEGATE
