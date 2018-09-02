
#ifndef __Util_H
#define __Util_H

#include <functional>

using namespace std;

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
	using __CB_Sort_T = const function<bool(const T&t1, const T&t2)>&;

	template <typename T>
	struct tagTrySort
	{
		tagTrySort(__CB_Sort_T<T> cb = NULL)
			: m_cb(cb)
		{
		}

		__CB_Sort_T<T> m_cb;

		bool operator()(const T&t1, const T&t2)const
		{
			if (m_cb)
			{
				return m_cb(t1, t2);
			}

			return _compare(t1, t2);
		}

		template <typename U>
		static auto _compare(const U&t1, const U&t2) -> decltype(declval<U>() < declval<U>())
		{
			return t1 < t2;
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

			if (!extract(cb, v))
			{
				return false;
			}

			if (sizeof...(others))
			{
				if (!extract(cb, others...))
				{
					return false;
				}
			}

			return true;
		}

		static bool extract(FN_ExtractCB cb, __DataType&v)
		{
			return cb(v);
		}
	};

	template <typename T, typename _RetType, typename _ITR = decltype(declval<T>().begin())>
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

#define __remove_constref(_type) std::remove_const<std::remove_reference<_type>::type>::type

	template <typename C, typename CB, typename T = __remove_constref(decltype(*(declval<C>().begin()))) >
	T reduce(const C& container, const CB& cb)
	{
		return reduce<T, C>(container, cb);
	}
}

#endif // __Util_H
