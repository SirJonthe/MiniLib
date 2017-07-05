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
	enum CharType
	{
		CharType_Stop,
		CharType_Alphanum,
		CharType_Other
	};

	struct Index
	{
		int      pos;
		int      line;
		int      match_line;
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
	mtlString     m_diag_str;
	bool          m_is_case_sensitive;
	bool          m_log_diag;
	char          m_quote_char;

private:
	CharType ClassifyChar(char ch) const;
	short    ClassifyToken(short ch) const;
	Index    PeekChar( void ) const;
	short    ReadChar( void );
	short    PeekStopToken( void ) const;
	short    ReadToken( void );
	bool     InQuote( void ) const;
	void     SplitExpressions(const mtlChars &expr, mtlList<mtlChars> &out) const;
	int      MatchSingle(const mtlChars &expr, mtlArray<mtlChars> &out, mtlChars *seq = NULL);
	int      CountVariables(const mtlChars &str) const;
	bool     VerifyBraces(const mtlChars &str) const;
	bool     IsFormat(short ch, short token) const;
	bool     IsFormat(short ch, const mtlChars &format) const;
	mtlChars ReadAny(const mtlChars &format1, const mtlChars &format2 = mtlChars(""));
	mtlChars ReadWord( void );
	mtlChars ReadTo(short token);
	mtlChars OptMatch(const mtlChars &expr);
	void     ClearLog( void );
	void     LogStr(const mtlChars &str);
	void     LogCompactStr(const mtlChars &str);
	void     LogToken(short token);
	void     LogChar(char ch);

public:
	enum ExpressionResult
	{
		ExpressionNotFound   = -1,
		ExpressionInputError = -2
	};

public:
	static bool BufferFile(const mtlPath &p_file, mtlString &p_buffer);

	mtlSyntaxParser( void );

	void SetBuffer(const mtlChars &buffer, int line_offset = 0);
	void CopyBuffer(const mtlChars &buffer, int line_offset = 0);
	void ClearBuffer( void );
	void SetHyphenators(const mtlChars &hyphenators);

	bool IsEnd( void ) const;

	void EnableCaseSensitivity( void );
	void DisableCaseSensitivity( void );
	bool IsCaseSensitive( void ) const;

	void EnableDiagnostics( void );
	void DisableDiagnostics( void );

	int  GetBraceDepth( void ) const;
	int  GetBraceDepth(char brace_type) const;

	int  GetBufferSize( void ) const;
	int  GetBufferSizeRemaining( void ) const;

	int  GetLineIndex( void ) const;
	int  GetCharIndex( void ) const;
	int  GetMatchLineIndex( void ) const;

	const mtlChars &GetBuffer( void ) const;
	mtlChars        GetBufferRemaining( void ) const;

	const mtlChars &GetDiagnostics( void ) const;

	int Match(const mtlChars &expr, mtlArray<mtlChars> &out, mtlChars *seq = NULL);
	int Match(const mtlChars &expr, mtlChars *seq = NULL);

	char Debug_ReadChar( void ) { return (char)ReadChar(); }
};

#endif
