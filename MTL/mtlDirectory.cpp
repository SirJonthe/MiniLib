/*
 *  mtlDirectory.cpp
 *  MiniLib
 *
 *  Created by Jonathan Karlsson on 11/2/13.
 *  Copyright 2013 Jonathan Karlsson. All rights reserved.
 *
 */

#include "mtlParser.h"
#include "mtlDirectory.h"

/*void mtlDirectory::CopyDirectory(const char *p_str, int p_num)
{
	for (int i = 0; i < p_num; ++i) {
		if (*p_str == ' ' || *p_str == '\t' || *p_str == '\n') {
			++p_str;
			--p_num;
			--i;
		} else {
			break;
		}
	}
	for (int i = p_num - 1; i >= 0; --i) {
		if (p_str[i] == ' ' || p_str[i] == '\t' || p_str[i] == '\n') {
			--p_num;
		} else {
			break;
		}
	}
	m_dir.Copy(p_str, p_num);
}*/

void mtlDirectory::CalculateComponents( void )
{
	m_file0 = m_dir.FindLastChar("\\/") + 1;
	if (m_file0 == m_dir.GetSize()) { // for directories containing only folders
		m_file1 = m_file0;
		m_ext0 = m_file0;
	} else { // folder and file
		m_file1 = m_dir.FindLastChar(".");
		m_ext0 = m_file1;
		if (m_ext0 == -1) { // no extension
			m_file1 = m_dir.GetSize();
			m_ext0 = m_file1;
		} else {
			++m_ext0;
		}
	}

	if (m_file0 > m_ext0) { // folders containing .
		m_file1 = m_file0;
		m_ext0 = m_file0;
	}
	if (m_file0 == m_file1) { // hidden Unix files (/Folder/.hidden)
		m_file1 = m_dir.GetSize();
		m_ext0 = m_file1;
	}
}

mtlDirectory::mtlDirectory(const mtlChars &p_directory)
{
	m_dir.Copy(p_directory);
	CalculateComponents();
}

mtlDirectory::mtlDirectory(const mtlString &p_directory)
{
	m_dir.Copy(p_directory);
	CalculateComponents();
}

mtlDirectory::mtlDirectory(const mtlSubstring &p_directory)
{
	m_dir.Copy(p_directory);
	CalculateComponents();
}

/*mtlDirectory::mtlDirectory(const char *p_directory)
{
	m_dir.Copy(p_directory);
	CalculateComponents();
}*/

mtlDirectory::mtlDirectory(const mtlDirectory &p_directory)
{
	m_dir.Copy(p_directory.m_dir);
	CalculateComponents();
}

mtlDirectory &mtlDirectory::operator=(const mtlChars &p_directory)
{
	m_dir.Copy(p_directory);
	CalculateComponents();
	return *this;
}

mtlDirectory &mtlDirectory::operator=(const mtlString &p_directory)
{
	m_dir.Copy(p_directory);
	CalculateComponents();
	return *this;
}

mtlDirectory &mtlDirectory::operator=(const mtlSubstring &p_directory)
{
	m_dir.Copy(p_directory);
	CalculateComponents();
	return *this;
}

/*mtlDirectory &mtlDirectory::operator=(const char *p_directory)
{
	m_dir.Copy(p_directory);
	CalculateComponents();
	return *this;
}*/

mtlDirectory &mtlDirectory::operator=(const mtlDirectory &p_directory)
{
	if (this != &p_directory) {
		m_dir.Copy(p_directory.m_dir);
		CalculateComponents();
	}
	return *this;
}

mtlSubstring mtlDirectory::GetFilename( void ) const
{
	if (m_file1 < 0) { return mtlSubstring(); }
	return m_dir.GetSubstring(m_file0, m_file1);
}

mtlSubstring mtlDirectory::GetExtension( void ) const
{
	if (m_dir.GetSize() == 0) { return mtlSubstring(); }
	return m_dir.GetSubstring(m_ext0, m_dir.GetSize());
}

mtlSubstring mtlDirectory::GetFolders( void ) const
{
	if (m_file0 < 0) { return mtlSubstring(); }
	return m_dir.GetSubstring(0, m_file0);
}
