#include <fstream>
#include "mtlParser.h"

// Input    "This is a string! (btw, I love strings)"
// Read as: "|this|is|a|string|!|(|btw|,|I|love|strings|)|\0"

mtlBinaryParser::mtlBinaryParser( void ) : m_copy(), m_buffer(), m_reader(0) {}

void mtlBinaryParser::SetBuffer(const mtlChars &buffer)
{
	m_copy.Free();
	m_buffer = buffer;
	m_reader = 0;
}

void mtlBinaryParser::CopyBuffer(const mtlChars &buffer)
{
	m_copy.Copy(buffer);
	m_buffer = m_copy;
	m_reader = 0;
}

int mtlBinaryParser::GetBufferSize( void ) const
{
	return m_buffer.GetSize();
}

int mtlBinaryParser::GetBytesLeft( void ) const
{
	return m_buffer.GetSize() - m_reader;
}

int mtlBinaryParser::GetIndex( void ) const
{
	return m_reader;
}

bool mtlBinaryParser::IsEnd( void ) const
{
	return m_reader >= m_buffer.GetSize() || m_reader < 0;
}

mtlChars mtlBinaryParser::Read(int count)
{
	int a = m_reader;
	StepIndex(count);
	int b = m_reader;
	return mtlChars(m_buffer, a < b ? a : b, a > b ? a : b);
}

void mtlBinaryParser::SetIndex(int index)
{
	m_reader = index < 0 ? -1 : (index >= m_buffer.GetSize() ? m_buffer.GetSize() : index);
}

void mtlBinaryParser::StepIndex(int step)
{
	SetIndex(m_reader + step);
}

int mtlBinaryParser::FindSignature(const mtlChars &signature) const
{
	return mtlChars(m_buffer, m_reader, m_buffer.GetSize()).FindFirstString(signature);
}

#define OpenBraces   "([{"
#define ClosedBraces ")]}"
#define Quotes       "\'\""
//#define Escape       '\\'
#define Variable     '%'

/*bool mtlSyntaxParser::IsFormat(char ch, short token) const
{
	bool match = false;
	switch (token) {

	case Token_Alpha:
		match = mtlChars::IsAlpha(ch);
		break;

	case Token_Int:
		match = mtlChars::IsNumeric(ch);
		break;

	default:
		match = mtlChars::ToLower(ch) == mtlChars::ToLower((char)token);
		break;
	}
	return match;
}

bool mtlSyntaxParser::IsFormat(char ch, const mtlChars &format) const
{
	mtlSyntaxParser parser;
	parser.SetBuffer(format);
	bool match = false;
	short token;

	while (!parser.IsEnd() && !match) {
		token = parser.ReadToken();
		match = IsFormat(ch, token);
	}

	return match;
}

void mtlSyntaxParser::SkipWhitespaces( void )
{
	while (!IsEnd() && mtlChars::IsWhitespace(m_buffer[m_reader])) {
		if (mtlChars::IsNewline(m_buffer[m_reader])) {
			++m_line;
		}
		++m_reader;
	}
}

short mtlSyntaxParser::PeekNextNonWhiteChar( void ) const
{
	int reader = m_reader + 1;
	while (reader < m_buffer.GetSize() && mtlChars::IsWhitespace(m_buffer[reader])) {
		++reader;
	}
	return GetChar(reader);
}

short mtlSyntaxParser::GetChar(int reader) const
{
	return reader < m_buffer.GetSize() ? (short)m_buffer[reader] : (short)Token_EndOfStream;
}

short mtlSyntaxParser::ReadChar( void )
{
	if (IsEnd()) { return (short)Token_EndOfStream; }

	short ch = (short)m_buffer[m_reader++];

	if (!InQuote() && mtlChars::IsWhitespace((char)ch)) {
		SkipWhitespaces();
		short next_non_white_char = GetChar(m_reader);
		if (mtlChars::IsAlphanumeric(m_prev_non_white) && mtlChars::IsAlphanumeric((char)next_non_white_char)) {
			ch = (short)' ';
		} else {
			ch = next_non_white_char;
			++m_reader;
		}
	}

	if (!mtlChars::IsWhitespace((char)ch)) { // in quote *or* separator for two alphanum words
		m_prev_non_white = ch;
	}

	int quote_index = -1;
	if ((quote_index = mtlChars::SameAsWhich(ch, Quotes, sizeof(Quotes))) != -1) {
		if (!InQuote()) {
			m_quote_char = Quotes[quote_index];
		} else if (Quotes[quote_index] == m_quote_char) {
			m_quote_char = 0;
		}
	} else {
		int open_index = -1;
		int closed_index = -1;
		if ((open_index = mtlChars::SameAsWhich(ch, OpenBraces, sizeof(OpenBraces))) != -1) {
			m_brace_stack.AddLast(OpenBraces[open_index]);
		} else if ((closed_index = mtlChars::SameAsWhich(ch, ClosedBraces, sizeof(ClosedBraces))) != -1) {
			open_index = mtlChars::SameAsWhich(m_brace_stack.GetLast()->GetItem(), OpenBraces, sizeof(OpenBraces));
			if (open_index == closed_index) {
				m_brace_stack.RemoveLast();
			}
		}
	}

	return ch;
}

short mtlSyntaxParser::PeekChar( void ) const
{
	if (IsEnd()) { return (short)Token_EndOfStream; }

	short ch = (short)m_buffer[m_reader];
	short next_non_white = PeekNextNonWhiteChar();

	if (mtlChars::IsWhitespace(ch) && !InQuote()) {
		ch = (mtlChars::IsAlphanumeric(m_prev_non_white) && mtlChars::IsAlphanumeric((char)next_non_white)) ? (short)' ' : next_non_white;
	}

	return (short)ch;
}

short mtlSyntaxParser::ClassifyToken(short token) const
{
	switch (token) {
	case '0':
		token = (short)Token_EndOfStream;
		break;
	case 'c':
		token = (short)Token_Char;
		break;
	case 'a':
		token = (short)Token_Alpha;
		break;
	case 'i':
		token = (short)Token_Int;
		break;
	case 'r':
		token = (short)Token_Real;
		break;
	case 'w':
		token = (short)Token_Word;
		break;
	case 's':
		token = (short)Token_NullStr;
		break;
	case 'S':
		token = (short)Token_Str;
		break;
	case '|':
		token = (short)Token_Split;
		break;
	case '?':
		token = (short)Token_NullOpt;
		break;
	case '!':
		token = (short)Token_Opt;
		break;
	case '&':
		token = (short)Token_Any;
		break;
	//case '~':
	//	token = (short)Token_AnyNot;
	//	break;
	//case '.':
	//	token = (short)Token_ComplexDelimiter;
	//	break;
	//case '*':
	//	token = (short)Token_Wild;
	//	break;
	case Variable:
	default:
		break;
	}
	return token;
}

short mtlSyntaxParser::ReadToken( void )
{
	int read_start = m_reader;
	short token = ReadChar();
	if (token == Variable) {
		token = ClassifyToken(ReadChar());
		if (token == Token_Opt) {
			m_reader = read_start;
		}
	}
	return token;
}

short mtlSyntaxParser::PeekToken( void ) const
{
	// A bit of a hack, but I don't want to rewrite a bunch of stuff
	mtlSyntaxParser p;
	p.m_buffer         = m_buffer;
	p.m_reader         = m_reader;
	p.m_quote_char     = m_quote_char;
	p.m_prev_non_white = m_prev_non_white;
	short token = p.PeekChar();
	if (token == Variable) {
		++p.m_reader;
		token = ClassifyToken(p.PeekChar());
		--p.m_reader;
	}
	return token;
}

bool mtlSyntaxParser::VerifyBraces(const mtlChars &str) const
{
	mtlSyntaxParser parser;
	parser.SetBuffer(str);
	while (!parser.IsEnd()) {
		parser.ReadChar();
	}
	return parser.GetBraceDepth() == 0;
}

int mtlSyntaxParser::CountVariables(const mtlChars &str) const
{
	mtlSyntaxParser p;
	p.SetBuffer(str);
	int count = 0;
	while (!p.IsEnd()) {
		short token = p.ReadToken();
		if (token > 255) {
			if (token == Token_EndOfStream) { break; }
			++count;
		}
	}
	return count;
}

mtlChars mtlSyntaxParser::ReadAny(const mtlChars &format)
{
	if (IsEnd()) { return mtlChars(); }

	const int start = m_reader;

	if (format.GetSize() > 0) {
		while (!IsEnd()) {
			const char next_ch = (char)PeekChar();
			if (!IsFormat(next_ch, format)) { break; }
			ReadChar();
		}
	} else {
		ReadChar();
	}

	const int end = m_reader;
	SkipWhitespaces();
	return mtlChars(m_buffer, start, end);
}

mtlChars mtlSyntaxParser::ReadTo(short token)
{
	int brace_depth = GetBraceDepth();
	int start = m_reader;

	while (!IsEnd()) {
		if (GetBraceDepth() == brace_depth && IsFormat((char)PeekChar(), token)) { break; }
		ReadChar();
	}

	mtlChars str = mtlChars(m_buffer, start, m_reader).GetTrimmed();
	SkipWhitespaces();
	return str;
}

void mtlSyntaxParser::SplitExpressions(const mtlChars &expr, mtlList<mtlChars> &out) const
{
	mtlSyntaxParser p;
	p.SetBuffer(expr);

	int start = p.m_reader;

	while (!p.IsEnd()) {
		int end = p.m_reader;
		short token = p.ReadToken();
		if (p.m_brace_stack.GetSize() == 0 && token == Token_Split) {
			out.AddLast(mtlChars(p.m_buffer, start, end).GetTrimmed());
			start = p.m_reader;
		} else if (p.IsEnd()) {
			out.AddLast(mtlChars(p.m_buffer, start, p.m_reader).GetTrimmed());
			start = p.m_reader;
		}
	}
}

int mtlSyntaxParser::MatchSingle(const mtlChars &expr, mtlArray<mtlChars> &out, mtlChars *seq)
{
	mtlItem<char> *brace_item = m_brace_stack.GetLast();

	if (!VerifyBraces(expr)) {
		return (int)ExpressionInputError;
	}

	out.Free();
	const int variable_count = CountVariables(expr);
	out.SetCapacity(variable_count);

	mtlSyntaxParser expr_parser;
	expr_parser.SetBuffer(expr);

	int result      = 1;
	int start       = m_reader;
	int brace_depth = GetBraceDepth();

	while (!expr_parser.IsEnd() && result == 1) {

		short expr_token = expr_parser.ReadToken();
		if (IsEnd()) {
			if (expr_token != Token_EndOfStream) {
				result = (int)ExpressionNotFound;
			}
			break;
		}

		bool test_len = false;

		switch (expr_token) {
		case Token_Char:
			out.Add(ReadAny(""));
			test_len = true;
			break;

		case Token_Alpha:
			out.Add(ReadAny("%a").GetTrimmed());
			test_len = true;
			break;

		case Token_Int:
			out.Add(ReadAny("%i").GetTrimmed());
			test_len = true;
			break;

		case Token_Real:
			{
				mtlChars real = ReadAny("%i.").GetTrimmed();
				int dec_delim = 0;
				for (int i = 0; i < real.GetSize() && dec_delim < 2; ++i) {
					if (real[i] == '.') { ++dec_delim; }
				}
				if (dec_delim == 1 && real.IsFloat()) {
					out.Add(real);
					test_len = true;
				} else {
					result = (int)ExpressionNotFound;
				}
				break;
			}

		case Token_Word:
			out.Add(ReadAny("%a%i_").GetTrimmed());
			test_len = true;
			break;

		case Token_Str:
			test_len = true;
		case Token_NullStr:
			{
				short next_expr_token = expr_parser.PeekToken();
				if (InQuote()) { out.Add(ReadTo(next_expr_token)); }
				else           { out.Add(ReadTo(next_expr_token).GetTrimmed()); }
				break;
			}

		case Token_Opt:
			test_len = true;
		case Token_NullOpt:
			{
				mtlArray<mtlChars> m;
				if (expr_parser.Match("(%s) %| %w", m) > -1) { out.Add(OptMatch(m[0]).GetTrimmed()); }
				else                                         { result = (int)ExpressionInputError; }
				break;
			}

		case Token_Any:
		{
			mtlArray<mtlChars> m;
			if (expr_parser.Match("(%s)", m) > -1) { out.Add(ReadAny(m[0]).GetTrimmed()); }
			else                                   { result = (int)ExpressionInputError; }
			break;
		}

		case Token_Split:
			result = (int)ExpressionInputError;
			break;

		case Token_EndOfStream:
		default:
			{
				char c1 = (char)ReadChar();
				char c2 = (char)expr_token;
				if (!InQuote() && !IsCaseSensitive()) {
					c1 = mtlChars::ToLower(c1);
					c2 = mtlChars::ToLower(c2);
				}
				if (c1 != c2) {
					result = (int)ExpressionNotFound;
				}
			}
		}

		if (test_len && out[out.GetSize() - 1].GetSize() < 1) {
			result = (int)ExpressionNotFound;
		}
	}
	if (result == 1 && GetBraceDepth() != brace_depth) {
		result = (int)ExpressionNotFound;
	}
	if (result != 1) {
		out.Free();
		m_reader = start;
	}
	if (seq != NULL) {
		*seq = mtlChars(m_buffer, start, m_reader);
		*seq = seq->GetTrimmed();
	}

	while (m_brace_stack.GetLast() != brace_item && m_brace_stack.GetSize() > 0) {
		m_brace_stack.RemoveLast();
	}

	if (result > 0) {
		SkipWhitespaces();
	}

	return result;
}

mtlChars mtlSyntaxParser::OptMatch(const mtlChars &expr)
{
	mtlChars seq;
	Match(expr, &seq);
	return seq;
}

bool mtlSyntaxParser::BufferFile(const mtlPath &p_file, mtlString &p_buffer)
{
	if (!p_file.IsFile()) { return false; }
	std::ifstream fin(p_file.GetPath().GetChars(), std::ios::ate|std::ios::binary);
	if (!fin.is_open()) { return false; }
	p_buffer.SetSize((int)fin.tellg());
	fin.seekg(0, std::ios::beg);
	return !fin.read(p_buffer.GetChars(), p_buffer.GetSize()).bad();
}

mtlSyntaxParser::mtlSyntaxParser( void ) : m_copy(), m_buffer(), m_reader(0), m_brace_stack(), m_line(0), m_quote_char(0), m_prev_non_white(0), m_case_sensitive(false)
{}

void mtlSyntaxParser::EnableCaseSensitivity( void )
{
	m_case_sensitive = true;
}

void mtlSyntaxParser::DisableCaseSensitivity( void )
{
	m_case_sensitive = false;
}

bool mtlSyntaxParser::IsCaseSensitive( void ) const
{
	return m_case_sensitive;
}

void mtlSyntaxParser::SetBuffer(const mtlChars &buffer)
{
	m_copy.Free();
	m_buffer = buffer;
	m_reader = 0;
	m_brace_stack.RemoveAll();
	m_quote_char = 0;
	m_prev_non_white = 0;
	SkipWhitespaces();
}

void mtlSyntaxParser::CopyBuffer(const mtlChars &buffer)
{
	m_copy.Copy(buffer);
	m_buffer = m_copy;
	m_reader = 0;
	m_brace_stack.RemoveAll();
	m_quote_char = 0;
	m_prev_non_white = 0;
	SkipWhitespaces();
}

bool mtlSyntaxParser::IsEnd( void ) const
{
	return m_reader >= m_buffer.GetSize();
}

bool mtlSyntaxParser::InQuote( void ) const
{
	return m_quote_char != 0;
}

int mtlSyntaxParser::GetBraceDepth( void ) const
{
	return m_brace_stack.GetSize();
}

int mtlSyntaxParser::GetBraceDepth(char brace_type) const
{
	const mtlItem<char> *iter = m_brace_stack.GetFirst();
	int depth = 0;
	while (iter != NULL) {
		if (iter->GetItem() == brace_type) {
			++depth;
		}
		iter = iter->GetNext();
	}
	return depth;
}

int mtlSyntaxParser::GetBufferSize( void ) const
{
	return m_buffer.GetSize();
}

int mtlSyntaxParser::GetBufferSizeRemaining( void ) const
{
	return m_buffer.GetSize() - m_reader;
}

int mtlSyntaxParser::GetLineIndex( void ) const
{
	return m_line;
}

int mtlSyntaxParser::GetCharIndex( void ) const
{
	return m_reader;
}

const mtlChars &mtlSyntaxParser::GetBuffer( void ) const
{
	return m_buffer;
}

mtlChars mtlSyntaxParser::GetBufferRemaining( void ) const
{
	return mtlChars(m_buffer, m_reader, m_buffer.GetSize());
}

int mtlSyntaxParser::Match(const mtlChars &expr, mtlArray<mtlChars> &out, mtlChars *seq)
{
	mtlList<mtlChars> exprs;
	SplitExpressions(expr, exprs);
	mtlItem<mtlChars> *expr_iter = exprs.GetFirst();

	int i = 0;
	while (expr_iter != NULL) {
		int code = MatchSingle(expr_iter->GetItem(), out, seq);
		switch (code) {
		case 1:                    return i;
		case ExpressionInputError: return (int)ExpressionInputError;
		default:                   break;
		}
		++i;
		expr_iter = expr_iter->GetNext();
	}
	return (int)ExpressionNotFound;
}

int mtlSyntaxParser::Match(const mtlChars &expr, mtlChars *seq)
{
	mtlArray<mtlChars> m;
	return Match(expr, m, seq);
}*/

mtlSyntaxParser::CharType mtlSyntaxParser::ClassifyChar(char ch) const
{
	if (m_hyphenators.SameAsAny(ch) || mtlChars::IsAlphanumeric(ch)) {
		return CharType_Alphanum;
	} else if (mtlChars::IsWhitespace(ch)) {
		return CharType_Stop;
	}
	return CharType_Other;
}

short mtlSyntaxParser::ClassifyToken(short token) const
{
	switch (token) {
	case '0':
		token = (short)Token_EndOfStream;
		break;
	case 'a':
		token = (short)Token_Alpha;
		break;
	case 'i':
		token = (short)Token_Int;
		break;
	case 'r':
		token = (short)Token_Real;
		break;
	case 'w':
		token = (short)Token_Word;
		break;
	case 's':
		token = (short)Token_NullStr;
		break;
	case 'S':
		token = (short)Token_Str;
		break;
	case '|':
		token = (short)Token_Split;
		break;
	case '?':
		token = (short)Token_Opt;
		break;
	case '!':
		token = (short)Token_Alt;
		break;
	//case '&':
	//	token = (short)Token_Any;
	//	break;
	//case '~':
	//	token = (short)Token_AnyNot;
	//	break;
	//case '.':
	//	token = (short)Token_ComplexDelimiter;
	//	break;
	//case '*':
	//	token = (short)Token_Wild;
	//	break;
	case Variable:
	default:
		break;
	}
	return token;
}

mtlSyntaxParser::Index mtlSyntaxParser::PeekChar( void ) const
{
	Index i;
	i.pos = m_index.pos;

	if (IsEnd()) {

		i.ch = (short)Token_EndOfStream;
		i.typ = CharType_Stop;
		i.pos = m_buffer.GetSize();

	} else {

		if (m_index.typ == CharType_Stop) {
			do {
				i.ch = (short)m_buffer[i.pos];
				i.typ = ClassifyChar((char)i.ch);
				i.pos = i.pos + 1;
			} while (i.typ == CharType_Stop);
		} else {
			i.ch = (short)m_buffer[i.pos];
			i.typ = ClassifyChar((char)i.ch);
			if (m_index.typ != i.typ || m_index.typ == CharType_Other) {
				i.ch = (short)' ';
				i.typ = ClassifyChar((char)i.ch);
			} else  {
				i.pos = i.pos + 1;
			}
		}

		if (!IsCaseSensitive() && !InQuote()) {
			i.ch = (short)mtlChars::ToLower((char)i.ch);
		}

	}

	return i;
}

short mtlSyntaxParser::ReadChar( void )
{
	m_index = PeekChar();
	if (m_index.ch == (short)Token_EndOfStream) { return m_index.ch; }

	int quote_index = -1;
	if ((quote_index = mtlChars::SameAsWhich(m_index.ch, Quotes, sizeof(Quotes))) != -1) {
		if (!InQuote()) {
			m_quote_char = Quotes[quote_index];
		} else if (Quotes[quote_index] == m_quote_char) {
			m_quote_char = 0;
		}
	} else {
		int open_index = -1;
		int closed_index = -1;
		if ((open_index = mtlChars::SameAsWhich(m_index.ch, OpenBraces, sizeof(OpenBraces))) != -1) {
			m_brace_stack.AddLast(OpenBraces[open_index]);
		} else if ((closed_index = mtlChars::SameAsWhich(m_index.ch, ClosedBraces, sizeof(ClosedBraces))) != -1) {
			open_index = mtlChars::SameAsWhich(m_brace_stack.GetLast()->GetItem(), OpenBraces, sizeof(OpenBraces));
			if (open_index == closed_index) {
				m_brace_stack.RemoveLast();
			}
		}
	}

	return m_index.ch;
}

short mtlSyntaxParser::PeekStopToken( void ) const
{
	mtlSyntaxParser p;
	p.m_buffer            = m_buffer;
	p.m_index             = m_index;
	p.m_quote_char        = m_quote_char;
	p.m_is_case_sensitive = m_is_case_sensitive;
	p.m_hyphenators       = m_hyphenators;
	p.m_brace_stack.Copy(m_brace_stack);
	short token;
	do {
		token = p.ReadToken();
	} while (token == (short)' ');
	return token;
}

short mtlSyntaxParser::ReadToken( void )
{
	//int read_start = m_index.pos;
	ReadChar();
	if (m_index.ch == Variable) {
		bool case_sensitivity = IsCaseSensitive();
		DisableCaseSensitivity();
		do {
			ReadChar();
		} while (m_index.typ == CharType_Stop);
		if (case_sensitivity) {
			EnableCaseSensitivity();
		}
		m_index.ch = ClassifyToken(m_index.ch);
		m_index.typ = CharType_Other; // the token is the type, not an alphanum or stop
		// BUG HERE
		//if (m_index.ch == (short)Token_Opt || m_index.ch == (short)Token_Alt) {
		//	m_index.pos = read_start;
		//}
	}
	return m_index.ch;
}

bool mtlSyntaxParser::InQuote( void ) const
{
	return m_quote_char != 0;
}

void mtlSyntaxParser::SplitExpressions(const mtlChars &expr, mtlList<mtlChars> &out) const
{
	mtlSyntaxParser p;
	p.SetBuffer(expr);

	int start = p.m_index.pos;

	while (!p.IsEnd()) {
		int end = p.m_index.pos;
		short token = p.ReadToken();
		if (p.m_brace_stack.GetSize() == 0 && token == Token_Split) {
			out.AddLast(mtlChars(p.m_buffer, start, end).GetTrimmed());
			start = p.m_index.pos;
		} else if (p.IsEnd()) {
			out.AddLast(mtlChars(p.m_buffer, start, p.m_index.pos).GetTrimmed());
			start = p.m_index.pos;
		}
	}
}

int mtlSyntaxParser::MatchSingle(const mtlChars &expr, mtlArray<mtlChars> &out, mtlChars *seq)
{
	m_index.typ = CharType_Other;
	mtlItem<char> *brace_item = m_brace_stack.GetLast();

	LogStr("matching: ");
	LogCompactStr(expr);
	LogStr("...\n");

	if (!VerifyBraces(expr)) {
		return (int)ExpressionInputError;
	}

	out.Free();
	const int variable_count = CountVariables(expr);
	out.SetCapacity(variable_count);

	mtlSyntaxParser expr_parser;
	expr_parser.m_is_case_sensitive = m_is_case_sensitive;
	expr_parser.m_hyphenators = m_hyphenators;
	expr_parser.SetBuffer(expr);

	int   result      = 1;
	Index start       = m_index;
	int   brace_depth = GetBraceDepth();

	while (!expr_parser.IsEnd() && result == 1) {

		short expr_token = expr_parser.ReadToken();

		LogStr("  looking for ");
		LogToken(expr_token);
		LogStr("...");

		if (IsEnd()) {
			if (expr_token != Token_EndOfStream) {
				result = (int)ExpressionNotFound;
			}
			break;
		}

		bool test_len = false;

		switch (expr_token) {

		case Token_Alpha:
			out.Add(ReadAny("%a").GetTrimmed());
			test_len = true;
			LogStr("reading alpha, found ");
			LogCompactStr(out[out.GetSize() - 1]);
			break;

		case Token_Int:
		{
			mtlChars hyp = m_hyphenators;
			m_hyphenators = "+-";
			mtlChars integer = ReadAny("+-%i").GetTrimmed();
			if (integer.IsInt()) {
				out.Add(integer);
				test_len = true;
			} else {
				result = (int)ExpressionNotFound;
			}
			LogStr("reading int, found ");
			LogCompactStr(integer);
			m_hyphenators = hyp;
			break;
		}

		case Token_Real:
			{
				mtlChars hyp = m_hyphenators;
				m_hyphenators = "+-.";
				mtlChars real = ReadAny("+-.%i").GetTrimmed();
				if (real.IsFloat()) {
					out.Add(real);
					test_len = true;
				} else {
					result = (int)ExpressionNotFound;
				}
				LogStr("reading int, found ");
				LogCompactStr(real);
				m_hyphenators = hyp;
				break;
			}

		case Token_Word:
			out.Add(ReadAny("%a%i", m_hyphenators).GetTrimmed());
			test_len = true;
			LogStr("reading complex word, found ");
			LogCompactStr(out[out.GetSize() - 1]);
			break;

		case Token_Str:
			test_len = true;
			LogStr("[non-optional] ");
		case Token_NullStr:
			{
				short next_expr_token = expr_parser.PeekStopToken();
				out.Add(ReadTo(next_expr_token));
				m_index.typ = CharType_Other;
				LogStr("reading complex string until ");
				LogToken(next_expr_token);
				LogStr(", found ");
				LogCompactStr(out[out.GetSize() - 1]);
				break;
			}

		case Token_Alt:
			test_len = true;
			LogStr("[non-optional] ");
		case Token_Opt:
			{
				LogStr("reading alt complex string, found ");
				mtlArray<mtlChars> m;
				if (expr_parser.MatchSingle("(%s)", m) > -1) {
					mtlChars match = OptMatch(m[0]).GetTrimmed();
					out.Add(match);
					LogCompactStr(match);
				} else {
					result = (int)ExpressionInputError;
					LogStr("input token error");
				}
				m_index.typ = expr_parser.m_index.typ = CharType_Stop;
				break;
			}

		case Token_Split:
			result = (int)ExpressionInputError;
			LogStr("found split, error");
			break;

		case Token_EndOfStream:
		default:
			{
				char c1 = (char)ReadChar();
				char c2 = (char)expr_token;
				LogStr("got ");
				LogChar(c1);
				LogStr(", ");
				if (c1 != c2) {
					result = (int)ExpressionNotFound;
					LogStr("no ");
				}
				LogStr("match");
			}
		}

		if (test_len && out[out.GetSize() - 1].GetSize() < 1) {
			result = (int)ExpressionNotFound;
			LogStr(", empty");
		}
		LogStr("\n");
	}
	if (result == 1 && GetBraceDepth() != brace_depth) {
		result = (int)ExpressionNotFound;
		LogStr("  mismatching braces\n");
	}
	if (result != 1) {
		out.Free();
		m_index = start;
	}
	if (seq != NULL) {
		*seq = mtlChars(m_buffer, start.pos, m_index.pos).GetTrimmed();
	}

	while (m_brace_stack.GetLast() != brace_item && m_brace_stack.GetSize() > 0) {
		m_brace_stack.RemoveLast();
	}

	if (result != 1) {
		LogStr("not found\n");
	} else {
		LogStr("found\n");
	}

	return result;
}

int mtlSyntaxParser::CountVariables(const mtlChars &str) const
{
	mtlSyntaxParser p;
	p.SetBuffer(str);
	int count = 0;
	while (!p.IsEnd()) {
		short token = p.ReadToken();
		if (token > 255) {
			if (token == Token_EndOfStream) { break; }
			++count;
		}
	}
	return count;
}

bool mtlSyntaxParser::VerifyBraces(const mtlChars &str) const
{
	mtlSyntaxParser parser;
	parser.SetBuffer(str);
	while (!parser.IsEnd()) {
		parser.ReadChar();
	}
	return parser.GetBraceDepth() == 0;
}

bool mtlSyntaxParser::IsFormat(short ch, short token) const
{
	bool match = false;
	switch (token) {

	case Token_Alpha:
		match = ch <= (unsigned char)(~0) && mtlChars::IsAlpha(ch);
		break;

	case Token_Int:
		match = ch <= (unsigned char)(~0) && mtlChars::IsNumeric(ch);
		break;

	default:
		match = (ch == token);
		break;
	}
	return match;
}

bool mtlSyntaxParser::IsFormat(short ch, const mtlChars &format) const
{
	mtlSyntaxParser parser;
	parser.SetBuffer(format);
	parser.m_is_case_sensitive = m_is_case_sensitive;
	short token;

	while (!parser.IsEnd()) {
		token = parser.ReadToken();
		if ((char)token == ' ')  { continue; }
		if (IsFormat(ch, token)) { return true; }
	}

	return false;
}

mtlChars mtlSyntaxParser::ReadAny(const mtlChars &format1, const mtlChars &format2)
{
	if (IsEnd()) { return mtlChars(); }

	const int start = m_index.pos;

	if (format1.GetSize() > 0) {
		while (!IsEnd()) {
			const short next_ch = PeekChar().ch;
			if (!IsFormat(next_ch, format1) && !IsFormat(next_ch, format2)) { break; }
			ReadChar();
		}
	} else {
		ReadChar();
	}

	return mtlChars(m_buffer, start, m_index.pos).GetTrimmed();
}

mtlChars mtlSyntaxParser::ReadTo(short token)
{
	int brace_depth = GetBraceDepth();
	int start = m_index.pos;

	while (!IsEnd()) {
		if (GetBraceDepth() == brace_depth && IsFormat(PeekChar().ch, token)) { break; }
		ReadChar();
	}

	return mtlChars(m_buffer, start, m_index.pos).GetTrimmed();
}

mtlChars mtlSyntaxParser::OptMatch(const mtlChars &expr)
{
	bool diag = m_log_diag;
	m_log_diag = false;
	mtlChars seq;
	Match(expr, &seq);
	m_log_diag = diag;
	return seq;
}

void mtlSyntaxParser::ClearLog( void )
{
	if (m_log_diag) {
		m_diag_str.Free();
		m_diag_str.Reserve(4096);
	}
}

void mtlSyntaxParser::LogStr(const mtlChars &str)
{
	if (m_log_diag) {
		m_diag_str.Append(str);
	}
}

void mtlSyntaxParser::LogCompactStr(const mtlChars &str)
{
	if (m_log_diag) {
		m_diag_str.Append("\"");
		int spaces = 0;
		for (int i = 0; i < str.GetSize(); ++i) {
			if (mtlChars::IsWhitespace(str[i])) {
				if (spaces == 0) {
					m_diag_str.Append(" ");
					++spaces;
				} else {
					continue;
				}
			} else {
				spaces = 0;
				m_diag_str.Append(str[i]);
			}
		}
		m_diag_str.Append("\"");
	}
}

void mtlSyntaxParser::LogToken(short token)
{
	if (m_log_diag) {
		m_diag_str.AppendInt(token);

		switch (token) {

		case Token_Alpha:
			m_diag_str.Append("(ALPHA)");
			break;

		case Token_Int:
			m_diag_str.Append("(INT)");
			break;

		case Token_Real:
			m_diag_str.Append("(REAL)");
			break;

		case Token_Word:
			m_diag_str.Append("(WORD)");
			break;

		case Token_Str:
			m_diag_str.Append("(STR)");
			break;

		case Token_NullStr:
			m_diag_str.Append("(NULL_STR)");
			break;

		case Token_Alt:
			m_diag_str.Append("(ALT)");
			break;

		case Token_Opt:
			m_diag_str.Append("(OPT)");
			break;

		case Token_Split:
			m_diag_str.Append("(SPLIT)");
			break;

		case Token_EndOfStream:
			m_diag_str.Append("(END)");
			break;

		default:
			LogChar((char)token);
		}
	}
}

void mtlSyntaxParser::LogChar(char ch)
{
	if (m_log_diag) {
		m_diag_str.Append("(");
		m_diag_str.Append(ch);
		m_diag_str.Append(")");
	}
}

bool mtlSyntaxParser::BufferFile(const mtlPath &p_file, mtlString &p_buffer)
{
	if (!p_file.IsFile()) { return false; }
	std::ifstream fin(p_file.GetPath().GetChars(), std::ios::ate|std::ios::binary);
	if (!fin.is_open()) { return false; }
	p_buffer.SetSize((int)fin.tellg());
	fin.seekg(0, std::ios::beg);
	return !fin.read(p_buffer.GetChars(), p_buffer.GetSize()).bad();
}

mtlSyntaxParser::mtlSyntaxParser( void ) : m_hyphenators("_"), m_line(0), m_is_case_sensitive(false), m_log_diag(false), m_quote_char(0)
{
	m_index.pos = 0;
	m_diag_str.SetPoolGrowth(4096);
}

void mtlSyntaxParser::SetBuffer(const mtlChars &buffer, int line_offset)
{
	m_copy.Free();
	m_buffer = buffer.GetTrimmed();
	m_brace_stack.RemoveAll();
	m_index.pos = 0;
	m_index.typ = CharType_Other;
	m_line = line_offset;
	m_quote_char = 0;
}

void mtlSyntaxParser::CopyBuffer(const mtlChars &buffer, int line_offset)
{
	m_copy.Copy(buffer.GetTrimmed());
	m_buffer = m_copy;
	m_brace_stack.RemoveAll();
	m_index.pos = 0;
	m_index.typ = CharType_Other;
	m_line = line_offset;
	m_quote_char = 0;
}

void mtlSyntaxParser::SetHyphenators(const mtlChars &hyphenators)
{
	m_hyphenators = hyphenators;
}

bool mtlSyntaxParser::IsEnd( void ) const
{
	return
		m_index.pos >= m_buffer.GetSize() &&
		m_index.typ == CharType_Stop
	;
}

void mtlSyntaxParser::EnableCaseSensitivity( void )
{
	m_is_case_sensitive = true;
}

void mtlSyntaxParser::DisableCaseSensitivity( void )
{
	m_is_case_sensitive = false;
}

bool mtlSyntaxParser::IsCaseSensitive( void ) const
{
	return m_is_case_sensitive;
}

void mtlSyntaxParser::EnableDiagnostics( void )
{
	m_log_diag = true;
}

void mtlSyntaxParser::DisableDiagnostics( void )
{
	m_log_diag = false;
	m_diag_str.Free();
}

int mtlSyntaxParser::GetBraceDepth( void ) const
{
	return m_brace_stack.GetSize();
}

int mtlSyntaxParser::GetBraceDepth(char brace_type) const
{
	const mtlItem<char> *iter = m_brace_stack.GetFirst();
	int depth = 0;
	while (iter != NULL) {
		if (iter->GetItem() == brace_type) {
			++depth;
		}
		iter = iter->GetNext();
	}
	return depth;
}

int mtlSyntaxParser::GetBufferSize( void ) const
{
	return m_buffer.GetSize();
}

int mtlSyntaxParser::GetBufferSizeRemaining( void ) const
{
	return m_buffer.GetSize() - m_index.pos;
}

int mtlSyntaxParser::GetLineIndex( void ) const
{
	return m_line;
}

int mtlSyntaxParser::GetCharIndex( void ) const
{
	return m_index.pos;
}

const mtlChars &mtlSyntaxParser::GetBuffer( void ) const
{
	return m_buffer;
}

mtlChars mtlSyntaxParser::GetBufferRemaining( void ) const
{
	return mtlChars(m_buffer, m_index.pos, m_buffer.GetSize());
}

const mtlChars &mtlSyntaxParser::GetDiagnostics( void ) const
{
	return m_diag_str;
}

int mtlSyntaxParser::Match(const mtlChars &expr, mtlArray<mtlChars> &out, mtlChars *seq)
{
	ClearLog();
	mtlList<mtlChars> exprs;
	SplitExpressions(expr, exprs);
	mtlItem<mtlChars> *expr_iter = exprs.GetFirst();

	int i = 0;
	while (expr_iter != NULL) {
		int code = MatchSingle(expr_iter->GetItem(), out, seq);
		switch (code) {
		case 1:                    return i;
		case ExpressionInputError: return (int)ExpressionInputError;
		default:                   break;
		}
		++i;
		expr_iter = expr_iter->GetNext();
	}
	return (int)ExpressionNotFound;
}

int mtlSyntaxParser::Match(const mtlChars &expr, mtlChars *seq)
{
	mtlArray<mtlChars> m;
	return Match(expr, m, seq);
}
