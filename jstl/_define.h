
#ifndef __Define_H
#define __Define_H

#include <initializer_list>

#include <vector>

#include <functional>

#include <algorithm>

using namespace std;

template<class _Iter>
#ifdef _MSC_VER
using checkIter_t = enable_if_t<_Is_iterator<_Iter>::value>;
#else
using checkIter_t = std::_RequireInputIter<_Iter>;
#endif

template<class T>
using checkContainer_t = checkIter_t<decltype(declval<T>().begin())>;

template <typename T, typename _RetType, class = checkContainer_t<T>>
struct tagCheckContainerT
{
	typedef _RetType type;
};
template <typename T>
struct tagCheckContainerT<T, void, void>
{
};

namespace NS_JSTL
{
	typedef size_t TD_PosType;

	template <typename T>
	using InitList_T = const initializer_list<T>&;

	template <typename T, typename RET>
	using CB_T_Ret = const function<RET(const T&v)>&;

	template <typename T>
	using CB_T_void = CB_T_Ret<T, void>;

	template <typename T>
	using CB_T_bool = CB_T_Ret<T, bool>;

	template <typename T>
	using CB_T_Pos = const function<bool(T, TD_PosType)>&;

	template <typename T>
	using __CB_Sort_T = const function<bool(T&lhs, T&rhs)>&;

	template<typename __DataType, template<typename...> class __BaseType> class JSArrayT;
	template <typename __DataType, template<typename...> class __BaseType = vector>
	using JSArray = JSArrayT<__DataType, __BaseType>;
}

#define __SuperType(T) typename __Super::T
#define __UsingSuperType(T) using T = __SuperType(T)

#endif // __Define_H