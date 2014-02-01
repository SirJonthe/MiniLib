#ifndef MTL_STRING_H_INCLUDED__
#define MTL_STRING_H_INCLUDED__

#include <cstddef>
#include "mtlList.h"

class mtlString;
class mtlSubstring;

class mtlChars
{
private:
	const char	*m_str;
	mutable int	m_size;
public:
	static bool	SameAsAny(char a, const char *b, int num = -1);
	static bool	SameAsAll(const char *a, const char *b, int num);
	static int	GetSizePtr(const char *str);
	template < int size >
	static int GetSizeArr(const char (&str)[size]) { return size - 1; }
	static void	ToLower(char *str, int num = -1);
public:
	inline				mtlChars( void );
	template < int p_size >
	inline				mtlChars(const char (&p_str)[p_size]);
	inline				mtlChars(const mtlSubstring &p_str);
	inline				mtlChars(const mtlString &p_str);
	inline const char	*GetChars( void ) const;
	inline int			GetSize( void ) const;
};

class mtlSubstring
{
private:
	//const mtlString	*m_parent;
	const char		*m_str;
	int				m_size;
public:
	inline				mtlSubstring( void );
	inline				mtlSubstring(const mtlChars &p_parent, int p_start, int p_end);
	inline explicit		mtlSubstring(const mtlChars &p_parent);
	inline int			GetSize( void ) const;
	inline mtlSubstring	GetSubstring(int p_start, int p_end = -1) const;
	inline const char	*GetChars( void ) const; // this one is dangerous (not NULL terminated by m_end)
	bool				Compare(const mtlChars &p_str) const;
	void				SplitByChar(mtlList<mtlSubstring> &p_out, const mtlChars &p_str) const;
	void				SplitByString(mtlList<mtlSubstring> &p_out, const mtlChars &p_str) const;
	int					FindFirstChar(const mtlChars &p_str) const;
	int					FindLastChar(const mtlChars &p_str) const;
	int					FindFirstString(const mtlChars &p_str) const;
	int					FindLastString(const mtlChars &p_str) const;
	bool				ToInt(int &p_out) const;
	bool				ToFloat(float &p_out) const;
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
	inline					mtlString( void );
	inline					mtlString(const mtlChars &p_str);
	inline					~mtlString( void );
	inline int				GetSize( void ) const;
	void					SetSize(int p_size);
	inline void				SetPoolGrowth(int p_growth);
	void					Insert(const mtlChars &p_str, int p_at);
	inline void				Append(const mtlChars &p_str);
	void					Overwrite(const mtlChars &p_str, int p_at);
	void					Remove(int p_begin, int p_num);
	void					Free( void );
	void					Copy(const mtlChars &p_str);
	void					CopyPtr(const char *p_str);
	bool					Compare(const mtlChars &p_str) const;
	inline void				ToLower( void );
	inline mtlSubstring		GetSubstring(int p_start, int p_end = -1) const;
	inline const char		*GetChars( void ) const;
	inline char				*GetChars( void );
	inline void				SplitByChar(mtlList<mtlSubstring> &p_out, const mtlChars &p_str) const;
	inline void				SplitByString(mtlList<mtlSubstring> &p_out, const mtlChars &p_str) const;
	inline int				FindFirstChar(const mtlChars &p_str) const;
	inline int				FindLastChar(const mtlChars &p_str) const;
	inline int				FindFirstString(const mtlChars &p_str) const;
	inline int				FindLastString(const mtlChars &p_str) const;
	inline bool				ToInt(int &p_out) const;
	inline bool				ToFloat(float &p_out) const;
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
	mtlHash(const mtlChars &p_str);
	template < int N >
	mtlHash(const char (&p_str)[N]) : value(mtlFNVConstHasher<N,N>::Hash(p_str)) {}
};

mtlChars::mtlChars( void ) :
	m_str(NULL), m_size(0)
{}

template < int p_size >
mtlChars::mtlChars(const char (&p_str)[p_size]) :
	m_str(p_str), m_size(p_size - 1)
{}

mtlChars::mtlChars(const mtlSubstring &p_str) :
	m_str(p_str.GetChars()), m_size(p_str.GetSize())
{}

mtlChars::mtlChars(const mtlString &p_str) :
	m_str(p_str.GetChars()), m_size(p_str.GetSize())
{}

const char *mtlChars::GetChars( void ) const
{
	return m_str;
}

int mtlChars::GetSize( void ) const
{
	//if (m_size < 0) { m_size = mtlChars::GetSize(m_str); } // lazy eval
	return m_size;
}

mtlSubstring::mtlSubstring( void ) :
m_str(NULL), m_size(0)
{}

mtlSubstring::mtlSubstring(const mtlChars &p_parent, int p_start, int p_end) :
m_str(p_parent.GetChars()+p_start), m_size(p_end - p_start)
{}

mtlSubstring::mtlSubstring(const mtlChars &p_parent) :
m_str(p_parent.GetChars()), m_size(p_parent.GetSize())
{}

int mtlSubstring::GetSize( void ) const
{
	return m_size;
}

mtlSubstring mtlSubstring::GetSubstring(int p_start, int p_end) const
{
	if (p_end < 0) {
		return mtlSubstring(*this, p_start, m_size - p_start);
	}
	return mtlSubstring(*this, p_start, p_end);
}

const char *mtlSubstring::GetChars( void ) const
{
	return m_str;
}

void mtlString::ToLower( void )
{
	mtlChars::ToLower(m_str, m_size);
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

void mtlString::Append(const mtlChars &p_str)
{
	Insert(p_str, m_size);
}

mtlSubstring mtlString::GetSubstring(int p_start, int p_end) const
{
	if (p_end < 0) {
		return mtlSubstring(*this, p_start, m_size);
	}
	return mtlSubstring(*this, p_start, p_end);
}

const char *mtlString::GetChars( void ) const
{
	return m_str;
}

char *mtlString::GetChars( void )
{
	return m_str;
}

void mtlString::SplitByChar(mtlList<mtlSubstring> &p_out, const mtlChars &p_str) const
{
	mtlSubstring(*this).SplitByChar(p_out, p_str);
}

void mtlString::SplitByString(mtlList<mtlSubstring> &p_out, const mtlChars &p_str) const
{
	mtlSubstring(*this).SplitByString(p_out, p_str);
}

int mtlString::FindFirstChar(const mtlChars &p_chars) const
{
	return mtlSubstring(*this).FindFirstChar(p_chars);
}

int mtlString::FindLastChar(const mtlChars &p_chars) const
{
	return mtlSubstring(*this).FindLastChar(p_chars);
}

int mtlString::FindFirstString(const mtlChars &p_str) const
{
	return mtlSubstring(*this).FindFirstString(p_str);
}

int mtlString::FindLastString(const mtlChars &p_str) const
{
	return mtlSubstring(*this).FindLastString(p_str);
}

bool mtlString::ToInt(int &p_out) const
{
	return mtlSubstring(*this).ToInt(p_out);
}

bool mtlString::ToFloat(float &p_out) const
{
	return mtlSubstring(*this).ToFloat(p_out);
}

#endif
