#include <fstream>
#include "mtlParser.h"

#define mtlVariablesStr "xXcifbswln([adhm_"

enum mtlReadState
{
	Constant,
	Variable,
	Escape
};

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

mtlParser::ExpressionResult mtlParser::VerifyInputExpression(const mtlChars &expr) const
{
	mtlReadState readState = Constant;

	for (int i = 0; i < expr.GetSize(); ++i) {

		// brace matching
		char ch = expr[i];

		if (readState == Constant) {

			if (ch == var) {
				readState = Variable;
			} else if (ch == esc) {
				readState = Escape;
			}/* else {

				if (mtlChars::SameAsAny(ch, mtlOpenBracesStr, sizeof(mtlOpenBracesStr))) {

					if (braceStack.GetSize() == 0 || (braceStack.GetLast()->GetItem() != '\"' && braceStack.GetLast()->GetItem() != '\'')) {
						braceStack.AddLast(ch);
					} else if (ch == braceStack.GetLast()->GetItem()) {
						braceStack.RemoveLast();
					}

				} else {

					int chi = mtlChars::SameAsWhich(ch, mtlClosedBracesStr, sizeof(mtlClosedBracesStr));
					if (chi > -1) {

						if (i == 0) { return ExpressionUnbalancedBraces; }

						char match = mtlOpenBracesStr[chi];
						if (match == braceStack.GetLast()->GetItem()) {
							braceStack.RemoveLast();
						} else {
							return ExpressionUnbalancedBraces;
						}
					}
				}
			}*/
		} else if (readState == Variable) {
			if (!mtlChars::SameAsAny(ch, mtlVariablesStr, sizeof(mtlVariablesStr))) {
				return ExpressionInputError;
			}
			readState = Constant;
		} else if (readState == Escape) {
			readState = Constant;
		}
	}

	return ExpressionValid;
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

void mtlParser::RemoveDelimiters(const mtlChars &chars, const mtlChars &delimiter, mtlString &out, bool caseSensitive) const
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
	return (!IsEnd(m_reader)) ? mtlChars(m_buffer, m_reader, ++m_reader) : mtlChars(m_buffer, m_reader, m_reader);
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

mtlChars mtlParser::ReadFormat(const mtlChars &format, bool caseSensitive)
{
	SkipWhitespaces();
	int start = m_reader;

	bool match = true;
	while (!IsEnd(m_reader) && match) {

		char ch = m_buffer[m_reader];

		mtlParser parser(format);
		match = false;

		while (!parser.IsEnd() && !match) {
			mtlChars word = parser.ReadWord();
			if (word.Compare("%", true)) {

				char type = parser.ReadChar();

				switch (type) {
				case 'a': {
					match = mtlChars::IsAlpha(ch);
					break;
				}
				case 'd': {
					match = mtlChars::IsNumeric(ch);
					break;
				}
				case 'b': {
					match = mtlChars::IsBin(ch);
					break;
				}
				case 'h': {
					match = mtlChars::IsHex(ch);
					break;
				}
				case '_': {
					match = mtlChars::IsWhitespace(ch);
					break;
				}
				case 'n': {
					match = mtlChars::IsNewline(ch);
					break;
				}
				case '(': {
					mtlList<mtlChars> out;
					if (parser.Match("%c-%c)", out, false) == mtlParser::ExpressionFound && out.GetSize() == 2) {
						char a = out.GetFirst()->GetItem()[0];
						char b = out.GetFirst()->GetItem()[0];
						if (b < a) {
							char t = a;
							a = b;
							b = t;
						}
						if (!caseSensitive) {
							match = (ch >= mtlChars::ToLower(a) && ch <= mtlChars::ToLower(b)) || (ch >= mtlChars::ToUpper(a) && ch <= mtlChars::ToUpper(b));
						} else {
							match = (ch >= a && ch <= b);
						}
					}
					break;
				}
				}

			} else {
				match = mtlChars::SameAsAny(m_buffer[m_reader], word.GetChars(), word.GetSize(), caseSensitive);
			}
		}

		++m_reader;
	}

	Selection s = { start, m_reader };
	return GetSubstring(s);

	/*while (!IsEnd(m_reader) && mtlChars::SameAsAny(m_buffer[m_reader], format.GetChars(), format.GetSize(), caseSensitive)) {
		++m_reader;
	}
	int end = m_reader;
	if (end - start == 0) { ++end; }
	Selection s = { start, end };
	return GetSubstring(s);*/
}

/*mtlChars mtlParser::PeekFormat(const mtlChars &format, bool caseSensitive) const
{
	int start = SkipWhitespaces(m_reader);
	int end = start;
	while (!IsEnd(end) && mtlChars::SameAsAny(m_buffer[end], format.GetChars(), format.GetSize(), caseSensitive)) {
		++end;
	}
	if (end - start == 0) { ++end; }
	Selection s = { start, end };
	return GetSubstring(s);
}*/

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

mtlChars mtlParser::PeekTo(const mtlChars &p_str, bool caseSensitive)
{
	return mtlChars(m_buffer, m_reader, IndexOf(p_str, caseSensitive)).GetTrimmed();
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

	ExpressionResult result = VerifyInputExpression(expr);
	mtlParser exprParser(expr);

	bool caseSensitive = false;
	int start = m_reader;

	while (result == ExpressionValid && !exprParser.IsEnd()) {

		if (IsEnd()) {

			result = ExpressionNotFound;

		} else {

			mtlChars e = exprParser.ReadFormat(mtlAlphanumStr);

			//case 'n': // makes sure there's a newline
			//case '_': // makes sure there's a white space

			if (e.Compare(var)) {

				char var_type = exprParser.ReadChar();
				mtlChars delimiter = exprParser.PeekCharStr();
				mtlString format;

				switch (var_type) {
				case 'x':
				case 'X':
					caseSensitive = (var_type == 'X');
					break;

				case 'c':
					out.AddLast(ReadCharStr());
					break;

				case 'i': {
					RemoveDelimiters(mtlIntStr, delimiter, format, caseSensitive);
					mtlChars t = ReadFormat(format);
					int i;
					if (t.ToInt(i)) {
						out.AddLast(t);
					} else {
						result = ExpressionNotFound;
					}
					break;
				}

				case 'f': {
					RemoveDelimiters(mtlFloatStr, delimiter, format, caseSensitive);
					mtlChars t = ReadFormat(format);
					float f;
					if (t.ToFloat(f)) {
						out.AddLast(t);
					} else {
						result = ExpressionNotFound;
					}
					break;
				}

				case 'b': {
					RemoveDelimiters(mtlAlphanumStr, delimiter, format, caseSensitive);
					mtlChars t = ReadFormat(format);
					bool b;
					if (t.ToBool(b)) {
						out.AddLast(t);
					} else {
						result = ExpressionNotFound;
					}
					break;
				}

				case 'w': {
					RemoveDelimiters(mtlAlphanumStr, delimiter, format, caseSensitive);
					mtlChars t = ReadFormat(format);
					out.AddLast(t);
					break;
				}

				case '[': {
					int index = exprParser.IndexOf("]", true);
					mtlChars format = exprParser.ReadRaw(index - exprParser.GetCurrentIndex());
					exprParser.ReadChar(); // consume ']'
					out.AddLast(ReadFormat(format, caseSensitive));
					break;
				}

				case '(': {
					int end = exprParser.IndexOf(")", true);
					mtlString format("%(");
					format.Append(exprParser.ReadRaw(end - exprParser.GetCurrentIndex() - 1).GetTrimmed()).Append(")");
					out.AddLast(ReadFormat(format));
					exprParser.SkipToIndex(end + 1);
					break;
				}

				case 's': {
					out.AddLast(ReadTo(delimiter, caseSensitive));
					break;
				}

				case 'l': {
					out.AddLast(ReadLine());
					break;
				}
				}

			} else {

				mtlChars p = ReadFormat(mtlAlphanumStr);
				if (!e.Compare(p, caseSensitive)) {
					result = ExpressionNotFound;
				}

			}
		}
	}

	if (result == ExpressionValid) {
		result = ExpressionFound;
	} else {
		out.RemoveAll();
		if (revert_on_fail) { m_reader = start; }
	}
	return result;
}

mtlParser::ExpressionResult mtlParser::MatchAll(const mtlChars &expr, mtlList<mtlChars> &out, bool revert_on_fail)
{
	ExpressionResult result = Match(expr, out, revert_on_fail);
	return (IsEnd()) ? result : ExpressionNotFound;
}
