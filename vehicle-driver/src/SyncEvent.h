#ifndef _SYNCSyncEvent_H_
#define _SYNCSyncEvent_H_

//-----------------------------------------------------------------------------
/**
	CSynCSyncEvent is a delegate with no arguments. CSynCSyncEvent is useful when one 
	wants to enable one system to call member function in another system without 
	the two	systems having to know about each other.

	Usage:
		class CFoo
		{
			void HandleKeyPress();
		};

		Declaration:	CSynCSyncEvent KeyEvent;

		Binding:		KeyEvent.Bind(this, &CFoo::HandleKeyPress);

		Calling:		KeyEvent();

		Unbinding:		KeyEvent.Unbind();
*/
//-----------------------------------------------------------------------------

#ifdef __GNUC__
struct __attribute__((__may_alias__)) SMember{int a; int b;};
#else
struct SMember{int a; int b;};
#endif

class CSyncEvent
{
	typedef void (*TPVoidFunction)();

	// This contains either a member function pointer or a static function pointer
	// since an event cannot point to both types at the same time, it saves memory
	// to store this in an union.
	union UFunctor
	{
		SMember pMemberFunc;
		void (*pStaticFunc)();
	};

public:

	CSyncEvent();

	CSyncEvent(TPVoidFunction pFunc);

	// Creates the CSyncEvent object and binds it to a void T::*pmFunc(void) member method.
	template<class T>
	CSyncEvent(T* pObj, void(T::*pmFunc)())
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	// Creates the CSyncEvent object and binds it to a int T::*pmFunc(void) member method.
	template<class T>
	CSyncEvent(T* pObj, int(T::*pmFunc)())
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	// Creates the CSyncEvent object and binds it to a bool T::*pmFunc(void) member method.
	template<class T>
	CSyncEvent(T* pObj, bool(T::*pmFunc)())
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	void Bind(TPVoidFunction pmFunc);

	template<class T>
	void Bind(T* pObj, void(T::*pmFunc)())
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	template<class T>
	void Bind(T* pObj, int(T::*pmFunc)())
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	template<class T>
	void Bind(T* pObj, bool(T::*pmFunc)())
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	void Unbind();

	void operator()();

private:
	UFunctor	m_Functor;
	void*		m_pObj;
};


//-----------------------------------------------------------------------------
/**
	CSyncEvent0 is a delegate with a return argument. CSyncEvent0 is useful when one wants to
	enable one system to call member function in another system without the two
	systems having to know about each other.

	When declaring CSyncEvent0 one must define a return type and a default value to 
	return if no function is bound to the CEven0 class.

	Usage:
		class CFoo
		{
			bool HandleKeyPress();
		};

		CFoo MyFoo;

		Declaration:	CSyncEvent0<bool, false> KeyEvent;

		Binding:		KeyEvent.Bind(&MyFoo, &CFoo::HandleKeyPress);

		Calling:		KeyEvent();	

		Unbinding:		KeyEvent.Unbind();

*/
//-----------------------------------------------------------------------------
template<class T0, T0 DefaultValue>
class CSyncEvent0
{
	typedef T0 (*TPVoidFunction)();

	// This contains either a member function pointer or a static function pointer
	// since an event cannot point to both types at the same time, it saves memory
	// to store this in an union.
	union UFunctor
	{
		SMember pMemberFunc;
		T0 (*pStaticFunc)();
	};

public:

	CSyncEvent0() :
		m_pObj(0)
	{
		m_Functor.pStaticFunc = 0;
	}

	CSyncEvent0(TPVoidFunction pFunc) :
		m_pObj(0)
	{
		m_Functor.pStaticFunc = pFunc;
	}

	void Bind( TPVoidFunction pmFunc )
	{
		m_Functor.pStaticFunc = pmFunc;
		m_pObj = 0;
	}

	template<class T>
	void Bind(T* pObj, T0(T::*pmFunc)())
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	void Unbind()
	{
		m_pObj = 0;
		m_Functor.pStaticFunc = 0;	// clear function pointer
	}

	bool IsBound() const
	{
		if(m_pObj)
		{
			return true;
		}
		else if(m_Functor.pStaticFunc)
		{
			return true;
		}
		return false;
	}

	T0 operator()()
	{
		if(m_pObj)
		{
			typedef T0(SMember::*T)();
			return ((SMember*)m_pObj->**(T*)&(m_Functor.pMemberFunc))();
		} 
		else if( m_Functor.pStaticFunc )
		{
			return m_Functor.pStaticFunc();
		}
		return DefaultValue;
	}

private:
	UFunctor	m_Functor;
	void*		m_pObj;
};


//-----------------------------------------------------------------------------
/**
	CSyncEvent1 is a delegate with one argument which is of a templated type. 
	CSyncEvent1 is useful when one wants to	enable one system to call member 
	function in another system without the two systems having to know about 
	each other.

	Usage:
		class CFoo
		{
			void HandleTimeEvent(uint32 iTime);
		};
	
		Declaration:	CSyncEvent1<uint32> TimeEvent;

		Binding:		TimeEvent.Bind(this, &CFoo::HandleTimeEvent);

		Calling:		TimeEvent(1000);

		Unbinding:		TimeEvent.Unbind();

*/
//-----------------------------------------------------------------------------

template<class T1>
class CSyncEvent1
{
	typedef void (*TPVoidFunction)(T1);

	// This contains either a member function pointer or a static function pointer
	// since an event cannot point to both types at the same time, it saves memory
	// to store this in an union.
	union UFunctor
	{
		SMember pMemberFunc;
		void (*pStaticFunc)(T1);
	};

public:

	CSyncEvent1()
	{
		m_pObj = 0;
		m_Functor.pStaticFunc = 0;
	}

	CSyncEvent1(TPVoidFunction pmFunc)
	{
		m_Functor.pStaticFunc = pmFunc;
		m_pObj = 0;
	}

	template<class T>
	CSyncEvent1(T* pObj, void(T::*pmFunc)(T1))
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	void Bind( TPVoidFunction pmFunc )
	{
		m_Functor.pStaticFunc = pmFunc;
		m_pObj = 0;
	}

	template<class T>
	void Bind(T* pObj, void(T::*pmFunc)(T1))
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	void Unbind()
	{
		m_pObj = 0;
		m_Functor.pStaticFunc = 0;	// clear function pointer
	}


	void operator()(T1 Arg1)
	{
		if(m_pObj)
		{
			typedef void(SMember::*T)(T1);
			((SMember*)m_pObj->**(T*)&m_Functor)(Arg1);
		}
		else if( m_Functor.pStaticFunc )
		{
			m_Functor.pStaticFunc(Arg1);
		}
	}

private:
	UFunctor	m_Functor;
	void*		m_pObj;
};


//-----------------------------------------------------------------------------
/**
	CSyncEvent2 is a delegate with two arguments which is of a templated type. 
	CSyncEvent2 is useful when one wants to	enable one system to call member 
	function in another system without the two systems having to know about 
	each other.

	Usage:
		class CFoo
		{
			void HandleTimeEvent(uint8_t* pBuffer, uint32_t iSize);
		};
	
		Declaration:	CSyncEvent2<uint8_t*, uint32_t> BufferEvent;

		Binding:		BufferEvent.Bind(this, &CFoo::HandleTimeEvent);

		Calling:		BufferEvent(p, 10);

		Unbinding:		TimeEvent.Unbind();

*/
//-----------------------------------------------------------------------------

template<class T1, class T2>
class CSyncEvent2
{
	typedef void (*TPVoidFunction)(T1, T2);

	// This contains either a member function pointer or a static function pointer
	// since an event cannot point to both types at the same time, it saves memory
	// to store this in an union.
	union UFunctor
	{
		SMember pMemberFunc;
		void (*pStaticFunc)(T1, T2);
	};

public:
	CSyncEvent2()
	{
		m_pObj = 0;
		m_Functor.pStaticFunc = 0;
	}

	CSyncEvent2(TPVoidFunction pmFunc)
	{
		m_Functor.pStaticFunc = pmFunc;
		m_pObj = 0;
	}

	template<class T>
	CSyncEvent2(T* pObj, void(T::*pmFunc)(T1, T2))
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	void Bind( TPVoidFunction pmFunc )
	{
		m_Functor.pStaticFunc = pmFunc;
		m_pObj = 0;
	}

	template<class T>
	void Bind(T* pObj, void(T::*pmFunc)(T1, T2))
	{
		m_Functor.pMemberFunc = *(SMember*)&pmFunc;
		m_pObj = pObj;
	}

	void Unbind()
	{
		m_pObj = 0;
		m_Functor.pStaticFunc = 0;	// clear function pointer
	}

	void operator()(T1 Arg1, T2 Arg2)
	{
		if(m_pObj)
		{
			typedef void(SMember::*T)(T1, T2);
			((SMember*)m_pObj->**(T*)&m_Functor)(Arg1, Arg2);
		}
		else if( m_Functor.pStaticFunc )
		{
			m_Functor.pStaticFunc(Arg1, Arg2);
		}
	}

private:
	UFunctor	m_Functor;
	void*		m_pObj;
};




#endif
