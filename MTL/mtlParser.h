#ifndef MTL_PARSER_H_INCLUDED__
#define MTL_PARSER_H_INCLUDED__

#include "mtlString.h"
#include "mtlList.h"
#include "mtlDirectory.h"

class mtlParser
{
private:
	static const char space = ' ';
	static const char tab = '\t';
	static const char newl = '\n';
	static const char cret = '\r';
	static const char esc = '\\';
	static const char var = '%';

public:
	static bool IsWhite(char c) { return c == space || c == tab || c == newl || c == cret; }
	static bool IsNewl(char c) { return c == newl || c == cret; }
	static bool IsLetter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }

	enum ExpressionResult
	{
		ExpressionNotFound,
		ExpressionInputError, // %s%s (two or more types without delimiters)
		ExpressionTypeMismatch, // %i evaluates to float/string/bool(non 0/1), %b evaluates to float/string/int(non 0/1), %f evaluates to string/bool(non 0/1)/int(if out of range)
		ExpressionUnbalancedBraces, // {%s}, where s = "string [ string", no closing brace (example)
		ExpressionValid,
		ExpressionFound
	};

private:
	mtlChars	m_buffer;
	int			m_reader;

private:
	bool				IsEnd(int i) const { return i >= m_buffer.GetSize() || i < 0; }
	ExpressionResult	VerifyInputExpression(const mtlChars &expr) const;

public:
					mtlParser( void );
	explicit		mtlParser(const mtlChars &p_buffer);

	const mtlChars &GetBuffer( void ) const;
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

	mtlChars		ReadTo(const mtlChars &p_str);
	mtlChars		PeekTo(const mtlChars &p_str);
	//void			BackTo(const mtlChars &p_str);
	int				IndexOf(const mtlChars &p_chars, bool caseSensitive=false) const;

	mtlChars		ReadToAny(const mtlChars &p_chars);
	mtlChars		PeekToAny(const mtlChars &p_chars);
	void			BackToAny(const mtlChars &p_chars);
	int				IndexOfAny(const mtlChars &p_chars, bool caseSensitive=false) const;

	mtlChars		ReadRest( void );
	mtlChars		PeekRest( void ) const;

	bool			IsEnd( void ) const { return IsEnd(m_reader); }
	int				GetCharCount( void ) const { return m_buffer.GetSize(); }
	int				GetCurrentPosition( void ) const { return m_reader; }
	int				GetCharsLeft( void ) const { return m_buffer.GetSize() - m_reader; }
	int				GetLineNumber( void ) const;

	void			JumpToStart( void ) { m_reader = 0; }
	void			JumpToEndChar( void ) { m_reader = m_buffer.GetSize() - 1; }
	void			JumpToEndWord( void );
	void			JumpToEndLine( void );
	void			JumpToIndex(int index) { m_reader = index; }

	bool			JumpToChar(const mtlChars &p_chars);
	bool			JumpToCharBack(const mtlChars &p_chars);

	void			ConsumeWhitespaces( void );

	ExpressionResult Match(const mtlChars &expr, mtlList<mtlChars> &out);
};

#endif
