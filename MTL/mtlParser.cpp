#include <fstream>
#include "mtlParser.h"

void mtlParser::ClearTrailingWhitespaces( void )
{
	while (!IsEndOfFile(m_size-1) && IsWhite(m_buffer.GetChars()[m_size-1])) {
		--m_size;
	}
}

mtlParser::mtlParser( void ) : m_buffer(), m_size(0), m_reader(0)
{}

mtlParser::mtlParser(const mtlChars &p_buffer) : m_buffer(), m_size(0), m_reader(0)
{
	SetBuffer(p_buffer);
}

void mtlParser::SetBuffer(const mtlChars &p_buffer)
{
	m_buffer = p_buffer;
	m_size = p_buffer.GetSize();
	m_reader = 0;
	ClearTrailingWhitespaces();
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
		while (!IsEndOfFile(m_reader) && IsWhite(m_buffer.GetChars()[m_reader])) {
			++m_reader;
		}
	}
	return m_buffer.GetChars()[m_reader++];
}

char mtlParser::PeekChar(bool skipWhite) const
{
	int i = m_reader;
	if (skipWhite) {
		while (!IsEndOfFile(i) && IsWhite(m_buffer.GetChars()[i])) {
			++i;
		}
	}
	return m_buffer.GetChars()[i];
}

void mtlParser::SkipChar(bool skipWhite)
{
	if (skipWhite) {
		while (!IsEndOfFile(m_reader) && IsWhite(m_buffer.GetChars()[m_reader])) {
			++m_reader;
		}
	}
	++m_reader;
}


void mtlParser::BackChar(bool skipWhite)
{
	if (skipWhite) {
		while (!IsEndOfFile(m_reader-1) && IsWhite(m_buffer.GetChars()[m_reader-1])) {
			--m_reader;
		}
	}
	--m_reader;
}

mtlChars mtlParser::ReadWord( void )
{
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	int start = m_reader;
	while (!IsEndOfFile() && !IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	return mtlChars(m_buffer, start, m_reader);
}

mtlChars mtlParser::PeekWord( void ) const
{
	int i = m_reader;
	while (!IsEndOfFile(i) && IsWhite(m_buffer.GetChars()[i])) {
		++i;
	}
	int start = i;
	while (!IsEndOfFile(i) && !IsWhite(m_buffer.GetChars()[i])) {
		++i;
	}
	return mtlChars(m_buffer, start, i);
}

void mtlParser::BackWord( void )
{
	int r = m_reader;
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	while (!IsEndOfFile() && !IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	if (IsEndOfFile() && r != m_reader+1) { ++m_reader; } // in order to make the first word in file readable only once
}

void mtlParser::SkipWord( void )
{
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	while (!IsEndOfFile() && !IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
}

mtlChars mtlParser::ReadLine( void )
{
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	int start = m_reader;
	while (!IsEndOfFile() && !IsNewl(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	int end = m_reader;
	while (!IsEndOfFile(end-1) && IsWhite(m_buffer.GetChars()[end-1])) {
		--end;
	}
	return mtlChars(m_buffer, start, end);
}

mtlChars mtlParser::PeekLine( void ) const
{
	int i = m_reader;
	while (!IsEndOfFile(i) && IsWhite(m_buffer.GetChars()[i])) {
		++i;
	}
	int start = i;
	while (!IsEndOfFile(i) && !IsNewl(m_buffer.GetChars()[i])) {
		++i;
	}
	while (!IsEndOfFile(i-1) && IsWhite(m_buffer.GetChars()[i-1])) {
		--i;
	}
	return mtlChars(m_buffer, start, i);
}

void mtlParser::BackLine( void )
{
	int r = m_reader;
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	while (!IsEndOfFile() && !IsNewl(m_buffer.GetChars()[m_reader])) {
		--m_reader;
	}
	if (IsEndOfFile() && r != m_reader+1) { ++m_reader; } // in order to make the first line in file readable only once
}

void mtlParser::SkipLine( void )
{
	while (!IsEndOfFile() && IsWhite(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
	while (!IsEndOfFile() && !IsNewl(m_buffer.GetChars()[m_reader])) {
		++m_reader;
	}
}

void mtlParser::JumpToEndWord( void )
{
	// results in two undo
	JumpToEndChar();
	BackWord();
}

void mtlParser::JumpToEndLine( void )
{
	// results in two undo
	JumpToEndChar();
	BackLine();
}

bool mtlParser::JumpToChar(const mtlChars &p_chars)
{
	const int count = p_chars.GetSize();
	while (!IsEndOfFile()) {
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
	while (!IsEndOfFile()) {
		char c = m_buffer.GetChars()[m_reader--];
		for (int i = 0; i < count; ++i) {
			if (c == p_chars.GetChars()[i]) {
				return true;
			}
		}
	}
	return false;
}

/*mtlParser::ExpressionResult mtlParser::Expression(const mtlChars &expr, mtlList<mtlString> &out)
{
	// function = { return %s, %f; } ;
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
	// other brace gets added until a matching (non-escaped) " or ' is found
	static const mtlChars openBraces  = "([{<\"\'";
	static const mtlChars closeBraces = ")]}>\"\'";

	int startPosition = m_reader;
	out.RemoveAll();
	ExpressionResult result = ExpressionFound;
	bool caseSensitive = true;

	mtlList<char> braceStack;
	int exprReader = 0;

	while (!IsEndOfFile() && exprReader < expr.GetSize() && result == ExpressionFound) {

		// skip all white characters
		while (exprReader < expr.GetSize() && IsWhite(expr[exprReader])) { ++exprReader; }

		int start = exprReader;

		// read a word
		while (exprReader < expr.GetSize()) {

			char ch = expr[exprReader++];

			// variable expression
			if (ch == '%') {
				if (exprReader == 0 || expr[exprReader-1] != esc) {
					++exprReader;
					break;
				}
			}

			// end of word
			else if (IsWhite(ch)) {
				break;
			}

			// opening brace, always valid to add to stack
			else if (mtlChars::SameAsAny(ch, openBraces.GetChars(), openBraces.GetSize())) {
				if (ch == '\'' || ch == '\"' || (braceStack.GetSize() == 0 && braceStack.GetLast()->GetItem() != '\'' && braceStack.GetLast()->GetItem() != '\"')) {
					braceStack.AddLast(ch);
					break;
				}
			}

			else if (mtlChars::SameAsAny(ch, closeBraces.GetChars(), closeBraces.GetSize())) {
				// add to stack if valid, else emit error
			}
		}
		mtlChars cexpr = expr.Substring(start, exprReader); // only a single word

		if (cexpr.GetSize() > 0) {

			// match a variable expression
			if (cexpr[0] == '%') {
				if (cexpr.GetSize() != 2) {
					result = ExpressionInputError;
				} else {
					switch (cexpr[1]) {
					case 'c':
					case 'C':
						caseSensitive = (cexpr[1] == 'C');
						break;
					}
				}
			}

			// match a constant expression
			else {
				// the variable sign has been escaped, so we need to adjust the substring
				if (cexpr[0] == '\\' && cexpr[1] == '%') { cexpr = cexpr.Substring(1, cexpr.GetSize()); }

				// match a constant
			}
		}

		++m_reader;
	}

	if (result != ExpressionFound) {
		out.RemoveAll();
		m_reader = startPosition;
	}
	return result;
}*/

mtlParser::ExpressionResult mtlParser::Expression(const mtlChars &expr, mtlList<mtlChars> &out)
{
	static const mtlChars openBraces  = "([{<\"\'";
	static const mtlChars closeBraces = ")]}>\"\'";
	static const mtlChars variables = "cCifbs_n";

	mtlList<char> braceStack;

	out.RemoveAll();

	const int initalReader = m_reader;
	ExpressionResult result = ExpressionFound;
	bool caseSensitive = true;
	int exprReader = 0;
	bool escapedChar = false;

	enum ReadState
	{
		Constant,
		Variable,
		Escape
	};

	ReadState readState = Normal;

	// DELIMITERS MUST ALWAYS BE A SINGLE CHARACTER, I.E. FIRST INSTANCE OF THE DELIMITER MEANS BREAK
		// OTHERWISE PARSER CANNOT DETERMINE WHERE TO BREAK A VARIABLE EXPRESSION (ESPESCIALLY STRINGS)

	while (!IsEndOfFile() && exprReader < expr.GetSize() && result == ExpressionFound) {

		char ech = expr[exprReader++];

		// Set read state
		if (readState == Constant) {
			if (ech == '%') {
				++exprReader;
				readState = Variable;
			}
		}

		// Process a variable
		if (readState == Variable) {

			if (exprReader >= expr.GetSize()) { result = ExpressionInputError; }

			char varType = expr[exprReader];

			switch (varType) {
			case 'c':
			case 'C':
				caseSensitive = (varType == 'C');
				break;

			case 'i':
			case 'f':
			case 'b':
			case 's':

				// for i/f/b delimiter is implicit (meaning user does not need to explicity specify a delimiter for trailing variable expressions)
					// Expression("%s=%i",out) will work
				// for s delimiter must be explicit (or else the parser parses the entire file)
					// Expression("%s=%s",out) will read all of the text file
					// Expression(%s=%s%_",out) will read one word
					// Expression("%s=%s%n",out) will read one line (spaces after the last word will be trimmed)
					// Expression("%s=%s;",out) will read until ;

				break;

			case '_':
				if (IsWhite()) { ++m_reader; }
				else { result = ExpressionNotFound; }
				break;

			case 'n': {
				bool foundNewl = false;
				while (!IsEndOfFile() && IsWhite() && !foundNewl) {
					if (IsNewl()) {
						foundNewl = true;
					}
					++m_reader;
				}
				if (!foundNewl) { result = ExpressionNotFound; }
				break;
			}

			default:
				result = ExpressionInputError;
				break;
			}

			++exprReader;
			readState = Constant;
		}

		// Process a constant expression
		else if (readState == Constant) {
		}

	}

	if (braceStack.GetSize() != 0) {
		result = ExpressionUnbalancedBraces;
	}

	if (result != ExpressionFound) {
		out.RemoveAll();
		m_reader = initalReader;
	}

	return result;
}
