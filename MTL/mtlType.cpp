#include "mtlType.h"

mtlTypeID mtlNewTypeID( void )
{
	static mtlTypeID id = 0;
	return ++id;
}

mtlBase::~mtlBase( void ) {}
