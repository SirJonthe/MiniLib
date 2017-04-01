#ifndef MTL_PARSER_H_INCLUDED__
#define MTL_PARSER_H_INCLUDED__

#include "mtlString.h"
#include "mtlList.h"
#include "mtlArray.h"
#include "mtlPath.h"

#define mtlOpenBracesStr	"([{\'\""
#define mtlClosedBracesStr	")]}\'\""
#define mtlWhitespacesStr	" \t\n\r"
#define mtlNewlinesStr		"\n\r"
#define mtlIntStr			"0123456789"
#define mtlFloatStr			"0123456789."
#define mtlHexStr			"0123456789abcdef"
#define mtlBinStr			"01"
#define mtlAlphaStr			"abcdefghijklmnopqrstuvwxyz_"
#define mtlAlphanumStr		"abcdefghijklmnopqrstuvwxyz0123456789_"

class mtlBinaryParser
{
private:
	mtlString m_copy;
	mtlChars  m_buffer;
	int       m_reader;

public:
	mtlBinaryParser( void );

	void     SetBuffer(const mtlChars &buffer);
	void     CopyBuffer(const mtlChars &buffer);

	int      GetBufferSize( void ) const;
	int      GetBytesLeft( void ) const;
	int      GetIndex( void ) const;
	bool     IsEnd( void ) const;

	mtlChars Read(int count = 1);

	void     SetIndex(int index);
	void     StepIndex(int step);
	int      FindSignature(const mtlChars &signature) const;
};

class mtlSyntaxParser
{
private:
	mtlString     m_copy;
	mtlChars      m_buffer;
	int           m_reader;
	mtlList<char> m_brace_stack;
	int           m_line;
	char          m_quote_char;
	char          m_prev_non_white;
	bool          m_case_sensitive;

private:
	enum TokenType
	{
		Token_EndOfStream = 256,
		Token_Char,             // c
		Token_Alpha,            // a
		Token_Int,              // i
		Token_Real,             // r
		Token_Word,             // w
		Token_NullStr,          // s
		Token_Str,              // S
		Token_Split,            // |
		Token_NullOpt,          // ?
		Token_Opt,              // !
		Token_Any,              // &
		Token_AnyNot,           // ~
		Token_ComplexDelimiter, // .
		Token_Wild              // *
	};

	struct Token
	{
		TokenType type;
		mtlChars  str;
	};

public:
	enum ExpressionResult
	{
		ExpressionNotFound   = -1,
		ExpressionInputError = -2
	};

private:
	bool     IsFormat(char ch, short token) const;
	bool     IsFormat(char ch, const mtlChars &format) const;
	void     SkipWhitespaces( void );
	short    PeekNextNonWhiteChar( void ) const;
	short    GetChar(int reader) const;
	short    ReadChar( void );
	short    PeekChar( void ) const;
	short    ClassifyToken(short token) const;
	short    ReadToken( void );
	short    PeekToken( void ) const;
	bool     VerifyBraces(const mtlChars &str) const;
	int      CountVariables(const mtlChars &str) const;
	mtlChars ReadAny(const mtlChars &expr);
	mtlChars ReadTo(short token);
	void     SplitExpressions(const mtlChars &expr, mtlList<mtlChars> &out) const;
	int      MatchSingle(const mtlChars &expr, mtlArray<mtlChars> &out, mtlChars *seq = NULL);
	mtlChars OptMatch(const mtlChars &expr);

public:
	static bool BufferFile(const mtlPath &p_file, mtlString &p_buffer);

	mtlSyntaxParser( void );

	void EnableCaseSensitivity( void );
	void DisableCaseSensitivity( void );
	bool IsCaseSensitive( void ) const;

	void SetBuffer(const mtlChars &buffer);
	void CopyBuffer(const mtlChars &buffer);

	bool IsEnd( void ) const;
	bool InQuote( void ) const;

	int  GetBraceDepth( void ) const;
	int  GetBraceDepth(char brace_type) const;

	int  GetBufferSize( void ) const;
	int  GetBufferSizeRemaining( void ) const;

	int  GetLineIndex( void ) const;
	int  GetCharIndex( void ) const;

	const mtlChars &GetBuffer( void ) const;
	mtlChars GetBufferRemaining( void ) const;

	int Match(const mtlChars &expr, mtlArray<mtlChars> &out, mtlChars *seq = NULL);
	int Match(const mtlChars &expr, mtlChars *seq = NULL);
};

class mtlSyntaxParser2
{
private:
	enum CharType
	{
		CharType_Stop,
		CharType_Alphanum,
		CharType_Other
	};

	struct Index
	{
		int      pos;
		CharType typ;
		short    ch;
	};

	enum TokenType
	{
		Token_EndOfStream = 256,
		//Token_Char,             // c
		Token_Alpha,            // a
		Token_Int,              // i
		Token_Real,             // r
		Token_Word,             // w
		Token_NullStr,          // s
		Token_Str,              // S
		Token_Split,            // |
		Token_Opt,              // ?
		Token_Alt,              // !
		//Token_Any,              // &
		//Token_AnyNot,           // ~
		//Token_ComplexDelimiter, // .
		//Token_Wild              // *
	};

	struct Token
	{
		TokenType type;
		mtlChars  str;
	};

private:
	mtlString     m_copy;
	mtlChars      m_buffer;
	mtlChars      m_hyphenators;
	mtlList<char> m_brace_stack;
	Index         m_index;
	int           m_line;
	bool          m_is_case_sensitive;
	char          m_quote_char;

private:
	CharType ClassifyChar(char ch) const;
	short    ClassifyToken(short ch) const;
	Index    PeekChar( void ) const;
	short    ReadChar( void );
	Index    PeekToken( void ) const;
	short    ReadToken( void );
	bool     InQuote( void ) const;
	void     SplitExpressions(const mtlChars &expr, mtlList<mtlChars> &out) const;
	int      MatchSingle(const mtlChars &expr, mtlArray<mtlChars> &out, mtlChars *seq = NULL);
	int      CountVariables(const mtlChars &str) const;
	bool     VerifyBraces(const mtlChars &str) const;
	bool     IsFormat(short ch, short token) const;
	bool     IsFormat(short ch, const mtlChars &format) const;
	mtlChars ReadAny(const mtlChars &format);
	mtlChars ReadTo(short token);
	mtlChars OptMatch(const mtlChars &expr);

public:
	enum ExpressionResult
	{
		ExpressionNotFound   = -1,
		ExpressionInputError = -2
	};

public:
	mtlSyntaxParser2( void );

	void SetBuffer(const mtlChars &buffer, int line_offset = 0);
	void CopyBuffer(const mtlChars &buffer, int line_offset = 0);

	bool IsEnd( void ) const;

	void EnableCaseSensitivity( void );
	void DisableCaseSensitivity( void );
	bool IsCaseSensitive( void ) const;

	int  GetBraceDepth( void ) const;
	int  GetBraceDepth(char brace_type) const;

	int  GetBufferSize( void ) const;
	int  GetBufferSizeRemaining( void ) const;

	int  GetLineIndex( void ) const;
	int  GetCharIndex( void ) const;

	const mtlChars &GetBuffer( void ) const;
	mtlChars        GetBufferRemaining( void ) const;

	int Match(const mtlChars &expr, mtlArray<mtlChars> &out, mtlChars *seq = NULL);
	int Match(const mtlChars &expr, mtlChars *seq = NULL);

	char Debug_ReadChar( void ) { return (char)ReadChar(); }
};

#endif
