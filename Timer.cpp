#include "Timer.h"

Timer::Timer()
{
	begin = end = elapsed = 0;
	QueryPerformanceFrequency(&Freq);
	Freq.QuadPart /= 1000;
}

Timer* Timer::Instance()
{
	static Timer inst;
	return &inst;
}

void Timer::Start()
{
	QueryPerformanceCounter(&Count);
	begin = ((double)Count.QuadPart / (double)Freq.QuadPart);
}

void Timer::Stop()
{
	QueryPerformanceCounter(&Count);
	end = ((double)Count.QuadPart / (double)Freq.QuadPart);
}

double Timer::GetTime()
{
	QueryPerformanceCounter(&Count);
	double current = ((double)Count.QuadPart / (double)Freq.QuadPart);
	elapsed = current - begin;
	return (elapsed/1000);
}

double Timer::GetTimeElapsed()
{
	double elap2;
	QueryPerformanceCounter(&Count);
	double current = ((double)Count.QuadPart / (double)Freq.QuadPart);
	elapsed = current - begin;
	elap2 = (elapsed/1000) - last;
	last = (elapsed/1000);
	return (elap2);
}

void Timer::Clear()
{
	begin = end = elapsed = 0;
	QueryPerformanceFrequency(&Freq);
	Freq.QuadPart /= 1000;
}