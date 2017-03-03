#ifndef __BASESTATE_H
#define __BASESTATE_H

//pure virtual interface for individual states


template <class ent_type> class State
{
public:
	virtual void InitializeState(ent_type*) = 0;
	virtual void UpdateScene(ent_type*, double dT) = 0;	//this will need to take in dT later
	virtual void RenderScene(ent_type*) = 0;
	virtual void OnResetDevice(ent_type*) = 0;
	virtual void OnLostDevice(ent_type*) = 0;
	virtual void LeaveState(ent_type*) = 0;
};

#endif