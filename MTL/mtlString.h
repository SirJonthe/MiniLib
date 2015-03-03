#ifndef MTL_STRING_H_INCLUDED__
#define MTL_STRING_H_INCLUDED__

#include <cstddef>
#include "mtlList.h"

class mtlString;

class mtlChars
{
private:
	const char	*m_str;
	int			m_size;

private:
	static int				GetSizeActual(int stringSize, int targetSize = -1) { return (targetSize < 0 || targetSize > stringSize) ? stringSize : targetSize; }
	static int				GetSizeActual(int stringSize, int start, int end) { return (end < 0 || (end - start) > stringSize) ? stringSize : (end - start); }

public:
	static bool				SameAsAny(char a, const char *b, int num = -1);
	static int				SameAsWhich(char a, const char *b, int num = -1);
	static bool				SameAsAll(const char *a, const char *b, int num);
	static int				GetDynamicSize(const char *str);
	template < int t_size >
	static int				GetStaticSize(const char (&str)[t_size]) { return t_size - 1; }
	static void				ToLower(char *str, int num = -1);
	static void				ToUpper(char *str, int num = -1);
	inline static mtlChars	FromDynamic(const char *p_str, int p_size = -1);
	inline static mtlChars	FromDynamic(const char *p_str, int p_start, int p_end);
	inline static char		ToLower(char ch);
	inline static char		ToUpper(char ch);
	inline static bool		IsAlpha(char ch);
	inline static bool		IsNumeric(char ch);
	inline static bool		IsAlphanumeric(char ch);
	inline static bool		IsMath(char ch);
	inline static bool		IsWhitespace(char ch);
	inline static bool		IsNewline(char ch);

public:
	inline					mtlChars( void );
	template < int t_size >
	inline					mtlChars(const char (&p_str)[t_size], int p_size = -1);
	inline					mtlChars(const mtlChars &p_str, int p_size = -1);
	inline					mtlChars(const mtlString &p_str, int p_size = -1);
	template < int t_size >
	inline					mtlChars(const char (&p_str)[t_size], int p_start, int p_end);
	inline					mtlChars(const mtlChars &p_str, int p_start, int p_end);
	inline					mtlChars(const mtlString &p_str, int p_start, int p_end);

	mtlChars				GetSubstring(int p_start, int p_end = -1) const;
	mtlChars				GetTrimmed( void ) const;
	bool					ToBool(bool &p_out) const;
	bool					ToInt(int &p_out) const;
	bool					ToFloat(float &p_out) const;

	void					Trim( void );
	void					Substring(int p_start, int p_end);

	void					SplitByChar(mtlList<mtlChars> &p_out, const mtlChars &p_chars, bool p_ignoreWhiteSpace = true) const;
	void					SplitByString(mtlList<mtlChars> &p_out, const mtlChars &p_str, bool p_ignoreWhiteSpace = true) const;

	int						FindFirstChar(const mtlChars &p_chars) const;
	int						FindLastChar(const mtlChars &p_chars) const;
	int						FindFirstString(const mtlChars &p_str) const;
	int						FindLastString(const mtlChars &p_str) const;

	inline const char		*GetChars( void ) const;
	inline int				GetSize( void ) const;

	inline bool				IsNullTerminated( void ) const;
	inline bool				IsNull( void ) const;

	inline char				operator[](int i) const;

	bool					Compare(const mtlChars &p_str, bool p_caseSensitive = true) const;
	inline bool				SameAsAny(char ch) const;
	inline int				SameAsWhich(char ch) const;
	inline bool				operator==(const mtlChars &str) const;
	inline bool				operator!=(const mtlChars &str) const;
};

class mtlString
{
private:
	char	*m_str;
	int		m_size;
	int		m_pool;
	int		m_growth;
	
private:
				mtlString(const mtlString&) {}
	mtlString	&operator=(const mtlString&) { return *this; }
	//inline int	CalculatePoolSize(int p_size) const;
	char		*NewPool(int p_size);
	void		NewPoolDelete(int p_size);
	void		NewPoolPreserve(int p_size);
	
public:
	inline				mtlString( void );
	inline explicit		mtlString(const mtlChars &p_str);
	inline				~mtlString( void );

	inline int			GetSize( void ) const;
	void				SetSize(int p_size);

	inline void			SetPoolGrowth(int p_growth);

	void				Insert(const mtlChars &p_str, int p_at);
	mtlString			&Append(const mtlChars &p_str);
	void				Overwrite(const mtlChars &p_str, int p_at);
	void				Remove(int p_start, int p_end = -1);
	void				Free( void );
	void				Copy(const mtlChars &p_str);

	inline void			ToLower( void );
	inline void			ToUpper( void );
	inline mtlChars		GetTrimmed( void ) const;
	inline mtlChars		GetSubstring(int p_start, int p_end = -1) const;

	inline const char	*GetChars( void ) const;
	inline char			*GetChars( void );
	inline char			operator[](int i) const;
	inline char			&operator[](int i);

	inline void			SplitByChar(mtlList<mtlChars> &p_out, const mtlChars &p_str, bool p_ignoreWhiteSpace = true) const;
	inline void			SplitByString(mtlList<mtlChars> &p_out, const mtlChars &p_str, bool p_ignoreWhiteSpace = true) const;

	inline int			FindFirstChar(const mtlChars &p_str) const;
	inline int			FindLastChar(const mtlChars &p_str) const;
	inline int			FindFirstString(const mtlChars &p_str) const;
	inline int			FindLastString(const mtlChars &p_str) const;

	inline bool			ToBool(bool &p_out) const;
	inline bool			ToInt(int &p_out) const;
	inline bool			ToFloat(float &p_out) const;
	bool				FromBool(bool b);
	bool				FromInt(int i);
	bool				FromFloat(float f);

	bool				Compare(const mtlChars &p_str, bool p_caseSensitive = true) const;
	bool				operator==(const mtlChars &p_str) const;
	bool				operator!=(const mtlChars &p_str) const;
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
	template < int N >
	mtlHash(const char (&p_str)[N]) : value(mtlFNVConstHasher<N,N>::Hash(p_str)) {}
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

template < int t_size >
mtlChars::mtlChars(const char (&p_str)[t_size], int p_size) :
	m_str(p_str), m_size(GetSizeActual(t_size-1, p_size))
{}

mtlChars::mtlChars(const mtlChars &p_str, int p_size) :
	m_str(p_str.m_str), m_size(GetSizeActual(p_str.m_size, p_size))
{}

mtlChars::mtlChars(const mtlString &p_str, int p_size) :
	m_str(p_str.GetChars()), m_size(GetSizeActual(p_str.GetSize(), p_size))
{}

template < int t_size >
mtlChars::mtlChars(const char (&p_str)[t_size], int p_start, int p_end) :
	m_str(p_str + p_start), m_size(GetSizeActual(t_size-1, p_start, p_end))
{}

mtlChars::mtlChars(const mtlChars &p_str, int p_start, int p_end) :
	m_str(p_str.m_str + p_start), m_size(GetSizeActual(p_str.m_size, p_start, p_end))
{}

mtlChars::mtlChars(const mtlString &p_str, int p_start, int p_end) :
	m_str(p_str.GetChars() + p_start), m_size(GetSizeActual(p_str.GetSize(), p_start, p_end))
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

bool mtlChars::IsNull( void ) const
{
	return m_str == NULL;
}

char mtlChars::operator[](int i) const
{
	return m_str[i];
}

bool mtlChars::SameAsAny(char ch) const
{
	return SameAsAny(ch, m_str, m_size);
}

int mtlChars::SameAsWhich(char ch) const
{
	return SameAsWhich(ch, m_str, m_size);
}

bool mtlChars::operator==(const mtlChars &p_str) const
{
	return Compare(p_str);
}

bool mtlChars::operator!=(const mtlChars &p_str) const
{
	return !Compare(p_str);
}

mtlString::mtlString( void ) :
m_str(NULL), m_size(0), m_pool(0), m_growth(32)
{}

mtlString::mtlString(const mtlChars &p_str) :
	m_str(NULL), m_size(0), m_pool(0), m_growth(32)
{
	Copy(p_str);
}

mtlString::~mtlString( void )
{
	delete [] m_str;
}

int mtlString::GetSize( void ) const
{
	return m_size;
}

void mtlString::SetPoolGrowth(int p_growth)
{
	m_growth = p_growth;
}

void mtlString::ToLower( void )
{
	mtlChars::ToLower(m_str, m_size);
}

void mtlString::ToUpper( void )
{
	mtlChars::ToUpper(m_str, m_size);
}

mtlChars mtlString::GetTrimmed( void ) const
{
	return mtlChars(*this, 0, m_size).GetTrimmed();
}

mtlChars mtlString::GetSubstring(int p_start, int p_end) const
{
	if (p_end < 0) {
		return mtlChars(*this, p_start, m_size);
	}
	return mtlChars(*this, p_start, p_end);
}

const char *mtlString::GetChars( void ) const
{
	return m_str;
}

char *mtlString::GetChars( void )
{
	return m_str;
}

char mtlString::operator[](int i) const
{
	return m_str[i];
}

char &mtlString::operator[](int i)
{
	return m_str[i];
}

void mtlString::SplitByChar(mtlList<mtlChars> &p_out, const mtlChars &p_str, bool p_ignoreWhiteSpace) const
{
	mtlChars(*this).SplitByChar(p_out, p_str, p_ignoreWhiteSpace);
}

void mtlString::SplitByString(mtlList<mtlChars> &p_out, const mtlChars &p_str, bool p_ignoreWhiteSpace) const
{
	mtlChars(*this).SplitByString(p_out, p_str, p_ignoreWhiteSpace);
}

int mtlString::FindFirstChar(const mtlChars &p_chars) const
{
	return mtlChars(*this).FindFirstChar(p_chars);
}

int mtlString::FindLastChar(const mtlChars &p_chars) const
{
	return mtlChars(*this).FindLastChar(p_chars);
}

int mtlString::FindFirstString(const mtlChars &p_str) const
{
	return mtlChars(*this).FindFirstString(p_str);
}

int mtlString::FindLastString(const mtlChars &p_str) const
{
	return mtlChars(*this).FindLastString(p_str);
}

bool mtlString::ToBool(bool &p_out) const
{
	return mtlChars(*this).ToBool(p_out);
}

bool mtlString::ToInt(int &p_out) const
{
	return mtlChars(*this).ToInt(p_out);
}

bool mtlString::ToFloat(float &p_out) const
{
	return mtlChars(*this).ToFloat(p_out);
}

#endif
