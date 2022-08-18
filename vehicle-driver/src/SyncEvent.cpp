#include "SyncEvent.h"


CSyncEvent::CSyncEvent() :
	m_pObj(0)
{
	m_Functor.pStaticFunc = 0;
}


CSyncEvent::CSyncEvent(TPVoidFunction pFunc) :
	m_pObj(0)
{
	m_Functor.pStaticFunc = pFunc;
}


void CSyncEvent::Bind( TPVoidFunction pmFunc )
{
	m_Functor.pStaticFunc = pmFunc;
	m_pObj = 0;
}


void CSyncEvent::Unbind()
{
	m_pObj = 0;
	m_Functor.pStaticFunc = 0;	// clear function pointer
}


void CSyncEvent::operator()()
{
	if(m_pObj)
	{
		typedef void(SMember::*T)();
		((SMember*)m_pObj->**(T*)&(m_Functor.pMemberFunc))();
	} 
	else if( m_Functor.pStaticFunc )
	{
		m_Functor.pStaticFunc();
	}
}


