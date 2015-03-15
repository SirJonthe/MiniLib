#include <fstream>
#include "mtlParser.h"

#define mtlVariablesStr "cCifbswln_"

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
	mtlList<char> braceStack;
	mtlReadState readState = Constant;

	for (int i = 0; i < expr.GetSize(); ++i) {

		// brace matching
		char ch = expr[i];

		if (readState == Constant) {

			if (ch == var) {
				readState = Variable;
			} else if (ch == esc) {
				readState = Escape;
			} else {

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
			}
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
	return (!IsEnd()) ? m_buffer[m_reader++] : 0;
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
	return (!IsEnd()) ? mtlChars(m_buffer, m_reader, ++m_reader) : mtlChars(m_buffer, m_reader, m_reader);
}

mtlChars mtlParser::PeekCharStr( void ) const
{
	int i = SkipWhitespaces(m_reader);
	return (!IsEnd()) ? mtlChars(m_buffer, i, i+1) : mtlChars(m_buffer, i, i);
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
	while (!IsEnd() && IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	while (!IsEnd() && !IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	if (IsEnd() && r != m_reader+1) { ++m_reader; }*/ // in order to make the first word in file readable only once

	int r = m_reader;
	BackWhitespaces();
	m_reader = BackNonWhitespaces(m_reader);
	if (IsEnd() && r != m_reader+1) { ++m_reader; } // in order to make the first word in file readable only once
}

mtlChars mtlParser::ReadFormat(const mtlChars &format, bool caseSensitive)
{
	SkipWhitespaces();
	int start = m_reader;
	while (!IsEnd() && mtlChars::SameAsAny(m_buffer[m_reader], format.GetChars(), format.GetSize(), caseSensitive)) {
		++m_reader;
	}
	if (m_reader - start == 0) { ++m_reader; }
	Selection s = { start, m_reader };
	return GetSubstring(s);
}

mtlChars mtlParser::PeekFormat(const mtlChars &format, bool caseSensitive)
{
	int start = SkipWhitespaces(m_reader);
	int end = start;
	while (!IsEnd(end) && mtlChars::SameAsAny(m_buffer[end], format.GetChars(), format.GetSize(), caseSensitive)) {
		++end;
	}
	if (end - start == 0) { ++end; }
	Selection s = { start, end };
	return GetSubstring(s);
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
	while (!IsEnd() && mtlChars::IsWhitespace(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	while (!IsEnd() && !mtlChars::IsNewline(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	if (IsEnd() && r != m_reader+1) { ++m_reader; } // in order to make the first line in file readable only once
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

mtlParser::ExpressionResult mtlParser::Match(const mtlChars &expr, mtlList<mtlChars> &out)
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

			if (e.Compare("%")) {

				char var_type = exprParser.ReadChar();
				mtlChars delimiter = exprParser.PeekCharStr();
				mtlString format;

				switch (var_type) {
				case 'c':
				case 'C':
					caseSensitive = (var_type == 'C');
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

				case 's': {
					mtlChars t = ReadTo(delimiter, caseSensitive);
					out.AddLast(t);
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
		m_reader = start;
	}
	return result;
}

/*// function = { return %s, %f; } ;
	// spaces mean there *can* be spaces (space/tab/newline) that separates the types, but there does not have to be
// function %_= %_{ return %_ %s %_};%n
	// %_ means there *must* be more than one space (space/tab/newline) that separates the types
	// %_ %_ is invalid since the first %_ already consumes all spaces
	// %n consumes all spaces, until a newline is found
		// expression fails if parser parses a non-white character when processing %n
// All % expressions can be escaped using \%, meaning we treat that as a character like any other rather than an expression

// %c = set to non-case sensitive
// %C = set to case sensitive

// " and ' are exceptions where if a " or ' lies on the stack no
// other brace gets added until a matching (non-escaped) " or ' is found*/


/*mtlParser::ExpressionResult mtlParser::Match(const mtlChars &expr, mtlList<mtlChars> &out)
{
	// implement case sensitivity (I'm not using the case state, only setting it to a value)

	// parse expr using an mtlParser (should simplify code a lot)

	// parsing expression with parser:
	// read word
	// if word starts with % then variable
		// next letter gives what type of variable
		// if next letter is unknown variable or end of character stream then expression error
		// if there is another letter in the word (that is not %) then that is the delimiter
	// matching w, b, i, f:
		// read whole string to delimiter
		// trim string
		// search for whitespace in string, no match if a whitespace is found
		// check for success on conversion to desired type, mismatch if conversion failure
		// if case sensitivity is off, convert to lower case
		// store in out
	// matching s:
		// read whole string to delimiter (spaces can not be delimiters)
		// trim string if this is not a quote section
		// if case sensitivity is off, convert to lower case
		// store in out
	// constant matching:
		// word for word matching (with or without case sensitivity)
		// number of spaces are irrelevant unless this is a quote section

	out.RemoveAll();

	if (expr.GetSize() == 0) { return ExpressionFound; }

	const int initalReader = m_reader;
	bool caseSensitive = true;
	int exprReader = 0;
	mtlReadState readState = Constant;
	char quoteChar = 0;

	ExpressionResult result = VerifyInputExpression(expr);

	while (!IsEnd() && exprReader < expr.GetSize() && result == ExpressionValid) {

		char ech = expr[exprReader++];

		// Set read state
		if (readState == Constant) {

			if (ech == var) {
				readState = Variable;

			} else if (ech == esc) {
				readState = Escape;

			} else {

				if (quoteChar == 0) {

					SkipWhitespaces();

					while (exprReader < expr.GetSize() && mtlChars::IsWhitespace(ech)) {
						ech = expr[exprReader++];
					}
				}

				if (ech != var) {
					char bufchar = m_buffer[m_reader++];

					if (caseSensitive && bufchar != ech) {
						result = ExpressionNotFound;
					} else if (!caseSensitive && mtlChars::ToLower(bufchar) != mtlChars::ToLower(ech)) {
						result = ExpressionNotFound;
					} else {

						if (quoteChar == 0) {
							if (ech == '\'' || ech == '\"') { // opening quote
								quoteChar = ech;
							}
						} else if (quoteChar == ech) { // closing quote
							quoteChar = 0;
						}

					}
				} else {
					readState = Variable;
				}
			}
		}

		// Process a variable
		else if (readState == Variable) {

			if (exprReader > expr.GetSize()) { result = ExpressionInputError; }

			switch (ech) {
			case 'c': // case sensitivity disabled
			case 'C': // case sensitivity enabled
				caseSensitive = (ech == 'C');
				break;

			case 'i': // Int (broken by delimiter or space)
			case 'f': // Float (broken by delimiter or space)
			case 'b': // Bool (broken by delimiter or space)
			case 'w': // Word (broken by delimiter or space)
			{
				bool delimiterFound = false;
				char delimiter = 0;
				bool escape = false;
				for (int i = exprReader; i < expr.GetSize() && !delimiterFound; ++i) {
					// look for a non-white character
					char ch = expr[i];
					if (ch == var && !escape) {
						delimiter = ' '; // there is no delimiter, only whitespaces
						delimiterFound = true;
					} else if (ch == esc) {
						escape = true;
						continue;
					} else if (!mtlChars::IsWhitespace(ch)) {
						delimiter = ch;
						delimiterFound = true;
					}
					escape = false;
				}

				// read until delimiter, or if a delimiter does not exist space
				SkipWhitespaces(); // remove all white spaces prior to word
				int start = m_reader;
				while (!IsEnd()) {
					char c = m_buffer[m_reader];
					if ((delimiterFound && c == delimiter) || mtlChars::IsWhitespace(c)) { break; }
					++m_reader;
				}
				mtlChars variable(m_buffer, start, m_reader);

				int ti = 0;
				float fi = 0.0f;
				bool bi = false;

				if (ech == 'w') {
					bool valid = true;
					for (int i = 0; i < variable.GetSize() && valid; ++i) {
						char ch = variable.GetChars()[i];
						if (!mtlChars::IsAlpha(ch) && ch != '_') {
							valid = false;
							result = ExpressionTypeMismatch;
						}
					}
					if (valid) {
						out.AddLast(variable);
					}
				} else if (ech == 'i') {
					if (variable.ToInt(ti)) {
						out.AddLast(variable);
					} else {
						result = ExpressionTypeMismatch;
					}
				} else if (ech == 'f') {
					if (variable.ToFloat(fi)) {
						out.AddLast(variable);
					} else {
						result = ExpressionTypeMismatch;
					}
				} else if (ech == 'b') {
					if (variable.ToBool(bi)) {
						out.AddLast(variable);
					} else {
						result = ExpressionTypeMismatch;
					}
				}

				break;
			}

			case 's': // String (Broken by delimiter)
			{
				bool delimiterFound = false;
				char delimiter = 0;
				bool escape = false;
				for (int i = exprReader; i < expr.GetSize() && !delimiterFound; ++i) {
					// look for a non-white character
					char ch = expr[i];
					if (ch == var && !escape) {
						continue;
					} else if (ch == esc) {
						escape = true;
						continue;
					} else if (!mtlChars::IsWhitespace(ch)) {
						delimiter = ch;
						delimiterFound = true;
					}
					escape = false;
				}

				mtlChars variable;
				if (!delimiterFound) {
					variable = mtlChars(m_buffer, m_reader, m_buffer.GetSize());
					m_reader = m_buffer.GetSize();
				} else {
					int start = m_reader;
					while (!IsEnd() && m_buffer[m_reader] != delimiter) {
						++m_reader;
					}
					variable = mtlChars(m_buffer, start, m_reader);
				}

				if (quoteChar != '\"' && quoteChar != '\'') {
					variable.Trim();
				}

				if (variable.GetSize() > 0) {
					out.AddLast(variable);
				} else {
					result = ExpressionNotFound;
				}

				break;
			}

			case 'l': // String (Broken by newline)
			{
				mtlChars variable = ReadLine();
				if (quoteChar != '\"' && quoteChar != '\'') {
					variable.Trim();
				}
				out.AddLast(variable);
				break;
			}

			case 'n': // Newline (makes sure there is a newline in the matched expression)
			{
				bool foundNewl = false;
				while (!IsEnd() && mtlChars::IsWhitespace(m_buffer[m_reader]) && !foundNewl) {
					if (mtlChars::IsNewline(m_buffer[m_reader])) {
						foundNewl = true;
					}
					++m_reader;
				}
				if (!foundNewl) { result = ExpressionNotFound; }
				break;
			}

			case '_':
			{
				if (!mtlChars::IsWhitespace(m_buffer[m_reader])) {
					result = ExpressionNotFound;
				} else {
					while (!IsEnd() && mtlChars::IsWhitespace(m_buffer[m_reader])) {
						++m_reader;
					}
				}
				break;
			}

			default:
				result = ExpressionInputError;
				break;
			}
			readState = Constant;
		}

		else if (readState == Escape) {
			++exprReader;
			readState = Constant;
		}

	}

	if (result != ExpressionValid) {
		out.RemoveAll();
		m_reader = initalReader;
	} else {
		result = ExpressionFound;
	}

	return result;
}*/
