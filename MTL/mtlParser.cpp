#include <fstream>
#include "mtlParser.h"

#define mtlWordFmtStr	"%a%d_"
#define mtlFloatFmtStr	"%d.f"
#define mtlIntFmtStr	"%d"
#define mtlBoolFmtStr	"%a%b"

// w	word	%(%a%d_)	X is delimiter
// i	int		%(%d)		Requires checking for overflow
// f	float	%(%d.f)		Requires checking for overflow
// b	bool	%(%a%b)		Requires checking for "true" or "false"
// c	char	N/A			Need to use some other function for this
// s	string	%!(X)		X is delimiter
// l	line	%!(%n)

int mtlParser::SkipWhitespaces(int i) const
{
	while (!IsEnd(i) && mtlChars::IsWhitespace(m_buffer[i])) {
		++i;
	}
	return i;
}

int mtlParser::BackWhitespaces(int i) const
{
	while (!IsEnd(i) && mtlChars::IsWhitespace(m_buffer[i])) {
		--i;
	}
	return i;
}

int mtlParser::SkipNonWhitespaces(int i) const
{
	while (!IsEnd(i) && !mtlChars::IsWhitespace(m_buffer[i])) {
		++i;
	}
	return i;
}

int mtlParser::BackNonWhitespaces(int i) const
{
	while (!IsEnd(i) && !mtlChars::IsWhitespace(m_buffer[i])) {
		--i;
	}
	return i;
}

int mtlParser::IndexOfAny(int i, const mtlChars &p_chars, bool caseSensitive) const
{
	while (!IsEnd(i) && p_chars.SameAsNone(m_buffer[i], caseSensitive)) {
		++i;
	}
	return i;
}

int mtlParser::IndexOfAnyBack(int i, const mtlChars &p_chars, bool caseSensitive) const
{
	while (!IsEnd(i) && p_chars.SameAsNone(m_buffer[i], caseSensitive)) {
		--i;
	}
	return i;
}

mtlParser::Selection mtlParser::WordIndex(int i) const
{
	int start = SkipWhitespaces(i);
	Selection s = { start, IndexOfAny(start, mtlWhitespacesStr, true) };
	return s;
}

mtlParser::Selection mtlParser::WordIndexBack(int i) const
{
	int end = BackWhitespaces(i);
	Selection s = { IndexOfAnyBack(end, mtlWhitespacesStr, true), end };
	return s;
}

mtlParser::Selection mtlParser::LineIndex(int i) const
{
	int start = SkipWhitespaces(i);
	Selection s = { start, IndexOfAny(start, mtlNewlinesStr, true) };
	return s;
}

mtlParser::Selection mtlParser::LineIndexBack(int i) const
{
	int end = BackWhitespaces(i);
	Selection s = { IndexOfAnyBack(end, mtlNewlinesStr, true), end };
	return s;
}

mtlChars mtlParser::GetSubstring(Selection s) const
{
	return mtlChars(m_buffer, s.start, s.end).GetTrimmed();
}

/*void mtlParser::RemoveDelimiters(const mtlChars &chars, const mtlChars &delimiter, mtlString &out, bool caseSensitive) const
{
	out.SetSize(chars.GetSize());
	int o = 0;
	for (int c = 0; c < chars.GetSize(); ++c, ++o) {
		if (mtlChars::SameAsAny(chars[c], delimiter.GetChars(), delimiter.GetSize(), caseSensitive)) {
			--o;
		} else {
			out[o] = chars[c];
		}
	}
	out.SetSize(o);
}*/

bool mtlParser::IsFormat(char ch, const mtlChars &format, bool caseSensitive, const bool notState) const
{
	mtlParser parser(format);
	bool match = false;
	mtlChars word;
	bool skip_read = false;
	bool not_state = false;

	while (!parser.IsEnd() && !match) {

		if (!skip_read) {
			not_state = false;
			word = parser.ReadCharStr();
		}
		skip_read = false;

		if (word.Compare(var, true)) {

			char type = parser.ReadChar();

			switch (type) {
			case 'a': { // alphabetic
				match = mtlChars::IsAlpha(ch) ^ not_state ^ notState;
				break;
			}
			case 'd': { // decimal
				match = mtlChars::IsNumeric(ch) ^ not_state ^ notState;
				break;
			}
			case 'b': { // binary
				match = mtlChars::IsBin(ch) ^ not_state ^ notState;
				break;
			}
			case 'h': { // hexadecimal
				match = mtlChars::IsHex(ch) ^ not_state ^ notState;
				break;
			}
			case '_': { // whitespace
				match = mtlChars::IsWhitespace(ch) ^ not_state ^ notState;
				break;
			}
			case 'n': { // newline
				match = mtlChars::IsNewline(ch) ^ not_state ^ notState;
				break;
			}
			case '[': { // RANGE
				char a = parser.ReadChar();
				char hyphen = parser.ReadChar();
				char b = parser.ReadChar();
				char close_bracket = parser.ReadChar();
				if (hyphen == '-' && close_bracket == ']') {
					if (b < a) {
						char t = a;
						a = b;
						b = t;
					}
					if (!caseSensitive) {
						match = ((ch >= mtlChars::ToLower(a) && ch <= mtlChars::ToLower(b)) || (ch >= mtlChars::ToUpper(a) && ch <= mtlChars::ToUpper(b)))  ^ not_state ^ notState;
					} else {
						match = (ch >= a && ch <= b) ^ not_state ^ notState;
					}
				}
				break;
			}
			case '(': { // OR
				mtlChars inner_format = parser.ReadRaw(parser.IndexOf(")", true) - parser.GetCurrentIndex());
				parser.ReadChar();
				match = IsFormat(ch, inner_format, caseSensitive, not_state ^ notState);
				break;
			}
			case '<': { // AND
				mtlChars inner_format = parser.ReadRaw(parser.IndexOf(">", true) - parser.GetCurrentIndex());
				parser.ReadChar();
				match = IsFormat(ch, inner_format, caseSensitive, not_state ^ notState);
				if (!match) { return false; } // kill loop, no match
				break;
			}
			case '!': { // inverse test
				not_state = true;
				skip_read = true;
				break;
			}
			case var: { // % is the escape character for %
				match = word.Compare(ch, caseSensitive);
				break;
			}
			default:
				return false;
			}

		} else {
			match = word.Compare(ch, caseSensitive);
		}
	}

	return match;
}

mtlParser::mtlParser( void ) : m_buffer(), m_reader(0)
{}

mtlParser::mtlParser(const mtlChars &p_buffer) : m_buffer(), m_reader(0)
{
	SetBuffer(p_buffer);
}

const mtlChars &mtlParser::GetBuffer( void ) const
{
	return m_buffer;
}

void mtlParser::SetBuffer(const mtlChars &p_buffer)
{
	m_buffer = p_buffer.GetTrimmed();
	m_reader = 0;
}

bool mtlParser::BufferFile(const mtlDirectory &p_file, mtlString &p_buffer)
{
	std::ifstream fin(p_file.GetDirectory().GetChars(), std::ios::ate|std::ios::binary);
	if (!fin.is_open()) { return false; }
	p_buffer.SetSize((int)fin.tellg());
	fin.seekg(0, std::ios::beg);
	return !fin.read(p_buffer.GetChars(), p_buffer.GetSize()).bad();
}

char mtlParser::ReadChar( void )
{
	SkipWhitespaces();
	return (!IsEnd(m_reader)) ? m_buffer[m_reader++] : 0;
}

char mtlParser::PeekChar( void ) const
{
	int i = SkipWhitespaces(m_reader);
	return (!IsEnd(i)) ? m_buffer[i] : 0;
}

void mtlParser::BackChar( void )
{
	/*if (skipWhite) {
		while (!IsEnd(m_reader-1) && IsWhite(m_buffer.GetChars()[m_reader-1])) {
			--m_reader;
		}
	} else {
		--m_reader;
	}*/
	m_reader = BackWhitespaces(m_reader-1);
}

mtlChars mtlParser::ReadCharStr( void )
{
	SkipWhitespaces();
	if (!IsEnd(m_reader)) {
		++m_reader;
		return mtlChars(m_buffer, m_reader-1, m_reader);
	}
	return mtlChars(m_buffer, m_reader, m_reader);
}

mtlChars mtlParser::PeekCharStr( void ) const
{
	int i = SkipWhitespaces(m_reader);
	return (!IsEnd(i)) ? mtlChars(m_buffer, i, i+1) : mtlChars(m_buffer, i, i);
}

mtlChars mtlParser::ReadWord( void )
{
	Selection s = WordIndex(m_reader);
	m_reader = s.end;
	return GetSubstring(s);
}

mtlChars mtlParser::PeekWord( void ) const
{
	Selection s = WordIndex(m_reader);
	return GetSubstring(s);
}

void mtlParser::BackWord( void )
{
	/*int r = m_reader;
	while (!IsEnd(m_reader) && IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	while (!IsEnd(m_reader) && !IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	if (IsEnd(m_reader) && r != m_reader+1) { ++m_reader; }*/ // in order to make the first word in file readable only once

	int r = m_reader;
	BackWhitespaces();
	m_reader = BackNonWhitespaces(m_reader);
	if (IsEnd(m_reader) && r != m_reader+1) { ++m_reader; } // in order to make the first word in file readable only once
}

mtlChars mtlParser::ReadFormat(const mtlChars &format, bool caseSensitive, bool notState)
{
	SkipWhitespaces();
	int start = m_reader;

	while (!IsEnd(m_reader) && IsFormat(m_buffer[m_reader], format, caseSensitive, notState)) {
		++m_reader;
	}
	Selection s = { start, m_reader };
	return GetSubstring(s);
}

mtlChars mtlParser::PeekFormat(const mtlChars &format, bool caseSensitive, bool notState) const
{
	int start = SkipWhitespaces(m_reader);
	int end = start;
	while (!IsEnd(end) && IsFormat(m_buffer[end], format, caseSensitive, notState)) {
		++end;
	}
	Selection s = { start, end };
	return GetSubstring(s);
}

mtlChars mtlParser::ReadFormatOne(const mtlChars &format, bool caseSensitive, bool notState)
{
	mtlChars str = ReadFormat(format, caseSensitive, notState);
	return str.GetSize() == 0 ? ReadCharStr() : str;
}

mtlChars mtlParser::PeekFormatOne(const mtlChars &format, bool caseSensitive, bool notState) const
{
	mtlChars str = PeekFormat(format, caseSensitive, notState);
	return str.GetSize() == 0 ? PeekCharStr() : str;
}

mtlChars mtlParser::ReadLine( void )
{
	Selection s = LineIndex(m_reader);
	m_reader = s.end;
	return GetSubstring(s);
}

mtlChars mtlParser::PeekLine( void ) const
{
	return GetSubstring(LineIndex(m_reader));
}

void mtlParser::BackLine( void )
{
	int r = m_reader;
	while (!IsEnd(m_reader) && mtlChars::IsWhitespace(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	while (!IsEnd(m_reader) && !mtlChars::IsNewline(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	if (IsEnd(m_reader) && r != m_reader+1) { ++m_reader; } // in order to make the first line in file readable only once
}

mtlChars mtlParser::ReadTo(const mtlChars &p_str, bool caseSensitive)
{
	int start = m_reader;
	SkipToIndex(IndexOf(p_str, caseSensitive));
	return mtlChars(m_buffer, start, m_reader).GetTrimmed();
}

mtlChars mtlParser::ReadTo(char ch, bool caseSensitive)
{
	const char chars[] = mtlCharToStr(ch);
	return ReadTo(chars, caseSensitive);
}

mtlChars mtlParser::PeekTo(const mtlChars &p_str, bool caseSensitive) const
{
	return mtlChars(m_buffer, m_reader, IndexOf(p_str, caseSensitive)).GetTrimmed();
}

mtlChars mtlParser::PeekTo(char ch, bool caseSensitive) const
{
	const char chars[] = mtlCharToStr(ch);
	return PeekTo(chars, caseSensitive);
}

int mtlParser::IndexOf(const mtlChars &p_str, bool caseSensitive) const
{
	if (p_str.GetSize() == 0) { return m_buffer.GetSize(); }
	int start = m_reader;
	while (!IsEnd(start) && m_buffer.GetSize() - start >= p_str.GetSize()) {
		if (mtlChars::SameAsAll(m_buffer.GetChars()+start, p_str.GetChars(), p_str.GetSize(), caseSensitive)) { return start; }
		++start;
	}
	return m_buffer.GetSize();
}

int mtlParser::IndexOf(char ch, bool caseSensitive) const
{
	const char chars[] = mtlCharToStr(ch);
	return IndexOf(chars, caseSensitive);
}

int mtlParser::IndexOfBack(const mtlChars &p_str, bool caseSensitive) const
{
	int start = m_reader;

	if (caseSensitive) {
		while (!IsEnd(start)) {
			int i;
			for (i = p_str.GetSize() - 1; i >= 0; --i) {
				char c1 = mtlChars::ToLower(m_buffer.GetChars()[i+start]);
				char c2 = mtlChars::ToLower(p_str.GetChars()[i]);
				if (c1 != c2) {
					break;
				}
			}

			if (i == p_str.GetSize()) {
				break;
			}

			--start;
		}
	} else {
		while (!IsEnd(start)) {
			int i;
			for (i = p_str.GetSize() - 1; i >= 0; --i) {
				char c1 = m_buffer.GetChars()[i-start];
				char c2 = p_str.GetChars()[i];
				if (c1 != c2) {
					break;
				}
			}

			if (i == p_str.GetSize()) {
				break;
			}

			--start;
		}
	}

	return start;
}

int mtlParser::IndexOfBack(char ch, bool caseSensitive)
{
	const char chars[] = mtlCharToStr(ch);
	return IndexOfBack(chars, caseSensitive);
}

mtlChars mtlParser::ReadToAny(const mtlChars &p_chars, bool caseSensitive)
{
	int start = m_reader;
	SkipToAny(p_chars, caseSensitive);
	return mtlChars(m_buffer, start, m_reader).GetTrimmed();
}

mtlChars mtlParser::PeekToAny(const mtlChars &p_chars, bool caseSensitive) const
{
	return mtlChars(m_buffer, m_reader, IndexOfAny(p_chars, caseSensitive)).GetTrimmed();
}

void mtlParser::BackToAny(const mtlChars &p_chars, bool caseSensitive)
{
	m_reader = IndexOfAnyBack(p_chars, caseSensitive);
}

int mtlParser::IndexOfAny(const mtlChars &p_chars, bool caseSensitive) const
{
	return IndexOfAny(m_reader, p_chars, caseSensitive);
}

int mtlParser::IndexOfAnyBack(const mtlChars &p_chars, bool caseSensitive) const
{
	return IndexOfAnyBack(m_reader, p_chars, caseSensitive);
}

mtlChars mtlParser::ReadRest( void )
{
	int start = m_reader;
	m_reader = m_buffer.GetSize();
	return mtlChars(m_buffer, start, m_reader).GetTrimmed();
}

mtlChars mtlParser::PeekRest( void ) const
{
	return mtlChars(m_buffer, m_reader, m_buffer.GetSize()).GetTrimmed();
}

bool mtlParser::IsEnd( void )
{
	SkipWhitespaces();
	return IsEnd(m_reader);
}

int mtlParser::GetLineNumber( void ) const
{
	int line_number = 1;
	for (int i = m_reader - 1; i > -1; --i) {
		if (mtlChars::IsNewline(m_buffer[i])) {
			++line_number;
		}
	}
	return line_number;
}

void mtlParser::SkipToEndWord( void )
{
	SkipToEndChar();
	BackWord();
}

void mtlParser::SkipToEndLine( void )
{
	SkipToEndChar();
	BackLine();
}

void mtlParser::SkipToAny(const mtlChars &p_chars, bool caseSensitive)
{
	m_reader = IndexOfAny(p_chars, caseSensitive);
}

void mtlParser::SkipToAnyBack(const mtlChars &p_chars, bool caseSensitive)
{
	m_reader = IndexOfAnyBack(p_chars, caseSensitive);
}

mtlChars mtlParser::ReadRaw(int count)
{
	int start = m_reader;
	m_reader += count;
	return mtlChars(m_buffer, start, m_reader);
}

mtlChars mtlParser::PeekRaw(int count) const
{
	return mtlChars(m_buffer, m_reader, m_reader+count);
}

void mtlParser::BackRaw(int count)
{
	m_reader -= count;
}

mtlParser::ExpressionResult mtlParser::Match(const mtlChars &expr, mtlList<mtlChars> &out, bool revert_on_fail)
{
	out.RemoveAll();

	if (expr.GetSize() == 0) { return ExpressionFound; }

	ExpressionResult result = ExpressionFound;
	mtlParser exprParser(expr);

	bool caseSensitive = false;
	int start = m_reader;
	bool not_state = false;

	while (result == ExpressionFound && !exprParser.IsEnd()) {

		if (IsEnd()) {

			result = ExpressionNotFound;

		} else {

			mtlChars e = exprParser.ReadFormatOne(mtlWordFmtStr);

			if (e.Compare(var)) {

				char var_type = exprParser.ReadChar();
				mtlChars delimiter = exprParser.PeekFormatOne(mtlWordFmtStr);

				switch (var_type) {
				case 'x':
				case 'X':
					caseSensitive = (var_type == 'X');
					break;

				case 'c':
					out.AddLast(ReadCharStr());
					break;

				case 'i': {
					mtlChars t = ReadFormat("%d"); // decimal
					int i;
					if (t.ToInt(i)) {
						out.AddLast(t);
					} else {
						result = ExpressionNotFound;
					}
					break;
				}

				case 'f': {
					mtlChars t = ReadFormat("%d.f"); // decimal and . and f
					float f;
					if (t.ToFloat(f)) {
						out.AddLast(t);
					} else {
						result = ExpressionNotFound;
					}
					break;
				}

				case 'b': {
					mtlChars t = ReadFormat("%a%b"); // alpha and binary
					bool b;
					if (t.ToBool(b)) {
						out.AddLast(t);
					} else {
						result = ExpressionNotFound;
					}
					break;
				}

				case 'w': {
					mtlChars t = ReadFormatOne(mtlWordFmtStr);
					if (t.GetSize() > 0) {
						out.AddLast(t);
					} else {
						result = ExpressionNotFound;
					}
					break;
				}

				case 's': {
					mtlChars c = ReadTo(delimiter, caseSensitive);
					out.AddLast(c);
					break;
				}

				case 'l': {
					out.AddLast(ReadLine());
					break;
				}

				case '!':
					var_type = exprParser.ReadChar();
					if (var_type != '(' && var_type != '<') {
						result = mtlParser::ExpressionInputError;
						break;
					}
					not_state = true;
					// fall through here
				case '<':
				case '(': {
					mtlChars open_brace_types   = "(<";
					mtlChars closed_brace_types = ")>";
					char closed_brace = closed_brace_types[open_brace_types.SameAsWhich(var_type, true)];
					int close_index = exprParser.IndexOf(closed_brace, true);
					int current_index = exprParser.GetCurrentIndex();
					mtlChars format = exprParser.ReadRaw(close_index - current_index);
					exprParser.ReadChar(); // consume closing brace
					out.AddLast(ReadFormat(format, caseSensitive, not_state));
					not_state = false;
					break;
				}

				case var: {
					mtlChars p = ReadFormatOne(mtlWordFmtStr);
					if (!e.Compare(p, caseSensitive)) {
						result = ExpressionNotFound;
					}
					break;
				}

				default:
					result = ExpressionInputError;
					break;

				}

			} else {

				mtlChars p = ReadFormatOne(mtlWordFmtStr);
				if (!e.Compare(p, caseSensitive)) {
					result = ExpressionNotFound;
				}

			}
		}
	}

	if (result != ExpressionFound && revert_on_fail) {
		m_reader = start;
	}
	return result;
}

mtlParser::ExpressionResult mtlParser::MatchAll(const mtlChars &expr, mtlList<mtlChars> &out, bool revert_on_fail)
{
	ExpressionResult result = Match(expr, out, revert_on_fail);
	return (result == ExpressionInputError || IsEnd()) ? result : ExpressionNotFound;
}
