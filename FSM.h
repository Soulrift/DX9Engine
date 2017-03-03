#ifndef __FSM_H
#define __FSM_H

//finite state machine

#include "BaseState.h"

template <class ent_type> class StateMachine
{
private:
	ent_type* m_pOwner;		//owner of the state machine
	State<ent_type>* m_pCurrentState;
	State<ent_type>* m_pPreviousState;
	State<ent_type>* m_pGlobalState;
public:
	//Constructor (force an owner)
	StateMachine(ent_type* p_owner):m_pOwner(p_owner),m_pCurrentState(NULL), m_pPreviousState(NULL), m_pGlobalState(NULL){}

	//setters
	void setCurrent(State<ent_type>* p_cur)
	{
		m_pCurrentState = p_cur;
		m_pCurrentState->InitializeState(m_pOwner);
	}
	void setPrevious(State<ent_type>* p_prev)
	{
		m_pPreviousState = p_prev;
	}
	void setGlobal(State<ent_type>* p_glob)
	{
		m_pGlobalState = p_glob;
	}

	//change state
	void ChangeState(State<ent_type>* p_new)
	{
		m_pPreviousState = m_pCurrentState;
		m_pCurrentState->LeaveState(m_pOwner);
		m_pCurrentState = p_new;
		m_pCurrentState->InitializeState(m_pOwner);
	}
	void RevertState()
	{
		ChangeState(m_pPreviousState);
	}

	//getters
	State<ent_type>* GetCurrent() const
	{
		return m_pCurrentState;
	}
	State<ent_type>* GetGlobal() const
	{
		return m_pGlobalState;
	}
	State<ent_type>* GetPrevious() const
	{
		return m_pPreviousState;
	}

	void Update(double dT) const
	{
		m_pCurrentState->UpdateScene(m_pOwner, dT);
	}

	void Render() const
	{
		m_pCurrentState->RenderScene(m_pOwner);
	}
	//code in here for reset/lost device
	void LeaveState() const
	{
		m_pCurrentState->LeaveState(m_pOwner);
	}

	void LostDevice() const
	{
		m_pCurrentState->OnLostDevice(m_pOwner);
	}

	void ResetDevice() const
	{
		m_pCurrentState->OnResetDevice(m_pOwner);
	}

};

#endif