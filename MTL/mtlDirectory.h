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
#include "mtlList.h"

class mtlDirectory
{
	//static mtlDirectory m_workingDirectory;
private:
	mtlString			m_dir;
	int					m_file0;
	int					m_file1;
	int					m_ext0;
	//mtlList<mtlChars>	m_dirs;
	// now I can just represent this using an mtlList<mtlChars> that's split by "/" or "\"
	// this will also make it super simple to make a directory relative to another
private:
	void CalculateComponents( void );
public:
							mtlDirectory( void );
							template < int t_size >
							mtlDirectory(const char (&p_str)[t_size], int p_size = -1);
							mtlDirectory(const mtlChars &p_directory);
							mtlDirectory(const mtlString &p_directory);
							mtlDirectory(const mtlDirectory &p_directory);
	mtlDirectory			&operator=(const mtlChars &p_directory);
	mtlDirectory			&operator=(const mtlString &p_directory);
	mtlDirectory			&operator=(const mtlDirectory &p_directory);
	mtlChars				GetFilename( void ) const;
	mtlChars				GetExtension( void ) const;
	mtlChars				GetFolders( void ) const;
	const mtlString			&GetDirectory( void ) const { return m_dir; }
	//const mtlList<mtlChars> &GetDirectories( void ) const { return m_dirs; }
};

template < int t_size >
mtlDirectory::mtlDirectory(const char (&p_str)[t_size], int p_size)
{
	m_dir.Copy(mtlChars(p_str, p_size));
	CalculateComponents();
}

#endif
