#ifndef MTL_PARSER_H_INCLUDED__
#define MTL_PARSER_H_INCLUDED__

#include "mtlString.h"
#include "mtlList.h"
#include "mtlDirectory.h"

// In order to implement reading enclosed strings
// I must ensure that the file is read sequentially
// forward/backward.
// Jumping arbitrarily is strictly prohibited
// Undo might have to be removed.

class mtlParser
{
private:
	static const char			space = ' ';
	static const char			tab = '\t';
	static const char			newl = '\n';
	static const char			cret = '\r';
	static const char			esc = '\\';
	static const unsigned int	nlbit = 1;
	static const unsigned int	nwbit = 2;
	/*struct Enclosing
	{
		char ch;
		int loc;
	};*/
public:
	static bool IsWhite(char c) { return c == space || c == tab || c == newl || c == cret; }
	static bool IsNewl(char c) { return c == newl || c == cret; }
private:
	mtlChars				m_buffer;
	int						m_size; // size can be smaller than m_buffer->GetSize() (skips trailing whitespaces)
	int						m_reader;
	//mtlList<Enclosing>	m_enclosing; // stack, in order to implement this, user must process each passed character
	//mtlString				m_separators; // treated as whitespaces when parsing
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
	mtlChars		ReadWord( void );
	mtlChars		PeekWord( void ) const;
	void			BackWord( void );
	void			SkipWord( void );
	mtlChars		ReadLine( void );
	mtlChars		PeekLine( void ) const;
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
	//bool			JumpToWord(const mtlChars &p_word); // only searches for WORD (i.e. separated by whitespaces)
	//bool			JumpToWordBack(const mtlChars &p_word);
	//bool			JumpToString(const mtlChars &p_string);
	//bool			JumpToStringBack(const mtlChars &p_string);
	//mtlChars		ReadEnclosure(int level, bool trimWhite=true) const;
	//mtlChars		ReadEnclosure(bool trimWhite=true) const;
	//mtlChars		ReadToChar(const mtlChars &p_chars, bool trimWhite=true); // read everything from current position to character
	//mtlChars		PeekToChar(const mtlChars &p_chars, bool trimWhite=true);
	//mtlChars		ReadToWord(const mtlChars &p_word, bool trimWhite=true);
	//mtlChars		PeekToWord(const mtlChars &p_word, bool trimWhite=true);
	//mtlChars		ReadToString(const mtlChars &p_string, bool trimWhite=true);
	//mtlChars		PeekToString(const mtlChars &p_string, bool trimWhite=true);
	
	// Maybe functions for setting custom separators (treated as whitespaces)
	// This enables the string:
	// variable=value
	// to be read as "variable" and "value" is a separator is "="
};

#endif
