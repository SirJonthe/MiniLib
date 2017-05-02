/*
 *  mtlPath.cpp
 *  MiniLib
 *
 *  Created by Jonathan Karlsson on 11/2/13.
 *  Copyright 2013 Jonathan Karlsson. All rights reserved.
 *
 */

#include "mtlParser.h"
#include "mtlPath.h"

#define mtlDirSeparator '/'

void mtlPath::CopyPath(const mtlChars &path, mtlPath::Type type)
{
	m_fullpath.Copy(path);
	for (int i = 0; i < m_fullpath.GetSize(); ++i) {
		if (m_fullpath[i] == '\\') {
			m_fullpath[i] = mtlDirSeparator;
		}
	}

	const int last_index = m_fullpath.GetSize() - 1;
	if (type == Directory || m_fullpath[last_index] == mtlDirSeparator) {
		if (m_fullpath.GetSize() == 0 || m_fullpath[last_index] != mtlDirSeparator) {
			m_fullpath.Append(mtlDirSeparator);
		}
		m_type = Directory;
	} else if (type == File && m_fullpath.GetSize() > 0) {
		m_type = File;
	} else {
		m_type = None;
	}
}

void mtlPath::RefreshDirs( void )
{
	m_fullpath.SplitByChar(m_dirs, mtlDirSeparator);

	//mtlItem<mtlChars> *i = m_dirs.GetFirst();
	if (m_dirs.GetSize() > 0 && m_dirs.GetLast()->GetItem().GetSize() <= 0) {
		m_dirs.RemoveLast();
	}
}

void mtlPath::SetVars(const mtlChars &path, mtlPath::Type type)
{
	CopyPath(path, type);
	RefreshDirs();
}

mtlPath::mtlPath( void ) : m_fullpath(), m_dirs(), m_type(None)
{}

mtlPath::mtlPath(const mtlChars &p_path, mtlPath::Type p_type) : m_fullpath(), m_dirs(), m_type(None)
{
	SetVars(p_path, p_type);
}

mtlPath::mtlPath(const mtlPath &p_path) : m_fullpath(), m_dirs(), m_type(p_path.m_type)
{
	m_fullpath.Copy(p_path.m_fullpath);
	m_dirs.Copy(p_path.m_dirs);
}

mtlPath &mtlPath::operator=(const mtlPath &p_path)
{
	if (this != &p_path) {
		m_fullpath.Copy(p_path.m_fullpath);
		m_dirs.Copy(p_path.m_dirs);
		m_type = p_path.m_type;
	}
	return *this;
}

mtlPath mtlPath::operator +(const mtlPath &p) const
{
	return mtlPath(*this) += p;
}

mtlPath &mtlPath::operator +=(const mtlPath &p)
{
	// Always remove the file if the path points to one
	if (m_type == File && m_dirs.GetSize() > 0) {
		m_dirs.RemoveLast();
	}

	// Generate a temporary directory list
	const mtlItem<mtlChars> *dir_iter = p.m_dirs.GetFirst();
	while (dir_iter != NULL) {
		if (dir_iter->GetItem().Compare("..") && m_dirs.GetSize() > 0 && !m_dirs.GetLast()->GetItem().Compare("..")) {
			m_dirs.RemoveLast(); // not quite correct
		} else {
			m_dirs.AddLast(dir_iter->GetItem());
		}
		dir_iter = dir_iter->GetNext();
	}

	// Generate full path string based on temporary directory list
	mtlString fullpath;
	dir_iter = m_dirs.GetFirst();
	while (dir_iter != NULL) {
		fullpath.Append(dir_iter->GetItem());
		dir_iter = dir_iter->GetNext();
		if (dir_iter != NULL || m_type != File) {
			fullpath.Append(mtlDirSeparator);
		}
	}

	// Refresh to get final directory list
	SetVars(fullpath, p.m_type);

	return *this;
}

mtlChars mtlPath::GetFileName( void ) const
{
	return (IsFile() && m_dirs.GetSize() > 0) ? m_dirs.GetLast()->GetItem() : "";
}

mtlChars mtlPath::GetFileExtension( void ) const
{
	mtlChars filename = GetFileName();
	int sep_index = filename.FindLastChar('.');
	return (sep_index > 0) ? mtlChars(filename, sep_index + 1, filename.GetSize()) : ""; // > 0 excludes unix hidden files from being an extension
}

mtlChars mtlPath::GetDirectory( void ) const
{
	int sep_index = m_fullpath.FindLastChar(mtlDirSeparator);
	return (sep_index > -1) ? mtlChars(m_fullpath, 0, sep_index + 1) : mtlChars(m_fullpath);
}
