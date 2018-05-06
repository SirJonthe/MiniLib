#include <limits.h>
#include <cmath>
#include <cstdio>
#include "mtlString.h"
#include "mtlMemory.h"
#include "mtlParser.h"

bool mtlChars::SameAsAny(char a, const char *b, int num, bool caseSensitive)
{
	if (num < 0) { num = mtlChars::GetDynamicSize(b); }
	if (caseSensitive) {
		for (int i = 0; i < num; ++i) {
			if (a == b[i]) {
				return true;
			}
		}
	} else {
		for (int i = 0; i < num; ++i) {
			if (ToLower(a) == ToLower(b[i])) {
				return true;
			}
		}
	}
	return false;
}

int mtlChars::SameAsWhich(char a, const char *b, int num, bool caseSensitive)
{
	if (num < 0) { num = mtlChars::GetDynamicSize(b); }
	if (caseSensitive) {
		for (int i = 0; i < num; ++i) {
			if (a == b[i]) {
				return i;
			}
		}
	} else {
		for (int i = 0; i < num; ++i) {
			if (ToLower(a) == ToLower(b[i])) {
				return i;
			}
		}
	}
	return -1;
}

bool mtlChars::SameAsNone(char a, const char *b, int num, bool caseSensitive)
{
	return !SameAsAny(a, b, num, caseSensitive);
}

bool mtlChars::SameAsAll(const char *a, const char *b, int num, bool caseSensitive)
{
	if (caseSensitive) {
		return mtlCompare(a, b, num);
	} else {
		for (int i = 0; i < num; ++i) {
			if (ToLower(a[i]) != ToLower(b[i])) {
				return false;
			}
		}
	}
	return true;
}

int mtlChars::GetDynamicSize(const char *str)
{
	if (str == NULL) { return 0; }
	const char *s = str;
	while (*(s++) != '\0') {}
	return s - str - 1;
}

void mtlChars::ToLower(char *str, int num)
{
	if (num < 0) { num = mtlChars::GetDynamicSize(str); }
	for (int i = 0; i < num; ++i) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] += 'a' - 'A';
		}
	}
}

void mtlChars::ToUpper(char *str, int num)
{
	if (num < 0) { num = mtlChars::GetDynamicSize(str); }
	for (int i = 0; i < num; ++i) {
		if (str[i] >= 'a' && str[i] <= 'z') {
			str[i] -= 'a' - 'A';
		}
	}
}

mtlChars mtlChars::GetTrimmed( void ) const
{
	mtlChars s(*this);
	while (s.m_size > 0 && IsWhitespace(s.m_str[0])) {
		++s.m_str;
		--s.m_size;
	}
	while (s.m_size > 0 && IsWhitespace(s.m_str[s.m_size - 1])) {
		--s.m_size;
	}
	return s;
}

bool mtlChars::ToBool(bool &p_out) const
{
	if (Compare("true")) {
		p_out = true;
		return true;
	} else if (Compare("false")) {
		p_out = false;
		return true;
	}
	return false;
}

bool mtlChars::ToInt(int &p_out) const
{
	// 1) optional + or -
	// 2) numbers only
	if (m_size == 0) { return false; }
	int sign = 0;
	int nums = m_size;
	int i = 0;
	if (m_str[0] == '+') {
		sign = 1;
		--nums;
		++i;
	} else if (m_str[0] == '-') {
		sign = -1;
		--nums;
		++i;
	} else {
		sign = 1;
	}
	if (nums <= 0) { return false; }
	long magnitude = (long)(pow(10.0, (double)(nums - 1)));
	if (magnitude == 0) { return false; }
	long val = 0;
	for (; i < m_size; ++i) {
		char ch = m_str[i];
		if (!mtlChars::IsNumeric(ch)) { return false; }
		val += (m_str[i] - '0') * magnitude;
		magnitude /= 10;
	}
	if (val > (long)INT_MAX || val < (long)INT_MIN) { return false; }
	p_out = (int)(val * sign);
	return true;
}

bool mtlChars::ToFloat(float &p_out) const
{
	// 1) optional + or -
	// 2) numbers only
	// 3) optional .
	// 4) optional f
	if (m_size == 0) { return false; }
	double sign = 0.0;
	double nums = (double)m_size;
	int i = 0;
	if (m_str[0] == '+') {
		sign = 1.0;
		--nums;
		++i;
	} else if (m_str[0] == '-') {
		sign = -1.0;
		--nums;
		++i;
	} else {
		sign = 1;
	}
	int seg1 = FindFirstChar(".");
	if (seg1 > -1) {
		--nums;
	} else {
		seg1 = m_size;
		if (m_str[seg1 - 1] == 'f') {
			--seg1;
		}
	}
	int seg2 = m_size;
	if (m_str[seg2 - 1] == 'f') {
		--seg2;
		--nums;
	}
	if (nums <= 0.0) { return false; }
	double magnitude = pow(10.0, (seg1 - i) - 1.0);
	double val = 0;
	for (; i < seg1; ++i) {
		char ch = m_str[i];
		if (!mtlChars::IsNumeric(ch)) { return false; }
		val += (float)((m_str[i] - '0') * magnitude);
		magnitude /= 10.0;
	}
	magnitude = 0.1;
	++i; // skip the '.'
	for (; i < seg2; ++i) {
		char ch = m_str[i];
		if (!mtlChars::IsNumeric(ch)) { return false; }
		val += (float)((m_str[i] - '0') * magnitude);
		magnitude /= 10.0;
	}
	p_out = (float)(val * sign);
	return true;
}

bool mtlChars::IsBool( void ) const
{
	bool dummy;
	return ToBool(dummy);
}

bool mtlChars::IsInt( void ) const
{
	int dummy;
	return ToInt(dummy);
}

bool mtlChars::IsFloat( void ) const
{
	float dummy;
	return ToFloat(dummy);
}

void mtlChars::SplitByChar(mtlList<mtlChars> &p_out, const mtlChars &p_chars, bool p_ignoreWhitespace) const
{
	p_out.RemoveAll();
	int start = 0;
	for (int end = 0; end < m_size; ++end) {
		char ch = m_str[end];
		if (p_chars.SameAsAny(ch)) {
			mtlChars str(*this, start, end);
			p_out.AddLast(p_ignoreWhitespace ? str.GetTrimmed() : str);
			start = end + 1;
		}
	}
	mtlChars str(*this, start, m_size);
	p_out.AddLast(p_ignoreWhitespace ? str.GetTrimmed() : str);
}

void mtlChars::SplitByChar(mtlList<mtlChars> &p_out, char p_ch, bool p_ignoreWhitespace) const
{
	char chars[] = mtlCharToStr(p_ch);
	SplitByChar(p_out, chars, p_ignoreWhitespace);
}

void mtlChars::SplitByString(mtlList<mtlChars> &p_out, const mtlChars &p_str, bool p_ignoreWhitespace) const
{
	p_out.RemoveAll();
	int start = 0;
	for (int end = 0; end < m_size; ++end) {
		if (p_str.GetSize() != 0 && mtlChars::SameAsAll(m_str + end, p_str.m_str, p_str.m_size)) {
			mtlChars str(*this, start, end);
			p_out.AddLast(p_ignoreWhitespace ? str.GetTrimmed() : str);
			start = end + p_str.m_size;
		}
	}
	if (start < m_size) {
		mtlChars str(*this, start, m_size);
		p_out.AddLast(p_ignoreWhitespace ? str.GetTrimmed() : str);
	}
}

int mtlChars::FindFirstChar(const mtlChars &p_chars) const
{
	const int num = p_chars.m_size;
	for (int i = 0; i < m_size; ++i) {
		if (mtlChars::SameAsAny(m_str[i], p_chars.m_str, num)) {
			return i;
		}
	}
	return -1;
}

int mtlChars::FindFirstChar(char p_ch) const
{
	const char ch[] = mtlCharToStr(p_ch);
	return FindFirstChar(ch);
}

int mtlChars::FindLastChar(const mtlChars &p_chars) const
{
	const int num = p_chars.m_size;
	for (int i = m_size - 1; i >= 0; --i) {
		if (mtlChars::SameAsAny(m_str[i], p_chars.m_str, num)) {
			return i;
		}
	}
	return -1;
}

int mtlChars::FindLastChar(char p_ch) const
{
	const char ch[] = mtlCharToStr(p_ch);
	return FindLastChar(ch);
}

int mtlChars::FindFirstString(const mtlChars &p_str) const
{
	const int num = p_str.m_size;
	for (int i = 0; i < m_size; ++i) {
		if (mtlChars::SameAsAll(m_str+i, p_str.m_str, num)) {
			return i;
		}
	}
	return -1;
}

int mtlChars::FindLastString(const mtlChars &p_str) const
{
	const int num = p_str.m_size;
	for (int i = m_size - 1; i >= 0; --i) {
		if (mtlChars::SameAsAll(m_str+i, p_str.m_str, num)) {
			return i;
		}
	}
	return -1;
}

int mtlChars::CountChars(char ch, bool p_caseSensitive) const
{
	int count = 0;
	if (p_caseSensitive) {
		for (int i = 0; i < m_size; ++i) {
			if (m_str[i] == ch) {
				++count;
			}
		}
	} else {
		ch = ToLower(ch);
		for (int i = 0; i < m_size; ++i) {
			if (ToLower(m_str[i]) == ch) {
				++count;
			}
		}
	}
	return count;
}

bool mtlChars::IsBlank( void ) const
{
	for (int i = 0; i < m_size; ++i) {
		if (!mtlChars::IsWhitespace(m_str[i])) {
			return false;
		}
	}
	return true;
}

bool mtlChars::Compare(const mtlChars &p_str, bool p_caseSensitive) const
{
	if (p_str.m_size != m_size) { return false; }
	return mtlChars::SameAsAll(m_str, p_str.m_str, m_size, p_caseSensitive);
}

bool mtlChars::Compare(char ch, bool p_caseSensitive) const
{
	if (m_size != 1) { return false; }
	return p_caseSensitive ? m_str[0] == ch : ToLower(m_str[0]) == ToLower(ch);
}

char *mtlString::NewPool(int p_size)
{
	const int actualSize = p_size + 1;
	if (actualSize > m_pool) {
		m_pool = ((actualSize / m_growth) + 1) * m_growth;
		char *chars = new char[m_pool];
		chars[m_pool-1] = '\0';
		return chars;
	}
	return NULL;
}

void mtlString::NewPoolDelete(int p_size)
{
	char *newPool = NewPool(p_size);
	if (newPool != NULL) {
		delete [] m_mut_str;
		m_mut_str = newPool;
		m_str = newPool;
		m_mut_str[p_size] = '\0';
	}
}

void mtlString::NewPoolPreserve(int p_size)
{
	char *newPool = NewPool(p_size);
	if (newPool != NULL) {
		mtlCopy(newPool, m_str, m_size);
		delete [] m_mut_str;
		m_mut_str = newPool;
		m_str = newPool;
		m_mut_str[p_size] = '\0';
	}
}

void mtlString::SetSize(int p_size)
{
	NewPoolPreserve(p_size);
	m_size = p_size >= 0 ? p_size : 0;
}

void mtlString::Reserve(int p_size)
{
	int old_size = m_size;
	SetSize(p_size);
	SetSize(old_size);
}

void mtlString::Free( void )
{
	delete [] m_mut_str;
	m_str = NULL;
	m_mut_str = NULL;
	m_size = 0;
	m_pool = 0;
}

void mtlString::Overwrite(const mtlChars &p_str, int p_at)
{
	//if (p_at > m_size) { return; }
	//const int num = p_str.GetSize();
	//const int newSize = num + p_at;
	//const char *str = p_str.GetChars();
	//NewPoolPreserve(newSize);
	//mtlCopy(m_mut_str+p_at, str, num);
	//m_size = m_size > newSize ? m_size : newSize;
	//m_mut_str[m_size] = '\0';

	if (p_at < 0) { return; }
	const int min_size = p_at + p_str.GetSize();
	if (min_size > m_size) {
		SetSize(min_size);
	}
	mtlCopy(m_mut_str + p_at, p_str.GetChars(), p_str.GetSize());
	m_mut_str[m_size] = '\0';
}

void mtlString::Remove(int p_start, int p_end)
{
	int end = p_end < 0 ? m_size : p_end;
	int num = end - p_start;
	mtlCopy(m_mut_str + p_start, m_str + p_start + num, m_size + 1);
	m_size -= num;
	m_mut_str[m_size] = '\0';
}

void mtlString::Insert(const mtlChars &p_str, int p_at)
{
	if (p_at >= m_size) {
		Overwrite(p_str, p_at);
	} else {
		const int p_num = p_str.GetSize();
		const int newSize = m_size + p_num;
		char *newPool = NewPool(newSize);
		m_size = newSize;
		if (newPool != NULL) {
			mtlCopy(newPool, m_str, p_at);
			mtlCopy(newPool + p_at + p_num, m_str + p_at, m_size - p_at);
			delete [] m_mut_str;
			m_mut_str = newPool;
			m_str = newPool;
		} else {
			mtlCopyOverlap(m_mut_str + p_at + p_num, m_str + p_at, m_size - p_at);
		}
		mtlCopy(m_mut_str+p_at, p_str.GetChars(), p_num);
		m_mut_str[m_size] = '\0';
	}
}

mtlString &mtlString::Append(const mtlChars &p_str)
{
	//NewPoolPreserve(m_size + p_str.GetSize());
	//mtlCopy(m_mut_str + m_size, p_str.GetChars(), p_str.GetSize());
	//m_size += p_str.GetSize();
	//m_mut_str[m_size] = '\0';

	Overwrite(p_str, m_size);
	return *this;
}

mtlString &mtlString::Append(char ch)
{
	//NewPoolPreserve(m_size + 1);
	//m_mut_str[m_size] = ch;
	//m_size += 1;
	//m_mut_str[m_size] = '\0';

	const char str[1] = { ch };
	Append(mtlChars::FromDynamic(str, 1));
	return *this;
}

mtlString &mtlString::AppendBool(bool b)
{
	mtlString tmp;
	tmp.FromBool(b);
	return Append(tmp);
}

mtlString &mtlString::AppendInt(int i)
{
	mtlString tmp;
	tmp.FromInt(i);
	return Append(tmp);
}

mtlString &mtlString::AppendFloat(float f)
{
	mtlString tmp;
	tmp.FromFloat(f);
	return Append(f);
}

void mtlString::Copy(const mtlChars &p_str)
{
	if (p_str.GetChars() == NULL) {
		if (m_str != NULL && m_size > 0) {
			m_mut_str[0] = '\0';
			m_size = 0;
		}
	} else {
		const int p_num = p_str.GetSize();
		NewPoolDelete(p_num);
		m_size = p_num;
		mtlCopy(m_mut_str, p_str.GetChars(), m_size);
		m_mut_str[m_size] = '\0';
	}
}

void mtlString::Reverse( void )
{
	const int half_size = m_size / 2;
	for (int i = 0, j = m_size - 1; i < half_size; ++i, --j) {
		char tmp = m_mut_str[i];
		m_mut_str[i] = m_mut_str[j];
		m_mut_str[j] = tmp;
	}
}

void mtlString::FromBool(bool b)
{
	if (b) { Copy("true"); }
	else   { Copy("false"); }
}

void mtlString::FromInt(int i)
{
	char ch_out[32];
	int size = sprintf(ch_out, "%d", i);
	if (size >= 0) {
		Copy(mtlChars(ch_out, size));
	}
}

void mtlString::FromFloat(float f)
{
	char ch_out[32];
	int size = sprintf(ch_out, "%f", f);
	if (size >= 0) {
		Copy(mtlChars(ch_out, size));
	}
}

mtlHash::mtlHash(const mtlChars &p_str) // probably prevents template constructor from getting called
{
	const int size = p_str.GetSize();
	value = 2166136261u;
	for (int i = 0; i < size; ++i) {
		value ^= p_str.GetChars()[i];
		value *= 16777619u;
	}
	value ^= '\0';
	value *= 16777619u;
}

mtlHash::mtlHash(const mtlString &p_str)
{
	const int size = p_str.GetSize();
	value = 2166136261u;
	for (int i = 0; i < size; ++i) {
		value ^= p_str.GetChars()[i];
		value *= 16777619u;
	}
	value ^= '\0';
	value *= 16777619u;
}
