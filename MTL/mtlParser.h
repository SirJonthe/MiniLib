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
	int			m_size; // size can be smaller than m_buffer->GetSize() (skips trailing whitespaces)
	int			m_reader;

private:
	bool				IsEndOfFile(int i) const { return i >= m_size || i < 0; }
	void				ClearTrailingWhitespaces( void );
	ExpressionResult	VerifyInputExpression(const mtlChars &expr) const;

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

	mtlChars		ReadTo(const mtlChars &p_str);
	mtlChars		PeekTo(const mtlChars &p_str);
	//void			BackTo(const mtlChars &p_str);

	mtlChars		ReadToAny(const mtlChars &p_chars);
	mtlChars		PeekToAny(const mtlChars &p_chars);
	void			BackToAny(const mtlChars &p_chars);

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

	/*bool Match("%s = { %f, %f, %f };", mtlList<mtlString> &out);
		// a string (any)
		// an equals sign
		// an opening brace
		// three floats separated by commas
		// a closing brace
		// a semi-colon
		// NOTE: %i, %f, %b, %s (int, float, bool, string)
			// NOTE: values will be returned in 'out'
			// NOTE: can specify if white spaces should be ignored - may even search multiple lines of text
		// NOTE: strings are terminated by the following character in the search string, i.e. "{%s}" is terminated by '}'
		// NOTE: does brace matching for strings, i.e. searching "function(function(), param)" with "function(%s)" returns "function(), param" rather than "function("
		// NOTE: returns FALSE if no match was found
		// NOTE: search always begins from current position forward (doesn't just look for match wherever it can find it)

		bool Expression(int &out, const mtlList<mtlString> &var, const mtlList<int> &val);
		bool Expressions(float &out, const mtlList<mtlString> &def, const mtlList<float> &val);
		// parses a string, treats it as a mathematical expression
		// calculates expression and retuns value in 'out'
		// uses strings in 'var' and values in 'val' to identify variables
		// IF FALSE IS RETURNED, THE CARET WILL NOT MOVE!!!
	*/

	ExpressionResult Match(const mtlChars &expr, mtlList<mtlChars> &out);
};

#endif
