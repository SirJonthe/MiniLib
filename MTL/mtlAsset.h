#ifndef MTL_ASSET_H_INCLUDED__
#define MTL_ASSET_H_INCLUDED__

#include "mtlList.h"
#include "mtlDirectory.h"

class mtlAssetInterface
{
protected:
	mtlString m_error;
public:
	virtual			~mtlAssetInterface( void ) {}
	virtual bool	Load(const mtlDirectory &p_filename) = 0;
	const mtlString	&GetError( void ) const { return m_error; }
};

template < typename type_t >
class mtlAsset
{
private:
	struct Ref
	{
		type_t			*asset;
		int				count;
		mtlHash			hash;
		mtlNode<Ref>	*node;
		mtlDirectory	filename;
						Ref( void ) : asset(NULL), count(0), hash(""), node(NULL), filename("") {}
	};
private:
	static mtlList<Ref> assets;
private:
	Ref			*m_ref;
	mtlString	m_error;
private:
	void CopyReference(const mtlAsset<type_t> &p_asset);
	void DeleteReference( void );
	bool IsValid( void ) const { return m_ref != NULL && m_ref->count > 0 && m_ref->asset != NULL; }
public:
						mtlAsset( void ) : m_ref(NULL) {}
	explicit			mtlAsset(const mtlAsset<type_t> &p_asset) : m_ref(NULL) { CopyReference(p_asset); }
	explicit			mtlAsset(const mtlChars &p_filename) : m_ref(NULL) { Load(p_filename); }
						~mtlAsset( void ) { DeleteReference(); }
	mtlAsset<type_t>	&operator=(const mtlAsset<type_t> &p_asset) { CopyReference(p_asset); return *this; }
	bool				Load(const mtlChars &p_filename);
	void				Invalidate( void ) { DeleteReference(); }
	const type_t		*GetAsset( void ) const { return IsValid() ? m_ref->asset : NULL; }
	const mtlDirectory	&GetFilename( void ) const { return IsValid() ? m_ref->filename : mtlDirectory(""); }
	const mtlString		&GetError( void ) const { return m_error; }
	static void			Purge( void );
	static int			GetSize( void ) { return assets.GetSize(); }
};

template < typename type_t >
mtlList<typename mtlAsset<type_t>::Ref> mtlAsset<type_t>::assets;

template < typename type_t >
void mtlAsset<type_t>::CopyReference(const mtlAsset<type_t> &p_asset)
{
	if (p_asset.m_ref != m_ref) {
		DeleteReference();
		if (p_asset.IsValid()) {
			m_ref = p_asset.m_ref;
			++m_ref->count;
		}
		m_error.Copy(p_asset.m_error);
	}
}

template < typename type_t >
void mtlAsset<type_t>::DeleteReference( void )
{
	if (IsValid()) {
		--m_ref->count;
	}
	m_ref = NULL;
}

template < typename type_t >
bool mtlAsset<type_t>::Load(const mtlChars &p_filename)
{
	DeleteReference();
	mtlDirectory dir = p_filename;
	mtlHash hash = mtlHash(dir.GetDirectory()); // should really forcibly convert filename to path relative to working directory
	mtlNode<Ref> *node = assets.GetFront();
	while (node != NULL) {
		if (node->value.hash.value == hash.value) { break; } // REMEMBER: must also compare filename to account for hash collision
		node = node->GetNext();
	}
	if (node != NULL) {
		m_ref = &node->value;
		++m_ref->count;
	} else {
		assets.PushBack(Ref());
		m_ref = &assets.GetBack()->value;
		m_ref->asset = new type_t;
		if (m_ref->asset->Load(p_filename)) {
			m_ref->count = 1;
			m_ref->hash = hash;
			m_ref->node = assets.GetBack();
			m_ref->filename = dir;
			m_error.Copy("");
		} else {
			m_error.Copy(m_ref->asset->GetError());
			delete m_ref->asset;
			assets.Remove(assets.GetBack());
			m_ref = NULL;
			return false;
		}
	}
	return true;
}

template < typename type_t >
void mtlAsset<type_t>::Purge( void )
{
	mtlNode<Ref> *node = assets.GetFront();
	while (node != NULL) {
		if (node->value.count <= 0) {
			delete node->value.asset;
			node = assets.Remove(node);
		} else {
			node = node->GetNext();
		}
	}
}

#endif
