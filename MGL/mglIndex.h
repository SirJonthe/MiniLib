#ifndef MGL_INDEX_H_INCLUDED__
#define MGL_INDEX_H_INCLUDED__

template < int n, typename type_t = unsigned short >
struct mglIndex
{
private:
	type_t index[n];
public:
	type_t &operator[](int i) { return index[i]; }
	type_t operator[](int i) const { return index[i]; }
};

template < int n, typename type_t = unsigned short >
class mglIndexer
{
public:
	mglIndexer( void );
	virtual ~mglIndexer( void ) {}
	void SetLimits(mglIndex<n, type_t> lim);
	virtual unsigned long long operator()(mglIndex<n, type_t> idx) const = 0;

protected:
	mglIndex<n, type_t> GetLimit( void ) const { return m_lim; }

private:
	mglIndex<n, type_t> m_lim;
};

template < int n, typename type_t = unsigned short >
class mglLinearIndexer : public mglIndexer<n, type_t>
{
public:
	virtual unsigned long long operator()(mglIndex<n, type_t> idx) const
	{
		mglIndex<n,type_t> lim = this->GetLimit();
		unsigned long long out = 0;
		for (int i = 0; i < n; ++i) {
			unsigned long long offset = 1;
			for (int j = 0; j < i; ++j) { // we can inline this using template metaprogramming...
				offset = offset * lim[j];
			}
			out += idx[i] * offset;
		}
		return out;
	}
};

template < int n, typename type_t = unsigned short >
class mglMortonIndexer : public mglIndexer<n, type_t>
{
};

template < int n, typename type_t = unsigned short >
class mglTileIndexer : public mglIndexer<n, type_t>
{
};

unsigned int mglDitheredIndex(float x, float y, unsigned short sx, unsigned short sy); // this is a SAMPLER rather than an indexer (as such maybe it should not be included?)

#endif // MGL_INDEX_H_INCLUDED__
