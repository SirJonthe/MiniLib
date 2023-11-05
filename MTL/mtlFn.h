#ifndef MTL_FN_H_INCLUDED__
#define MTL_FN_H_INCLUDED__

#if __cplusplus >= 201103

template < typename ret_t >
class mtlFn
{
public:
	template < typename... parms_t >
	using in = ret_t (*)(parms_t...);
};

#endif

#endif
