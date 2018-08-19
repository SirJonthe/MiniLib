#ifndef MTL_ASSET_H_INCLUDED
#define MTL_ASSET_H_INCLUDED

#include "mtlBinaryTree.h"
#include "mtlList.h"
#include "mtlPath.h"
#include "mtlPointer.h"

class mtlAssetInterface
{
private:
	mtlString m_error;

protected:
	void SetError(const mtlChars &error) { m_error.Copy(error); }

public:
	virtual         ~mtlAssetInterface( void ) {}
	virtual bool     Load(const mtlPath &p_filename) = 0;
	//virtual bool   Save(const mtlPath &p_filename) = 0;
	//virtual void   Free( void ) = 0;
	const mtlString &GetError( void ) const { return m_error; }
};

template < typename type_t >
class mtlAsset
{
private:
	struct Instance
	{
		type_t  *asset;
		mtlPath  file;
		Instance( void ) : asset(NULL), file("") {}
		~Instance( void ) { delete asset; }
	};
	struct AssetNode // store this at every node
	{
		mtlHash												hash;
		mutable mtlShared< mtlList< mtlShared<Instance> > >	assets; // all assets with same hash
		bool operator>(const mtlHash &h)    const { return hash.value > h.value; }
		bool operator==(const mtlHash &h)   const { return hash.value == h.value; }
		bool operator>(const AssetNode &n)  const { return hash.value > n.hash.value; }
		bool operator==(const AssetNode &n) const { return hash.value == n.hash.value; }
	};
	static mtlBinaryTree<AssetNode> &GetAssetBank( void );
	static void Purge(mtlNode<AssetNode> *node);
private:
	mtlShared<Instance>	m_ref;
public:
	mtlAsset( void ) {}
	mtlAsset(const mtlAsset<type_t> &asset);
	mtlAsset<type_t> &operator=(const mtlAsset<type_t> &asset);

	const type_t *GetAsset( void ) const;
	void Release( void );
	bool IsNull( void ) const;
	int GetReferenceCount( void ) const;

	template < typename derived_t >
	static mtlAsset<type_t> Load(const mtlChars &file);
	static mtlAsset<type_t> Load(const mtlChars &file);
	static void Purge( void );
};

template < typename type_t >
mtlBinaryTree<typename mtlAsset<type_t>::AssetNode> &mtlAsset<type_t>::GetAssetBank( void )
{
	static mtlBinaryTree<AssetNode> assetBank;
	return assetBank;
}

template < typename type_t >
void mtlAsset<type_t>::Purge(mtlNode<typename mtlAsset<type_t>::AssetNode> *node)
{
	if (node != NULL) {
		Purge(node->GetNegative());
		mtlItem< mtlShared<Instance> > *n = node->GetItem().assets.GetShared()->GetFirst();
		while (n != NULL) {
			if (n->GetItem().GetCount() <= 1) { // should never be less than 1
				n = n->Remove();
			} else {
				n = n->GetNext();
			}
		}
		Purge(node->GetPositive());
		if (node->GetItem().assets.GetShared()->GetSize() == 0) { // only remove once we are done with the node to avoid pressing for bugs
			node = node->Remove();
		}
	}
}

template < typename type_t >
mtlAsset<type_t>::mtlAsset(const mtlAsset<type_t> &asset) : m_ref(asset.m_ref)
{
}

template < typename type_t >
mtlAsset<type_t> &mtlAsset<type_t>::operator=(const mtlAsset<type_t> &asset)
{
	if (&asset != this) {
		m_ref = asset.m_ref;
	}
	return *this;
}

template < typename type_t >
const type_t *mtlAsset<type_t>::GetAsset( void ) const
{
	return m_ref.GetShared() != NULL ? m_ref.GetShared()->asset : NULL;
}

template < typename type_t >
void mtlAsset<type_t>::Release( void )
{
	m_ref.Delete();
}

template < typename type_t >
bool mtlAsset<type_t>::IsNull( void ) const
{
	return m_ref.IsNull();
}

template < typename type_t >
int mtlAsset<type_t>::GetReferenceCount( void ) const
{
	 // the tree always references the object so we need to subtract one to get the relevant count
	return m_ref.GetCount() - 1;
}

template < typename type_t >
template < typename derived_t >
mtlAsset<type_t> mtlAsset<type_t>::Load(const mtlChars &file)
{
	mtlHash hash(file);
	mtlNode<AssetNode> *branch = GetAssetBank().GetRoot();
	if (branch != NULL) {
		branch = branch->Find(hash);
	}

	mtlAsset<type_t> asset;
	mtlItem< mtlShared<Instance> > *node = NULL;
	if (branch != NULL) {
		node = branch->GetItem().assets.GetShared()->GetFirst();
		while (node != NULL) {
			if (node->GetItem().GetShared()->file.GetPath().Compare(file)) {
				break;
			}
			node = node->GetNext();
		}
	}
	if (node == NULL) {
		mtlShared<Instance> instance;
		instance.New();
		instance.GetShared()->asset = new derived_t;
		instance.GetShared()->file = file;
		if (instance.GetShared()->asset->Load(file)) {
			if (branch == NULL) {
				AssetNode assetNode;
				assetNode.assets.New();
				assetNode.hash = hash;
				branch = GetAssetBank().Insert(assetNode);
			}
			branch->GetItem().assets.GetShared()->AddLast(instance);
		}
		asset.m_ref = instance;
	} else {
		asset.m_ref = node->GetItem();
	}
	return asset;
}

template < typename type_t >
mtlAsset<type_t> mtlAsset<type_t>::Load(const mtlChars &file)
{
	return Load<type_t>(file);
}

template < typename type_t >
void mtlAsset<type_t>::Purge( void )
{
	Purge(GetAssetBank().GetRoot());
	// if (GetAssetBank().GetRoot() != NULL && !GetAssetBank().GetRoot()->IsBalanced(2)) { GetAssetBank().GetRoot()->Balance(); } // permit a difference of 2
}

#endif
