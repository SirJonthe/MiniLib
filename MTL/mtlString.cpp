#include <limits.h>
#include <cmath>
#include "mtlString.h"

bool mtlChars::SameAsAny(char a, const char *b, int num)
{
	if (num < 0) { num = mtlChars::GetSizePtr(b); }
	for (int i = 0; i < num; ++i) {
		if (a == b[i]) {
			return true;
		}
	}
	return false;
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

int mtlChars::GetSizePtr(const char *str)
{
	if (str == NULL) { return 0; }
	const char *s = str;
	while (*(s++) != '\0') {}
	return s - str - 1;
}

void mtlChars::ToLower(char *str, int num)
{
	if (num < 0) { num = mtlChars::GetSizePtr(str); }
	for (int i = 0; i < num; ++i) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] += 'a' - 'A';
		}
	}
}

bool mtlSubstring::Compare(const mtlChars &p_str) const
{
	if (p_str.GetSize() != GetSize()) { return false; }
	for (int i = 0; i < GetSize(); ++i) {
		if (p_str.GetChars()[i] != m_str[i]) {
			return false;
		}
	}
	return true;
}


void mtlSubstring::SplitByChar(mtlList<mtlSubstring> &p_out, const mtlChars &p_str) const
{
	const int num = p_str.GetSize();
	int start = 0;
	for (int i = 0; i < m_size; ++i) {
		if (mtlChars::SameAsAny(m_str[i], p_str.GetChars(), num)) {
			//if (i - start > 0) {
			p_out.PushBack(mtlSubstring(*this, start, i));
			//}
			start = i + 1;
		}
	}
	//if (GetSize() - start > 0) {
	p_out.PushBack(mtlSubstring(*this, start, GetSize()));
	//}
}

void mtlSubstring::SplitByString(mtlList<mtlSubstring> &p_out, const mtlChars &p_str) const
{
	int start = 0;
	const int num = p_str.GetSize();
	for (int i = 0; i < GetSize() - num; ++i) {
		if (mtlChars::SameAsAll(m_str+i, p_str.GetChars(), num)) {
			//if (i - start > 0) {
			p_out.PushBack(mtlSubstring(*this, start, i));
			//}
			start = i + num;
		}
	}
	//if (GetSize() - start > 0) {
	p_out.PushBack(mtlSubstring(*this, start, GetSize()));
	//}
}

int mtlSubstring::FindFirstChar(const mtlChars &p_chars) const
{
	const int num = p_chars.GetSize();
	for (int i = 0; i < m_size; ++i) {
		if (mtlChars::SameAsAny(m_str[i], p_chars.GetChars(), num)) {
			return i;
		}
	}
	return -1;
}

int mtlSubstring::FindLastChar(const mtlChars &p_chars) const
{
	const int num = p_chars.GetSize();
	for (int i = m_size - 1; i >= 0; --i) {
		if (mtlChars::SameAsAny(m_str[i], p_chars.GetChars(), num)) {
			return i;
		}
	}
	return -1;
}

int mtlSubstring::FindFirstString(const mtlChars &p_str) const
{
	const int num = p_str.GetSize();
	for (int i = 0; i < m_size; ++i) {
		if (mtlChars::SameAsAll(m_str+i, p_str.GetChars(), num)) {
			return i;
		}
	}
	return -1;
}

int mtlSubstring::FindLastString(const mtlChars &p_str) const
{
	const int num = p_str.GetSize();
	for (int i = m_size - 1; i >= 0; --i) {
		if (mtlChars::SameAsAll(m_str+i, p_str.GetChars(), num)) {
			return i;
		}
	}
	return -1;
}

bool mtlSubstring::ToInt(int &p_out) const
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

bool mtlSubstring::ToFloat(float &p_out) const
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

void mtlString::SetSize(int p_size)
{
	NewPoolPreserve(p_size);
	m_size = p_size;
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

void mtlString::Overwrite(const mtlChars &p_str, int p_at)
{
	const int p_num = p_str.GetSize();
	const int newSize = p_at + p_num;
	char *newPool = NewPool(newSize);
	if (newPool != NULL) {
		const int to = m_size < p_at ? m_size : p_at;
		for (int i = 0; i < to; ++i) {
			newPool[i] = m_str[i];
		}
		delete [] m_str;
		m_str = newPool;
		m_size = newSize;
	}
	for (int i = 0; i < p_num; ++i) {
		m_str[i + p_at] = p_str.GetChars()[i];
	}
	m_str[m_size] = '\0';
}

void mtlString::Remove(int p_begin, int p_num)
{
	for (int i = p_begin; i <= m_size; ++i) {
		m_str[i] = m_str[i + p_num];
	}
	m_size -= p_num;
}

void mtlString::Free( void )
{
	delete [] m_str;
	m_str = NULL;
	m_size = 0;
	m_pool = 0;
}

char *mtlString::NewPool(int p_size)
{
	const int actualSize = p_size + 1;
	if (actualSize > m_pool) {
		m_pool = ((actualSize / m_growth) + 1) * m_growth;
		return new char[m_pool];
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
		newPool[m_size] = '\0';
		delete [] m_str;
		m_str = newPool;
	}
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

void mtlString::CopyPtr(const char *p_str)
{
	if (p_str == NULL) {
		if (m_str != NULL && m_size > 0) {
			m_str[0] = '\0';
			m_size = 0;
		}
	} else {
		const int p_num = mtlChars::GetSizePtr(p_str);
		NewPoolDelete(p_num);
		m_size = p_num;
		for (int i = 0; i < m_size; ++i) {
			m_str[i] = p_str[i];
		}
		m_str[m_size] = '\0';
	}
}

bool mtlString::Compare(const mtlChars &p_str) const
{
	const int p_num = p_str.GetSize();
	if (m_size != p_num) { return false; }
	for (int i = 0; i < m_size; ++i) {
		if (m_str[i] != p_str.GetChars()[i]) { return false; }
	}
	return true;
}

mtlHash::mtlHash(const mtlChars &p_str)
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
