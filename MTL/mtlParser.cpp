#include <fstream>
#include "mtlParser.h"

void mtlParser::ClearTrailingWhitespaces( void )
{
	while (!IsEndOfFile(m_size-1) && IsWhite(m_buffer.GetChars()[m_size-1])) {
		--m_size;
	}
}

mtlParser::mtlParser( void ) : m_buffer(), m_size(0), m_reader(0), m_undo()
{}

mtlParser::mtlParser(const mtlChars &p_buffer) : m_buffer(), m_size(0), m_reader(0), m_undo()
{
	SetBuffer(p_buffer);
}

void mtlParser::SetBuffer(const mtlChars &p_buffer)
{
	m_buffer = p_buffer;
	m_size = p_buffer.GetSize();
	m_reader = 0;
	m_undo.Free();
	ClearTrailingWhitespaces();
}

bool mtlParser::BufferFile(const mtlDirectory &p_file, mtlString &p_buffer)
{
	std::ifstream fin(p_file.GetDirectory().GetChars(), std::ios::ate|std::ios::binary);
	if (!fin.is_open()) { return false; }
	p_buffer.SetSize((int)fin.tellg());
	fin.seekg(0, std::ios::beg);
	return !fin.read(p_buffer.GetChars(), p_buffer.GetSize()).bad();
}

char mtlParser::ReadChar(bool skipWhite)
{
	m_undo.PushBack(m_reader);
	if (skipWhite) {
		while (!IsEndOfFile(m_reader) && IsWhite(m_buffer.GetChars()[m_reader])) {
			++m_reader;
		}
	}
	return m_buffer.GetChars()[m_reader++];
}

char mtlParser::PeekChar(bool skipWhite) const
{
	int i = m_reader;
	if (skipWhite) {
		while (!IsEndOfFile(i) && IsWhite(m_buffer.GetChars()[i])) {
			++i;
		}
	}
	return m_buffer.GetChars()[i];
}

void mtlParser::SkipChar(bool skipWhite)
{
	m_undo.PushBack(m_reader);
	if (skipWhite) {
		while (!IsEndOfFile(m_reader) && IsWhite(m_buffer.GetChars()[m_reader])) {
			++m_reader;
		}
	}
	++m_reader;
}


void mtlParser::BackChar(bool skipWhite)
{
	m_undo.PushBack(m_reader);
	if (skipWhite) {
		while (!IsEndOfFile(m_reader-1) && IsWhite(m_buffer.GetChars()[m_reader-1])) {
			--m_reader;
		}
	}
	--m_reader;
}

mtlSubstring mtlParser::ReadWord( void )
{
	m_undo.PushBack(m_reader);
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	int start = m_reader;
	while (!IsEndOfFile() && !IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	return mtlSubstring(m_buffer, start, m_reader);
}

mtlSubstring mtlParser::PeekWord( void ) const
{
	int i = m_reader;
	while (!IsEndOfFile(i) && IsWhite(m_buffer.GetChars()[i])) {
		++i;
	}
	int start = i;
	while (!IsEndOfFile(i) && !IsWhite(m_buffer.GetChars()[i])) {
		++i;
	}
	return mtlSubstring(m_buffer, start, i);
}

void mtlParser::BackWord( void )
{
	m_undo.PushBack(m_reader);
	int r = m_reader;
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	while (!IsEndOfFile() && !IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	if (IsEndOfFile() && r != m_reader+1) { ++m_reader; } // in order to make the first word in file readable only once
}

void mtlParser::SkipWord( void )
{
	m_undo.PushBack(m_reader);
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	while (!IsEndOfFile() && !IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
}

mtlSubstring mtlParser::ReadLine( void )
{
	m_undo.PushBack(m_reader);
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	int start = m_reader;
	while (!IsEndOfFile() && !IsNewl(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	int end = m_reader;
	while (!IsEndOfFile(end-1) && IsWhite(m_buffer.GetChars()[end-1])) {
		--end;
	}
	return mtlSubstring(m_buffer, start, end);
}

mtlSubstring mtlParser::PeekLine( void ) const
{
	int i = m_reader;
	while (!IsEndOfFile(i) && IsWhite(m_buffer.GetChars()[i])) {
		++i;
	}
	int start = i;
	while (!IsEndOfFile(i) && !IsNewl(m_buffer.GetChars()[i])) {
		++i;
	}
	while (!IsEndOfFile(i-1) && IsWhite(m_buffer.GetChars()[i-1])) {
		--i;
	}
	return mtlSubstring(m_buffer, start, i);
}

void mtlParser::BackLine( void )
{
	m_undo.PushBack(m_reader);
	int r = m_reader;
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	while (!IsEndOfFile() && !IsNewl(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	if (IsEndOfFile() && r != m_reader+1) { ++m_reader; } // in order to make the first line in file readable only once
}

void mtlParser::SkipLine( void )
{
	m_undo.PushBack(m_reader);
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	while (!IsEndOfFile() && !IsNewl(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
}

void mtlParser::JumpToEndWord( void )
{
	// results in two undo
	JumpToEndChar();
	BackWord();
}

void mtlParser::JumpToEndLine( void )
{
	// results in two undo
	JumpToEndChar();
	BackLine();
}

bool mtlParser::JumpToChar(const mtlChars &p_chars)
{
	m_undo.PushBack(m_reader);
	const int count = p_chars.GetSize();
	while (!IsEndOfFile()) {
		char c = m_buffer.GetChars()[m_reader++];
		for (int i = 0; i < count; ++i) {
			if (c == p_chars.GetChars()[i]) {
				return true;
			}
		}
	}
	return false;
}

bool mtlParser::JumpToCharBack(const mtlChars &p_chars)
{
	m_undo.PushBack(m_reader);
	const int count = p_chars.GetSize();
	while (!IsEndOfFile()) {
		char c = m_buffer.GetChars()[m_reader--];
		for (int i = 0; i < count; ++i) {
			if (c == p_chars.GetChars()[i]) {
				return true;
			}
		}
	}
	return false;
}

/*mtlSubstring mtlParser::GetEnclosedString(char closing, bool allowEscape, bool allowMultiLine)
{
	return mtlSubstring();
}*/

void mtlParser::UndoRead( void )
{
	if (UndoCount() > 0) {
		m_reader = m_undo.GetBack()->GetItem();
		m_undo.PopBack();
	}
}
