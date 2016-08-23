/*
 *  mtlPath.h
 *  MiniLib
 *
 *  Created by Jonathan Karlsson on 10/22/13.
 *  Copyright 2013 Jonathan Karlsson. All rights reserved.
 *
 */

#ifndef MTL_PATH_H_INCLUDED__
#define MTL_PATH_H_INCLUDED__

#include "mtlString.h"
#include "mtlList.h"

class mtlPath
{
public:
	enum Type
	{
		None,
		File,
		Directory
	};

private:
	mtlString         m_fullpath;
	mtlList<mtlChars> m_dirs;
	Type              m_type;


private:
	void CopyPath(const mtlChars &path, Type type);
	void RefreshDirs( void );
	void SetVars(const mtlChars &path, Type type);

public:
	mtlPath( void );
	mtlPath(const mtlChars &p_path, Type p_type = File);
	mtlPath(const mtlPath &p_path);
	mtlPath &operator=(const mtlPath &p_path);

	mtlPath operator+(const mtlPath &p) const;
	mtlPath &operator+=(const mtlPath &p);

	const mtlList<mtlChars> &GetDirectories( void )   const { return m_dirs; }
	const mtlChars          &GetPath( void )          const { return m_fullpath; }
	mtlChars                 GetFileName( void )      const;
	mtlChars                 GetFileExtension( void ) const;
	mtlChars                 GetDirectory( void )     const;

	bool IsFile( void )      const { return m_type == File; }
	bool IsDirectory( void ) const { return m_type == Directory; }
};

#endif
