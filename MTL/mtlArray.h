#ifndef MTL_ARRAY_H_INCLUDED
#define MTL_ARRAY_H_INCLUDED

#include "mtlMemory.h"

template < typename type_t >
class mtlArray
{
private:
	type_t *m_arr;
	int     m_size;
	int     m_pool;

public:
	bool poolMemory;

private:
	void MergeSort(type_t *p_arr, type_t *p_out, int p_size) const;
	int  BinarySearch(int p_start, int p_end, const type_t &p_value) const;

public:
	inline            mtlArray( void );
	inline explicit   mtlArray(int p_size);
					  mtlArray(const mtlArray<type_t> &a);
	mtlArray<type_t> &operator=(const mtlArray<type_t> &a);
	inline           ~mtlArray( void );
	void              Copy(const mtlArray<type_t> &p_array);
	void              Copy(const mtlArray<type_t> &p_array, int p_begin, int p_end = -1);
	void              Copy(const type_t *p_array, int p_size);
	void              Create(int p_size);
	void              Resize(int p_size);
	void              MergeSort(mtlArray<type_t> &p_out) const;
	int               BinarySearch(const type_t &p_value) const;
	inline void       Free( void );
	inline int        GetSize( void ) const;
	void              SetCapacity(int p_size);
	int               GetCapacity( void ) const;
	void              Add(const type_t &p_value);
	type_t           &Add( void );
	inline            operator const type_t *( void ) const;
	inline            operator type_t *( void );
};

template < typename type_t >
void mtlArray<type_t>::MergeSort(type_t *p_arr, type_t *p_out, int p_size) const
{
	if (p_size == 1) {
		p_out[0] = p_arr[0];
		return;
	}

	int size1 = p_size>>1;
	type_t *arr1 = new type_t[size1];
	MergeSort(p_arr, arr1, size1);
	int size2 = p_size - size1;
	type_t *arr2 = new type_t[size2];
	MergeSort(p_arr+size1, arr2, size2);

	int top1 = 0, top2 = 0;
	int top = 0;
	while (top1 < size1 && top2 < size2) {
		if (arr1[top1] < arr2[top2]) {
			p_out[top++] = arr1[top1++];
		} else {
			p_out[top++] = arr2[top2++];
		}
	}
	if (top1 == size1) {
		while (top2 < size2) {
			p_out[top++] = arr2[top2++];
		}
	} else {
		while (top1 < size1) {
			p_out[top++] = arr1[top1++];
		}
	}

	delete [] arr1;
	delete [] arr2;
}

template < typename type_t >
int	mtlArray<type_t>::BinarySearch(int p_start, int p_end, const type_t &p_value) const
{
	int mid = (p_start + p_end) >> 1;
	if (p_start >= p_end || m_arr[mid] == p_value) { return mid; }
	else if (m_arr[mid] < p_value) { return BinarySearch(mid + 1, p_end, p_value); }
	return BinarySearch(p_start, mid - 1, p_value);
}

template < typename type_t >
mtlArray<type_t>::mtlArray( void ) :
m_arr(0), m_size(0), m_pool(0), poolMemory(true)
{}

template < typename type_t >
mtlArray<type_t>::mtlArray(int p_size) :
m_arr(0), m_size(0), m_pool(0), poolMemory(true)
{
	Create(p_size);
}

template < typename type_t >
mtlArray<type_t>::mtlArray(const mtlArray<type_t> &p_arr) :
m_arr(0), m_size(0), m_pool(0), poolMemory(true)
{
	Copy(p_arr);
}

template < typename type_t >
mtlArray<type_t> &mtlArray<type_t>::operator=(const mtlArray<type_t> &p_arr)
{
	if (this != &p_arr) {
		Copy(p_arr);
	}
	return *this;
}

template < typename type_t >
mtlArray<type_t>::~mtlArray( void )
{
	delete [] m_arr;
}

template < typename type_t >
void mtlArray<type_t>::Copy(const mtlArray<type_t> &p_array)
{
	if (this != &p_array) {
		Create(p_array.m_size);
		mtlCopy(m_arr, p_array.m_arr, m_size);
	}
}

template < typename type_t >
void mtlArray<type_t>::Copy(const mtlArray<type_t> &p_array, int p_begin, int p_end)
{
	if (p_end < 0) { p_end = p_array.GetSize(); }
	Create(p_end - p_begin);
	mtlCopy(m_arr, p_array.m_arr+p_begin, m_size);
}

template < typename type_t >
void mtlArray<type_t>::Copy(const type_t *p_array, int p_size)
{
	Create(p_size);
	mtlCopy(m_arr, p_array, p_size);
}

template < typename type_t >
void mtlArray<type_t>::Create(int p_size)
{
	if (p_size < 0) { p_size = 0; }
	if ((poolMemory && p_size > m_pool) || (p_size != m_size)) {
		delete [] m_arr;
		if (p_size > 0) {
			m_arr = new type_t[static_cast<unsigned int>(p_size)];
		} else {
			m_arr = NULL;
		}
		m_pool = p_size;
	}
	m_size = p_size;
}

template < typename type_t >
void mtlArray<type_t>::Resize(int p_size)
{
	if (p_size < 0) { p_size = 0; }
	if ((poolMemory && p_size > m_pool) || (!poolMemory && p_size != m_size)) {
		if (p_size > 0) {
			const unsigned int minSize = (p_size < m_size) ? p_size : m_size;
			type_t *data = m_arr;
			m_arr = new type_t[static_cast<unsigned int>(p_size)];
			mtlCopy(m_arr, data, minSize);
			delete [] data;
		} else if (!poolMemory) {
			delete [] m_arr;
			m_arr = NULL;
		}
		m_pool = p_size;
	}
	m_size = p_size;
}

template < typename type_t >
void mtlArray<type_t>::MergeSort(mtlArray &p_out) const
{
	if (m_size > 0) {
		p_out.Create(m_size);
		MergeSort(m_arr, p_out.m_arr, m_size);
	} else {
		p_out.Free();
	}
}

template < typename type_t >
int mtlArray<type_t>::BinarySearch(const type_t &p_value) const
{
	return BinarySearch(0, m_size - 1, p_value);
}

template < typename type_t >
void mtlArray<type_t>::Free( void )
{
	if (!poolMemory) {
		delete [] m_arr;
		m_arr = NULL;
		m_pool = 0;
	}
	m_size = 0;
}

template < typename type_t >
int mtlArray<type_t>::GetSize( void ) const
{
	return m_size;
}

template < typename type_t >
mtlArray<type_t>::operator const type_t *( void ) const
{
	return m_arr;
}

template < typename type_t >
mtlArray<type_t>::operator type_t *( void )
{
	return m_arr;
}

template < typename type_t >
void mtlArray<type_t>::SetCapacity(int p_size)
{
	bool poolValue = poolMemory;
	poolMemory = true;
	int sizeValue = m_size;
	Resize(p_size);
	Resize(sizeValue);
	poolMemory = poolValue;
}

template < typename type_t >
int mtlArray<type_t>::GetCapacity( void ) const
{
	return m_pool;
}

template < typename type_t >
void mtlArray<type_t>::Add(const type_t &p_value)
{
	bool poolValue = poolMemory;
	Resize(m_size+1);
	poolMemory = poolValue;
	m_arr[m_size-1] = p_value;
}

template < typename type_t >
type_t &mtlArray<type_t>::Add( void )
{
	bool poolValue = poolMemory;
	Resize(m_size+1);
	poolMemory = poolValue;
	return m_arr[m_size-1];
}

#endif
