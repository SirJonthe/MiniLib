#ifndef MTL_STRING_H_INCLUDED
#define MTL_STRING_H_INCLUDED

#include <cstddef>
#include "mtlList.h"
#include "mtlArray.h"

#define mtlCharToStr(ch) { ch, 0 }

// mtlChars
// Represents a view into a string of characters. Does not own its own memory. This class allows for string manipulation that does not result in a modification of the underlying string.
class mtlChars
{
protected:
	const char *m_str;
	int         m_size;

private:
	static int GetSizeActual(int stringSize, int targetSize = -1) { return (targetSize < 0 || targetSize > stringSize) ? stringSize : targetSize; }
	static int GetSizeActual(int stringSize, int start, int end) { return (end < 0 || (end - start) > stringSize) ? stringSize : (end - start); }

public:
	
	// SameAsAny
	// Checks if sequence of characters 'b' (defined by length 'num') contains the character 'a'. 'caseSensitive' allows/disallows the use of case conversion before comparison.
	static bool SameAsAny(char a, const char *b, int num = -1, bool caseSensitive = false);
	
	// SameAsWhich
	// Same as SameAsAny, but returns the index into 'b' that is the first occurrence of 'a'.
	static int SameAsWhich(char a, const char *b, int num = -1, bool caseSensitive = false);
	
	static bool SameAsNone(char a, const char *b, int num = -1, bool caseSensitive = false);
	
	static bool SameAsAll(const char *a, const char *b, int num, bool caseSensitive = false);
	
	// GetDynamicSize
	// Counts characters in 'str' until a 0 byte is reached.
	static int GetDynamicSize(const char *str);
	
	template < int t_size >
	static int              GetStaticSize(const char (&str)[t_size]) { return t_size - 1; }
	static void             ToLower(char *str, int num = -1);
	static void             ToUpper(char *str, int num = -1);
	inline static mtlChars  FromDynamic(const char *p_str, int p_size = -1);
	inline static mtlChars  FromDynamic(const char *p_str, int p_start, int p_end);
	inline static char      ToLower(char ch);
	inline static char      ToUpper(char ch);
	inline static bool      IsAlpha(char ch);
	inline static bool      IsNumeric(char ch);
	inline static bool      IsAlphanumeric(char ch);
	inline static bool      IsBin(char ch);
	inline static bool      IsHex(char ch);
	inline static bool      IsMath(char ch);
	inline static bool      IsWhitespace(char ch);
	inline static bool      IsNewline(char ch);

public:
	inline                  mtlChars( void );
	inline                  mtlChars(const char &ch);
	template < int t_size >
	inline                  mtlChars(const char (&p_str)[t_size], int p_size = -1);
	inline                  mtlChars(const mtlChars &p_str, int p_size = -1);
	inline mtlChars         &operator=(const mtlChars &p_str);
	template < int t_size >
	inline                  mtlChars(const char (&p_str)[t_size], int p_start, int p_end);
	inline                  mtlChars(const mtlChars &p_str, int p_start, int p_end);
	virtual                ~mtlChars( void ) {}

	mtlChars                GetTrimmed( void ) const;
	bool                    ToBool(bool &p_out) const;
	bool                    ToInt(int &p_out) const;
	bool                    ToFloat(float &p_out) const;
	bool                    IsBool( void ) const;
	bool                    IsInt( void ) const;
	bool                    IsFloat( void ) const;

	void                    SplitByChar(mtlList<mtlChars> &p_out, const mtlChars &p_chars, bool p_ignoreWhitespace = true) const;
	void                    SplitByChar(mtlList<mtlChars> &p_out, char p_ch, bool p_ignoreWhitespace = true) const;
	void                    SplitByChar(mtlArray<mtlChars> &p_out, const mtlChars &p_chars, bool p_ignoreWhitespace = true) const;
	void                    SplitByChar(mtlArray<mtlChars> &p_out, char p_ch, bool p_ignoreWhitespace = true) const;
	void                    SplitByString(mtlList<mtlChars> &p_out, const mtlChars &p_str, bool p_ignoreWhiteSpace = true) const;

	int                     FindFirstChar(const mtlChars &p_chars) const;
	int                     FindFirstChar(char p_ch) const;
	int                     FindLastChar(const mtlChars &p_chars) const;
	int                     FindLastChar(char p_ch) const;
	int                     FindFirstString(const mtlChars &p_str) const;
	int                     FindLastString(const mtlChars &p_str) const;

	int                     CountChars(char ch, bool p_caseSensitive = false) const;

	inline const char      *GetChars( void ) const;
	inline char             operator[](int i) const;
	inline int              GetSize( void ) const;

	inline bool             IsNullTerminated( void ) const;
	bool                    IsBlank( void ) const;

	bool                    Compare(const mtlChars &p_str, bool p_caseSensitive = false) const;
	bool                    Compare(char ch, bool p_caseSensitive = false) const;
	inline bool             SameAsAny(char ch, bool caseSensitive = false) const;
	inline int              SameAsWhich(char ch, bool caseSensitive = false) const;
	inline bool             SameAsNone(char ch, bool caseSensitive = false) const;
	inline bool             operator==(const mtlChars &str) const;
	inline bool             operator!=(const mtlChars &str) const;
};

// mtlString
// Represents a string of characters. Owns its own memory. Allows memory to be manipulated, but may invalidate mtlChar classes that refer to the same memory.
class mtlString : public mtlChars
{
protected:
	char *m_mut_str;
	int   m_pool;
	int   m_growth;
	
private:
			   mtlString(const mtlString&) : mtlChars() {}
	mtlString &operator=(const mtlString&) { return *this; }
	//inline int	CalculatePoolSize(int p_size) const;
	char      *NewPool(int p_size);
	void       NewPoolDelete(int p_size);
	void       NewPoolPreserve(int p_size);
	
public:
	inline             mtlString( void );
	inline explicit    mtlString(const mtlChars &p_str);
	inline            ~mtlString( void );

	void               SetSize(int p_size);
	void               Reserve(int p_size);
	inline void        SetPoolGrowth(int p_growth);

	void               Free( void );
	void               Overwrite(const mtlChars &p_str, int p_at);
	void               Remove(int p_start, int p_end = -1);
	void               Insert(const mtlChars &p_str, int p_at);
	mtlString         &Append(const mtlChars &p_str);
	mtlString         &Append(char ch);
	mtlString         &AppendBool(bool b);
	mtlString         &AppendInt(int i);
	mtlString         &AppendFloat(float f);
	void               Copy(const mtlChars &p_str);

	void               Reverse( void );

	void               FromBool(bool b);
	void               FromInt(int i);
	void               FromFloat(float f);

	inline void        ToLower( void );
	inline void        ToUpper( void );

	inline char       *GetChars( void );
	inline const char *GetChars( void ) const;
	inline char       &operator[](int i);
	inline char        operator[](int i) const;
};

template <unsigned int N, unsigned int I>
struct mtlFNVConstHasher
{
	static unsigned int Hash(const char (&str)[N]) { return (mtlFNVConstHasher<N, I-1>::Hash(str) ^ str[I-1])*16777619u; }
};

template <unsigned int N>
struct mtlFNVConstHasher<N, 1>
{
	static unsigned int Hash(const char (&str)[N]) { return (2166136261u ^ str[0])*16777619u; }
};

class mtlHash
{
public:
	unsigned int value;
public:
	mtlHash( void ) : value(0) {}
	mtlHash(const mtlHash &hash) : value(hash.value) {}
	mtlHash(const mtlString &p_str);
	mtlHash(const mtlChars &p_str); // THIS MIGHT PREVENT TEMPLATE CONSTRUCTOR FROM EVER GETTING CALLED
	mtlHash &operator=(const mtlHash &hash) { value = hash.value; return *this; }
	template < int N >
	mtlHash(const char (&p_str)[N]) : value(mtlFNVConstHasher<N,N>::Hash(p_str)) {} // generates hash at compile time
	bool operator==(mtlHash h) const { return value == h.value; }
	bool operator!=(mtlHash h) const { return value != h.value; }
	bool operator<(mtlHash h) const { return value < h.value; }
	bool operator<=(mtlHash h) const { return value <= h.value; }
	bool operator>(mtlHash h) const { return value > h.value; }
	bool operator>=(mtlHash h) const { return value >= h.value; }
};

mtlChars mtlChars::FromDynamic(const char *p_str, int p_size)
{
	mtlChars ch;
	ch.m_str = p_str;
	ch.m_size = (p_size < 0) ? GetDynamicSize(p_str) : p_size;
	return ch;
}

mtlChars mtlChars::FromDynamic(const char *p_str, int p_start, int p_end)
{
	mtlChars ch;
	ch.m_str = p_str + p_start;
	ch.m_size = p_end - p_start;
	return ch;
}

char mtlChars::ToLower(char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		ch += 'a' - 'A';
	}
	return ch;
}

char mtlChars::ToUpper(char ch)
{
	if (ch >= 'a' && ch <= 'z') {
		ch -= 'a' - 'A';
	}
	return ch;
}

bool mtlChars::IsAlpha(char ch)
{
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool mtlChars::IsNumeric(char ch)
{
	return (ch >= '0' && ch <= '9');
}

bool mtlChars::IsAlphanumeric(char ch)
{
	return IsAlpha(ch) || IsNumeric(ch);
}

bool mtlChars::IsBin(char ch)
{
	return ch == '0' || ch == '1';
}

bool mtlChars::IsHex(char ch)
{
	return IsNumeric(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

bool mtlChars::IsMath(char ch)
{
	return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^';
}

bool mtlChars::IsWhitespace(char ch)
{
	return ch == ' ' || ch == '\t' || IsNewline(ch);
}

bool mtlChars::IsNewline(char ch)
{
	return ch == '\n' || ch == '\r';
}

mtlChars::mtlChars( void ) :
	m_str(NULL), m_size(0)
{}

mtlChars::mtlChars(const char &ch) :
	m_str(&ch), m_size(1)
{}

template < int t_size >
mtlChars::mtlChars(const char (&p_str)[t_size], int p_size) :
	m_str(p_str), m_size(GetSizeActual(t_size-1, p_size))
{}

mtlChars::mtlChars(const mtlChars &p_str, int p_size) :
	m_str(p_str.m_str), m_size(GetSizeActual(p_str.m_size, p_size))
{}

mtlChars &mtlChars::operator=(const mtlChars &p_str)
{
	m_str = p_str.m_str;
	m_size = p_str.m_size;
	return *this;
}

template < int t_size >
mtlChars::mtlChars(const char (&p_str)[t_size], int p_start, int p_end) :
	m_str(p_str + p_start), m_size(GetSizeActual(t_size-1, p_start, p_end))
{}

mtlChars::mtlChars(const mtlChars &p_str, int p_start, int p_end) :
	m_str(p_str.m_str + p_start), m_size(GetSizeActual(p_str.m_size, p_start, p_end))
{}

const char *mtlChars::GetChars( void ) const
{
	return m_str;
}

int mtlChars::GetSize( void ) const
{
	return m_size;
}

bool mtlChars::IsNullTerminated( void ) const
{
	return m_str[m_size] == '\0';
}

char mtlChars::operator[](int i) const
{
	return m_str[i];
}

bool mtlChars::SameAsAny(char ch, bool caseSensitive) const
{
	return SameAsAny(ch, m_str, m_size, caseSensitive);
}

int mtlChars::SameAsWhich(char ch, bool caseSensitive) const
{
	return SameAsWhich(ch, m_str, m_size, caseSensitive);
}

bool mtlChars::SameAsNone(char ch, bool caseSensitive) const
{
	return SameAsNone(ch, m_str, m_size, caseSensitive);
}

bool mtlChars::operator==(const mtlChars &p_str) const
{
	return Compare(p_str, true);
}

bool mtlChars::operator!=(const mtlChars &p_str) const
{
	return !Compare(p_str, true);
}

mtlString::mtlString( void ) :
	mtlChars(), m_mut_str(NULL), m_pool(0), m_growth(32)
{}

mtlString::mtlString(const mtlChars &p_str) :
	mtlChars(), m_mut_str(NULL), m_pool(0), m_growth(32)
{
	Copy(p_str);
}

mtlString::~mtlString( void )
{
	delete [] m_str;
}

void mtlString::SetPoolGrowth(int p_growth)
{
	m_growth = p_growth;
}

void mtlString::ToLower( void )
{
	mtlChars::ToLower(m_mut_str, m_size);
}

void mtlString::ToUpper( void )
{
	mtlChars::ToUpper(m_mut_str, m_size);
}

char *mtlString::GetChars( void )
{
	return m_mut_str;
}

const char *mtlString::GetChars( void ) const
{
	return m_str;
}

char &mtlString::operator[](int i)
{
	return m_mut_str[i];
}

char mtlString::operator[](int i) const
{
	return m_str[i];
}

#endif
