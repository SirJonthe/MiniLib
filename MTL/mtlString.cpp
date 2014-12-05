#include <limits.h>
#include <cmath>
#include <cstdio>
#include "mtlString.h"

bool mtlChars::SameAsAny(char a, const char *b, int num)
{
	if (num < 0) { num = mtlChars::GetDynamicSize(b); }
	for (int i = 0; i < num; ++i) {
		if (a == b[i]) {
			return true;
		}
	}
	return false;
}

int mtlChars::SameAsWhich(char a, const char *b, int num)
{
	if (num < 0) { num = mtlChars::GetDynamicSize(b); }
	for (int i = 0; i < num; ++i) {
		if (a == b[i]) {
			return i;
		}
	}
	return -1;
}

bool mtlChars::SameAsAll(const char *a, const char *b, int num)
{
	for (int i = 0; i < num; ++i) {
		if (a[i] != b[i]) {
			return false;
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

mtlChars mtlChars::GetSubstring(int p_start, int p_end) const
{
	return mtlChars(*this, p_start, p_end);
}

mtlChars mtlChars::GetTrimmed( void ) const
{
	mtlChars s(*this);
	s.Trim();
	return s;
}

bool mtlChars::ToBool(bool &p_out) const
{
	if (Compare("true", false)) {
		p_out = true;
		return true;
	}
	if (Compare("false", false)) {
		p_out = false;
		return true;
	}
	return false;
}

bool mtlChars::ToInt(int &p_out) const
{
	// 1) optional + or -
	// 2) numbers only
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
		if (ch < '0' || ch > '9') { return false; }
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
		if (ch < '0' || ch > '9') { return false; }
		val += (float)((m_str[i] - '0') * magnitude);
		magnitude /= 10.0;
	}
	magnitude = 0.1;
	++i; // skip the '.'
	for (; i < seg2; ++i) {
		char ch = m_str[i];
		if (ch < '0' || ch > '9') { return false; }
		val += (float)((m_str[i] - '0') * magnitude);
		magnitude /= 10.0;
	}
	p_out = (float)(val * sign);
	return true;
}

void mtlChars::Trim( void )
{
	for (int i = 0; i < m_size; ++i) {
		if (m_str[0] == ' ' || m_str[0] == '\t' || m_str[0] == '\n' || m_str[0] == '\r') {
			++m_str;
			--m_size;
		} else {
			break;
		}
	}

	for (int i = m_size - 1; i >= 0; --i) {
		if (m_str[i] == ' ' || m_str[i] == '\t' || m_str[i] == '\n' || m_str[i] == '\r') {
			--m_size;
		} else {
			break;
		}
	}
}

void mtlChars::Substring(int p_start, int p_end)
{
	*this = mtlChars(*this, p_start, p_end);
}

void mtlChars::SplitByChar(mtlList<mtlChars> &p_out, const mtlChars &p_str, bool p_ignoreWhiteSpace) const
{
	const int num = p_str.GetSize();
	int start = 0;
	for (int i = 0; i < m_size; ++i) {
		if (mtlChars::SameAsAny(m_str[i], p_str.GetChars(), num)) {
			//if (i - start > 0) {
			mtlChars str(*this, start, i);
			if (p_ignoreWhiteSpace) {
				str.Trim();
			}
			p_out.AddLast(str);
			//}
			start = i + 1;
		}
	}
	//if (GetSize() - start > 0) {
	mtlChars str(*this, start, GetSize());
	if (p_ignoreWhiteSpace) {
		str.Trim();
	}
	p_out.AddLast(str);
	//}
}

void mtlChars::SplitByString(mtlList<mtlChars> &p_out, const mtlChars &p_str, bool p_ignoreWhiteSpace) const
{
	int start = 0;
	const int num = p_str.GetSize();
	for (int i = 0; i < GetSize() - num; ++i) {
		if (mtlChars::SameAsAll(m_str+i, p_str.GetChars(), num)) {
			//if (i - start > 0) {
			mtlChars str(*this, start, i);
			if (p_ignoreWhiteSpace) {
				str.Trim();
			}
			p_out.AddLast(str);
			//}
			start = i + num;
		}
	}
	//if (GetSize() - start > 0) {
	mtlChars str(*this, start, GetSize());
	if (p_ignoreWhiteSpace) {
		str.Trim();
	}
	p_out.AddLast(str);
	//}
}

int mtlChars::FindFirstChar(const mtlChars &p_chars) const
{
	const int num = p_chars.GetSize();
	for (int i = 0; i < m_size; ++i) {
		if (mtlChars::SameAsAny(m_str[i], p_chars.GetChars(), num)) {
			return i;
		}
	}
	return -1;
}

int mtlChars::FindLastChar(const mtlChars &p_chars) const
{
	const int num = p_chars.GetSize();
	for (int i = m_size - 1; i >= 0; --i) {
		if (mtlChars::SameAsAny(m_str[i], p_chars.GetChars(), num)) {
			return i;
		}
	}
	return -1;
}

int mtlChars::FindFirstString(const mtlChars &p_str) const
{
	const int num = p_str.GetSize();
	for (int i = 0; i < m_size; ++i) {
		if (mtlChars::SameAsAll(m_str+i, p_str.GetChars(), num)) {
			return i;
		}
	}
	return -1;
}

int mtlChars::FindLastString(const mtlChars &p_str) const
{
	const int num = p_str.GetSize();
	for (int i = m_size - 1; i >= 0; --i) {
		if (mtlChars::SameAsAll(m_str+i, p_str.GetChars(), num)) {
			return i;
		}
	}
	return -1;
}

bool mtlChars::Compare(const mtlChars &p_str, bool p_caseSensitive) const
{
	if (p_str.GetSize() != GetSize()) { return false; }
	if (p_caseSensitive) {
		for (int i = 0; i < GetSize(); ++i) {
			if (p_str.GetChars()[i] != m_str[i]) {
				return false;
			}
		}
	} else {
		mtlString a;
		a.Copy(*this);
		a.ToLower();
		mtlString b;
		b.Copy(p_str);
		b.ToLower();
		for (int i = 0; i < GetSize(); ++i) {
			if (a.GetChars()[i] != b.GetChars()[i]) {
				return false;
			}
		}
	}
	return true;
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
		delete [] m_str;
		m_str = newPool;
	}
}

void mtlString::NewPoolPreserve(int p_size)
{
	char *newPool = NewPool(p_size);
	if (newPool != NULL) {
		for (int i = 0; i < m_size; ++i) {
			newPool[i] = m_str[i];
		}
		delete [] m_str;
		m_str = newPool;
	}
}

void mtlString::SetSize(int p_size)
{
	NewPoolPreserve(p_size);
	m_size = p_size >= 0 ? p_size : 0;
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
			for (int i = 0; i < p_at; ++i) {
				newPool[i] = m_str[i];
			}
			for (int i = m_size-1; i >= p_at + p_num; --i) {
				newPool[i] = m_str[i - p_num];
			}
			delete [] m_str;
			m_str = newPool;
		} else {
			for (int i = 0; i < p_at; ++i) {
				m_str[i] = m_str[i];
			}
			for (int i = m_size-1; i >= p_at + p_num; --i) {
				m_str[i] = m_str[i - p_num];
			}
		}
		for (int i = 0; i < p_num; ++i) {
			m_str[i + p_at] = p_str.GetChars()[i];
		}
		m_str[m_size] = '\0';
	}
}

mtlString &mtlString::Append(const mtlChars &p_str)
{
	if (m_size + p_str.GetSize() >= m_pool) {
		NewPoolPreserve(m_size + p_str.GetSize());
	}
	for (int i = 0; i < p_str.GetSize(); ++i) {
		m_str[i + m_size] = p_str.GetChars()[i];
	}
	m_size += p_str.GetSize();
	m_str[m_size] = '\0';
	return *this;
}

void mtlString::Overwrite(const mtlChars &p_str, int p_at)
{
	if (p_at > m_size) { return; }
	const int num = p_str.GetSize();
	const int newSize = num + p_at;
	const char *str = p_str.GetChars();
	NewPoolPreserve(newSize);
	for (int i = 0; i < num; ++i) {
		m_str[p_at+i] = str[i];
	}
	m_size = m_size > newSize ? m_size : newSize;
	m_str[m_size] = '\0';
}

void mtlString::Remove(int p_begin, int p_num)
{
	for (int i = p_begin; i <= m_size; ++i) {
		m_str[i] = m_str[i + p_num];
	}
	m_size -= p_num;
	m_str[m_size] = '\0';
}

void mtlString::Free( void )
{
	delete [] m_str;
	m_str = NULL;
	m_size = 0;
	m_pool = 0;
}

void mtlString::Copy(const mtlChars &p_str)
{
	if (p_str.GetChars() == NULL) {
		if (m_str != NULL && m_size > 0) {
			m_str[0] = '\0';
			m_size = 0;
		}
	} else {
		const int p_num = p_str.GetSize();
		NewPoolDelete(p_num);
		m_size = p_num;
		for (int i = 0; i < m_size; ++i) {
			m_str[i] = p_str.GetChars()[i];
		}
		m_str[m_size] = '\0';
	}
}

bool mtlString::Compare(const mtlChars &p_str, bool p_caseSensitive) const
{
	const int p_num = p_str.GetSize();
	if (m_size != p_num) { return false; }
	if (p_caseSensitive) {
		for (int i = 0; i < m_size; ++i) {
			if (m_str[i] != p_str.GetChars()[i]) { return false; }
		}
	} else {
		mtlString a;
		a.Copy(*this);
		a.ToLower();
		mtlString b;
		b.Copy(p_str);
		b.ToLower();
		for (int i = 0; i < m_size; ++i) {
			if (a.GetChars()[i] != b.GetChars()[i]) { return false; }
		}
	}
	return true;
}

bool mtlString::FromBool(bool b)
{
	if (b) { Copy("true"); }
	else { Copy("false"); }
	return true;
}

bool mtlString::FromInt(int i)
{
	char ch_out[32];
	int size = sprintf(ch_out, "%d", i);
	if (size >= 0) {
		Copy(mtlChars::FromDynamic(ch_out, size));
	}
	return size >= 0;
}

bool mtlString::FromFloat(float f)
{
	char ch_out[32];
	int size = sprintf(ch_out, "%f", f);
	if (size >= 0) {
		Copy(mtlChars::FromDynamic(ch_out, size));
	}
	return size >= 0;
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
