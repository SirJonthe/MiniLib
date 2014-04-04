//
//  mglTimer.cpp
//  MiniLib
//
//  Created by Jonathan Karlsson on 3/21/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#include "mglEngine.h"
#include "mglTimer.h"

mglTimer::mglTimer(const mglEngine *p_engine) : m_sinceLast(-1.0), m_interval(0.0), m_numTicks(0.0), m_engine(p_engine)
{}

void mglTimer::SetInterval(double p_interval)
{
	m_interval = p_interval;
}

void mglTimer::SetTicksPerSecond(double p_ticks)
{
	m_interval = 1.0 / p_ticks;
}

void mglTimer::Start( void )
{
	m_sinceLast = m_engine->GetTime();
}

void mglTimer::Stop( void )
{
	m_sinceLast = -1.0;
	m_numTicks = 0.0;
}

double mglTimer::Update( void )
{
	if (!IsStopped()) {
		const double time = m_engine->GetTime();
		m_numTicks = (time - m_sinceLast) / m_interval;
		if (time > m_sinceLast + m_interval) { m_sinceLast = time; }
	}
	return m_numTicks;
}
