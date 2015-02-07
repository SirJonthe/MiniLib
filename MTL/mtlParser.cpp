#include <fstream>
#include "mtlParser.h"

#define mtlOpenBraces	"([{<\'\""
#define mtlClosedBraces	")]}>\'\""
#define mtlVariables	"cCifbswln_"
#define mtlWhitespaces	" \t\n\r"

enum mtlReadState
{
	Constant,
	Variable,
	Escape
};

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

				if (mtlChars::SameAsAny(ch, mtlOpenBraces, sizeof(mtlOpenBraces))) {

					if (braceStack.GetSize() == 0 || (braceStack.GetLast()->GetItem() != '\"' && braceStack.GetLast()->GetItem() != '\'')) {
						braceStack.AddLast(ch);
					} else if (ch == braceStack.GetLast()->GetItem()) {
						braceStack.RemoveLast();
					}

				} else {

					int chi = mtlChars::SameAsWhich(ch, mtlClosedBraces, sizeof(mtlClosedBraces));
					if (chi > -1) {

						if (i == 0) { return ExpressionUnbalancedBraces; }

						char match = mtlOpenBraces[chi];
						if (match == braceStack.GetLast()->GetItem()) {
							braceStack.RemoveLast();
						} else {
							return ExpressionUnbalancedBraces;
						}
					}
				}
			}
		} else if (readState == Variable) {
			if (!mtlChars::SameAsAny(ch, mtlVariables, sizeof(mtlVariables))) {
				return ExpressionInputError;
			}
			readState = Constant;
		} else if (readState == Escape) {
			readState = Constant;
		}
	}

	return ExpressionValid;
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
	m_buffer = p_buffer;
	m_buffer.Trim();
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

char mtlParser::ReadChar(bool skipWhite)
{
	if (skipWhite) {
		while (!IsEnd(m_reader) && IsWhite(m_buffer.GetChars()[m_reader])) {
			++m_reader;
		}
	}
	return m_buffer.GetChars()[m_reader++];
}

char mtlParser::PeekChar(bool skipWhite) const
{
	int i = m_reader;
	if (skipWhite) {
		while (!IsEnd(i) && IsWhite(m_buffer.GetChars()[i])) {
			++i;
		}
	}
	return m_buffer.GetChars()[i];
}

void mtlParser::SkipChar(bool skipWhite)
{
	if (skipWhite) {
		while (!IsEnd(m_reader) && IsWhite(m_buffer.GetChars()[m_reader])) {
			++m_reader;
		}
	}
	++m_reader;
}


void mtlParser::BackChar(bool skipWhite)
{
	if (skipWhite) {
		while (!IsEnd(m_reader-1) && IsWhite(m_buffer.GetChars()[m_reader-1])) {
			--m_reader;
		}
	}
	--m_reader;
}

mtlChars mtlParser::ReadWord( void )
{
	while (!IsEnd() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	int start = m_reader;
	while (!IsEnd() && !IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	return mtlChars(m_buffer, start, m_reader);
}

mtlChars mtlParser::PeekWord( void ) const
{
	int i = m_reader;
	while (!IsEnd(i) && IsWhite(m_buffer.GetChars()[i])) {
		++i;
	}
	int start = i;
	while (!IsEnd(i) && !IsWhite(m_buffer.GetChars()[i])) {
		++i;
	}
	return mtlChars(m_buffer, start, i);
}

void mtlParser::BackWord( void )
{
	int r = m_reader;
	while (!IsEnd() && IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	while (!IsEnd() && !IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	if (IsEnd() && r != m_reader+1) { ++m_reader; } // in order to make the first word in file readable only once
}

void mtlParser::SkipWord( void )
{
	while (!IsEnd() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	while (!IsEnd() && !IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
}

mtlChars mtlParser::ReadLine( void )
{
	while (!IsEnd() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	int start = m_reader;
	while (!IsEnd() && !IsNewl(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	int end = m_reader;
	while (!IsEnd(end-1) && IsWhite(m_buffer.GetChars()[end-1])) {
		--end;
	}
	return mtlChars(m_buffer, start, end);
}

mtlChars mtlParser::PeekLine( void ) const
{
	int i = m_reader;
	while (!IsEnd(i) && IsWhite(m_buffer.GetChars()[i])) {
		++i;
	}
	int start = i;
	while (!IsEnd(i) && !IsNewl(m_buffer.GetChars()[i])) {
		++i;
	}
	while (!IsEnd(i-1) && IsWhite(m_buffer.GetChars()[i-1])) {
		--i;
	}
	return mtlChars(m_buffer, start, i);
}

void mtlParser::BackLine( void )
{
	int r = m_reader;
	while (!IsEnd() && IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	while (!IsEnd() && !IsNewl(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	if (IsEnd() && r != m_reader+1) { ++m_reader; } // in order to make the first line in file readable only once
}

void mtlParser::SkipLine( void )
{
	while (!IsEnd() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	while (!IsEnd() && !IsNewl(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
}

mtlChars mtlParser::ReadTo(const mtlChars &p_str)
{
	int start = m_reader;
	while (!IsEnd() && p_str.GetSize() <= GetCharsLeft() && !mtlChars::SameAsAll(p_str.GetChars(), m_buffer.GetChars()+m_reader, p_str.GetSize())) {
		++m_reader;
	}
	if (GetCharsLeft() < p_str.GetSize()) {
		m_reader = m_buffer.GetSize();
	}
	return mtlChars(m_buffer, start, m_reader);
}

mtlChars mtlParser::PeekTo(const mtlChars &p_str)
{
	int i = m_reader;
	while (!IsEnd(i) && p_str.GetSize() <= GetCharsLeft() && !mtlChars::SameAsAll(p_str.GetChars(), m_buffer.GetChars()+i, p_str.GetSize())) {
		++i;
	}
	if (GetCharsLeft() < p_str.GetSize()) {
		i = m_buffer.GetSize();
	}
	return mtlChars(m_buffer, m_reader, i);
}

mtlChars mtlParser::ReadToAny(const mtlChars &p_chars)
{
	int start = m_reader;
	while (!IsEnd() && mtlChars::SameAsAny(m_buffer[m_reader], p_chars.GetChars(), p_chars.GetSize())) {
		++m_reader;
	}
	return mtlChars(m_buffer, start, m_reader);
}

mtlChars mtlParser::PeekToAny(const mtlChars &p_chars)
{
	int i = m_reader;
	while (!IsEnd(i) && mtlChars::SameAsAny(m_buffer[i], p_chars.GetChars(), p_chars.GetSize())) {
		++i;
	}
	return mtlChars(m_buffer, m_reader, i);
}

void mtlParser::BackToAny(const mtlChars &p_chars)
{
	while (!IsEnd() && !mtlChars::SameAsAny(m_buffer[m_reader], p_chars.GetChars(), p_chars.GetSize())) {
		--m_reader;
	}
}

mtlChars mtlParser::ReadRest( void )
{
	while (!IsEnd() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	int start = m_reader;
	while (!IsEnd()) {
		++m_reader;
	}
	return mtlChars(m_buffer, start, m_reader);
}

mtlChars mtlParser::PeekRest( void ) const
{
	int start = m_reader;
	while (!IsEnd(start) && IsWhite(m_buffer.GetChars()[start])) {
		++start;
	}
	int end = start;
	while (!IsEnd(end)) {
		++end;
	}
	return mtlChars(m_buffer, start, end);
}

int mtlParser::GetLineNumber( void ) const
{
	int line_number = 1;
	for (int i = m_reader - 1; i > -1; --i) {
		if (IsNewl(m_buffer[i])) {
			++line_number;
		}
	}
	return line_number;
}

void mtlParser::JumpToEndWord( void )
{
	JumpToEndChar();
	BackWord();
}

void mtlParser::JumpToEndLine( void )
{
	JumpToEndChar();
	BackLine();
}

bool mtlParser::JumpToChar(const mtlChars &p_chars)
{
	const int count = p_chars.GetSize();
	while (!IsEnd()) {
		char c = m_buffer.GetChars()[m_reader++];
		for (int i = 0; i < count; ++i) {
			if (c == p_chars.GetChars()[i]) {
				return true;
			}
		}
	}
	return false;
}

bool mtlParser::JumpToCharBack(const mtlChars &p_chars)
{
	const int count = p_chars.GetSize();
	while (!IsEnd()) {
		char c = m_buffer.GetChars()[m_reader--];
		for (int i = 0; i < count; ++i) {
			if (c == p_chars.GetChars()[i]) {
				return true;
			}
		}
	}
	return false;
}

void mtlParser::ConsumeWhitespaces( void )
{
	while (!IsEnd() && IsWhite(m_buffer[m_reader])) {
		++m_reader;
	}
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


mtlParser::ExpressionResult mtlParser::Match(const mtlChars &expr, mtlList<mtlChars> &out)
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

					ConsumeWhitespaces();

					while (exprReader < expr.GetSize() && IsWhite(ech)) {
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
					} else if (!IsWhite(ch)) {
						delimiter = ch;
						delimiterFound = true;
					}
					escape = false;
				}

				// read until delimiter, or if a delimiter does not exist space
				ConsumeWhitespaces(); // remove all white spaces prior to word
				int start = m_reader;
				while (!IsEnd()) {
					char c = m_buffer[m_reader];
					if ((delimiterFound && c == delimiter) || IsWhite(c)) { break; }
					++m_reader;
				}
				mtlChars variable(m_buffer, start, m_reader);

				int ti = 0;
				float fi = 0.0f;
				bool bi = false;

				if (ech == 'w') {
					out.AddLast(variable);
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
					} else if (!IsWhite(ch)) {
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
				while (!IsEnd() && IsWhite(m_buffer[m_reader]) && !foundNewl) {
					if (IsNewl(m_buffer[m_reader])) {
						foundNewl = true;
					}
					++m_reader;
				}
				if (!foundNewl) { result = ExpressionNotFound; }
				break;
			}

			case '_':
			{
				if (!IsWhite(m_buffer[m_reader])) {
					result = ExpressionNotFound;
				} else {
					while (!IsEnd() && IsWhite(m_buffer[m_reader])) {
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
}
