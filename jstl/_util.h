
#ifndef __Util_H
#define __Util_H

#include "_define.h"

namespace NS_JSTL
{
	template <typename T, typename U>
	struct decay_is_same :
		std::is_same<typename std::decay<T>::type, U>::type
	{};

	template <typename T>
	struct tagTryCompare
	{
		static bool compare(const T&t1, const T&t2)
		{
			return _compare(t1, t2);
		}

		template <typename U>
		static auto _compare(const U&t1, const U&t2) ->decltype(declval<U>() == declval<U>())
		{
			return t1 == t2;
		}

		static bool _compare(...)
		{
			return false;
		}
	};

	template <typename T>
	struct tagSortT
	{
		tagSortT(__CB_Sort_T<T> cb)
			: m_cb(cb)
		{
		}

		__CB_Sort_T<T> m_cb;

		bool operator()(const T&lhs, const T&rhs)const
		{
			return m_cb(lhs, rhs);
		}
	};

	template <typename T>
	struct tagTrySort
	{
		tagTrySort(__CB_Sort_T<T> cb = NULL)
			: m_cb(cb)
		{
		}

		__CB_Sort_T<T> m_cb;

		bool operator()(const T&lhs, const T&rhs)const
		{
			if (m_cb)
			{
				return m_cb(lhs, rhs);
			}

			return _compare(lhs, rhs);
		}

		template <typename U>
		static auto _compare(const U&lhs, const U&rhs) -> decltype(declval<U>() < declval<U>())
		{
			return lhs < rhs;
		}

		static bool _compare(...)
		{
			return false;
		}
	};

	template <typename T, typename U>
	struct tagTryLMove
	{
		static void lmove(T&t, const U&u)
		{
			_lmove(&t, &u);
		}

		template <typename X, typename Y>
		static auto _lmove(X*px, const Y*py) -> decltype(declval<X*>()->operator<<(declval<Y>()))
		{
			return *px << *py;
		}

		static bool _lmove(...)
		{
			return false;
		}

		enum { value = std::is_same<decltype(_lmove(declval<T*>(), declval<U*>())), T&>::value };
	};

	template <typename T, typename U = int>
	struct tagLMove
	{
		tagLMove(T&t)
			: m_t(t)
		{
		}

		T& m_t;

		tagLMove& operator<<(const U& u)
		{
			tagTryLMove<T, U>::lmove(m_t, u);
			return *this;;
		}

		template <typename V>
		tagLMove& operator<<(const V&v)
		{
			tagLMove<T, V>(m_t) << v;
			return *this;
		}
	};

	using tagSSTryLMove = tagLMove<stringstream>;

	template<typename __DataType>
	class tagDynamicArgsExtractor
	{
	public:
		using FN_ExtractCB = const function<bool(__DataType&v)>&;

		template<typename... args>
		static bool extract(FN_ExtractCB cb, __DataType&v, args&... others)
		{
			if (!cb)
			{
				return false;
			}

			return _extract(cb, true, v, others...);
		}

		template<typename... args>
		static bool extractReverse(FN_ExtractCB cb, __DataType&v, args&... others)
		{
			if (!cb)
			{
				return false;
			}

			return _extract(cb, false, v, others...);
		}

		template<typename... args>
		static bool _extract(FN_ExtractCB cb, bool bForward, __DataType&v, args&... others)
		{
			if (bForward)
			{
				if (!_extract(cb, bForward, v))
				{
					return false;
				}
			}

			if (sizeof...(others))
			{
				if (!_extract(cb, bForward, others...))
				{
					return false;
				}
			}

			if (!bForward)
			{
				if (!_extract(cb, bForward, v))
				{
					return false;
				}
			}

			return true;
		}

		static bool _extract(FN_ExtractCB cb, bool bForward, __DataType&v)
		{
			return cb(v);
		}
	};

	template <typename T, typename _RetType
		, typename _ITR = decltype(declval<T>().begin())>
	_RetType checkContainer();

	template <typename T, typename C>
	T reduce(const C& container, const function<T(const T& t1, const T& t2)>& cb)
	{
		if (!cb)
		{
			return T();
		}

		auto itr = container.begin();
		if (itr == container.end())
		{
			return T();
		}

		auto itrPrev = itr;
		itr++;
		if (itr == container.end())
		{
			return *itrPrev;
		}

		T ret = cb(*itrPrev, *itr);
		while (true)
		{
			itr++;
			if (itr == container.end())
			{
				break;
			}

			ret = cb(ret, *itr);
		}

		return ret;
	}

	template <typename C, typename CB, typename T
		= decltype(declval<CB>()(*(declval<C>().begin()), *(declval<C>().begin())))>
	T reduce(const C& container, const CB& cb)
	{
		return reduce<T, C>(container, cb);
	}
}

#endif // __Util_H
