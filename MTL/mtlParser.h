#ifndef MTL_PARSER_H_INCLUDED__
#define MTL_PARSER_H_INCLUDED__

#include "mtlString.h"
#include "mtlList.h"
#include "mtlDirectory.h"

class mtlParser
{
private:
	static const char			space = ' ';
	static const char			tab = '\t';
	static const char			newl = '\n';
	static const char			cret = '\r';
	static const unsigned int	nlbit = 1;
	static const unsigned int	nwbit = 2;
public:
	static bool IsWhite(char c) { return c == space || c == tab || c == newl || c == cret; }
	static bool IsNewl(char c) { return c == newl || c == cret; }
private:
	mtlChars		m_buffer;
	int				m_size; // size can be smaller than m_buffer->GetSize() (skips trailing whitespaces)
	int				m_reader;
	mtlList<int>	m_undo;
private:
	bool IsEndOfFile(int i) const { return i >= m_size || i < 0; }
	void ClearTrailingWhitespaces( void );
public:
					mtlParser( void );
	explicit		mtlParser(const mtlChars &p_buffer);
	void			SetBuffer(const mtlChars &p_buffer);
	static bool		BufferFile(const mtlDirectory &p_file, mtlString &p_buffer);
	char			ReadChar(bool skipWhite=true);
	char			PeekChar(bool skipWhite=true) const;
	void			BackChar(bool skipWhite=true);
	void			SkipChar(bool skipWhite=true);
	mtlSubstring	ReadWord( void );
	mtlSubstring	PeekWord( void ) const;
	void			BackWord( void );
	void			SkipWord( void );
	mtlSubstring	ReadLine( void );
	mtlSubstring	PeekLine( void ) const;
	void			BackLine( void );
	void			SkipLine( void );
	bool			IsEndOfFile( void ) const { return IsEndOfFile(m_reader); }
	int				GetCharCount( void ) const { return m_size; }
	int				GetCurrentPosition( void ) const { return m_reader; }
	int				GetCharsLeft( void ) const { return m_size - m_reader; }
	void			JumpToStart( void ) { m_reader = 0; }
	void			JumpToEndChar( void ) { m_reader = m_size-1; }
	void			JumpToEndWord( void );
	void			JumpToEndLine( void );
	void			JumpToIndex(int index) { m_reader = index; }
	bool			JumpToChar(const mtlChars &p_chars);
	bool			JumpToCharBack(const mtlChars &p_chars);
	int				UndoCount( void ) const { return m_undo.GetSize(); }
	void			UndoRead( void );
	//mtlSubstring	GetEnclosedString(char closing, bool allowEscape=true, bool allowMultiLine=false); // the user must manually determine if the opening character has been passed
};

#endif
