//
//  mglTimer.h
//  MiniLib
//
//  Created by Jonathan Karlsson on 3/21/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#ifndef MGL_TIMER_H_INCLUDED__
#define MGL_TIMER_H_INCLUDED__

class mglEngine;

class mglTimer
{
private:
	double			m_sinceLast;
	double			m_interval;
	double			m_numTicks;
	const mglEngine	*m_engine;
public:
	explicit	mglTimer(const mglEngine *p_engine);
	void		SetInterval(double p_interval);
	void		SetTicksPerSecond(double p_ticks);
	void		Start( void );
	void		Stop( void );
	bool		IsStopped( void ) const { return m_sinceLast < 0.0; }
	double		Update( void );
	double		GetTickCount( void ) const { return m_numTicks; }
};

#endif
