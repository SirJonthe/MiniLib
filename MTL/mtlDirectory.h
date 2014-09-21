/*
 *  mtlDirectory.h
 *  MiniLib
 *
 *  Created by Jonathan Karlsson on 10/22/13.
 *  Copyright 2013 Jonathan Karlsson. All rights reserved.
 *
 */

#ifndef MTL_DIRECTORY_H_INCLUDED__
#define MTL_DIRECTORY_H_INCLUDED__

#include "mtlString.h"

class mtlDirectory
{
	//static mtlDirectory m_workingDirectory;
private:
	mtlString	m_dir;
	int			m_file0;
	int			m_file1;
	int			m_ext0;
private:
	void CalculateComponents( void );
public:
					mtlDirectory(const mtlChars &p_directory);
					mtlDirectory(const mtlString &p_directory);
					mtlDirectory(const mtlDirectory &p_directory);
	mtlDirectory	&operator=(const mtlChars &p_directory);
	mtlDirectory	&operator=(const mtlString &p_directory);
	mtlDirectory	&operator=(const mtlDirectory &p_directory);
	mtlChars		GetFilename( void ) const;
	mtlChars		GetExtension( void ) const;
	mtlChars		GetFolders( void ) const;
	const mtlString	&GetDirectory( void ) const { return m_dir; }
};

#endif
