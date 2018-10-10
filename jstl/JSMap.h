
#ifndef __JSMap_H
#define __JSMap_H

#include "Container.h"

#include <unordered_map>
#include <map>

#include "JSArray.h"
 
namespace NS_JSTL
{
	template<typename __DataType> class JSArray;

#define __JSMapSuper ContainerT<pair<__KeyType const, __ValueType>, __MapType<__KeyType, __ValueType>, __KeyType>

	template<typename __KeyType, typename __ValueType, template<typename...> typename __MapType> class JSMapT
		: public __JSMapSuper
	{
	private:
		using __Super = __JSMapSuper;

#ifndef _MSC_VER
	protected:
		__UsingSuperType(__ContainerType);

		__UsingSuperType(__DataType);

		__UsingSuperType(__InitList);
		__UsingSuperType(__InitList_Key);
		
		__UsingSuperType(__KeyConstRef);
#endif

	protected:
		using __ValueRef = __ValueType&;
		using __ValueConstRef = const __ValueType&;

		//template <typename T> using __CB_KeyCR_ValueR_T = const function<T(__KeyConstRef, __ValueRef)>&;
		//using __CB_KeyCR_ValueR_bool = __CB_KeyCR_ValueR_T<bool>;
		//using __CB_KeyCR_ValueR_void = __CB_KeyCR_ValueR_T<void>;

		template <typename T>
		using __CB_KeyCR_ValueCR_T = const function<T(__KeyConstRef, __ValueConstRef)>&;
		using __CB_KeyCR_ValueCR_bool = __CB_KeyCR_ValueCR_T<bool>;
		//using __CB_KeyCR_ValueCR_void = __CB_KeyCR_ValueCR_T<void>;

		using __CB_ValueR_KeyCR_void = const function<void(__ValueRef, __KeyConstRef)>&;
		using __CB_ValueCR_KeyCR_bool = const function<bool(__ValueConstRef, __KeyConstRef)>&;
		using __CB_ValueCR_KeyCR_void = const function<void(__ValueConstRef, __KeyConstRef)>&;

	protected:
		__ContainerType& _data()
		{
			return __Super::m_data;
		}

		const __ContainerType& _data() const
		{
			return __Super::m_data;
		}

	public:
		JSMapT()
		{
		}

		template <typename T>
		explicit JSMapT(const T& keys, const function<__ValueType(__KeyType)>& cb)
		{
			set(keys, cb);
		}

		explicit JSMapT(__InitList_Key keys, const function<__ValueType(__KeyType)>& cb)
		{
			set(keys, cb);
		}

		explicit JSMapT(const JSMapT& map)
			: __Super(map)
		{
		}

		JSMapT(JSMapT&& map)
		{
			__Super::swap(map);
		}

		JSMapT(__InitList initList)
			: __Super(initList)
		{
		}

		template<typename T>
		explicit JSMapT(const T& container)
			: __Super(container)
		{
		}

		JSMapT& operator=(const JSMapT& map)
		{
			__Super::assign(map);
			return *this;
		}

		JSMapT& operator=(JSMapT&& map)
		{
			__Super::swap(map);
			return *this;
		}

		JSMapT& operator=(__InitList initList)
		{
			__Super::assign(initList);
			return *this;
		}

		template<typename T>
		JSMapT& operator=(const T&t)
		{
			__Super::assign(t);
			return *this;
		}

	protected:
		TD_SizeType _add(const __DataType& pr) override
		{
			_data().insert(pr);

			return _data().size();
		}

		TD_SizeType _del(__KeyConstRef key) override
		{
			auto itr = _data().find(key);
			if (itr == _data().end())
			{
				return 0;
			}

			_data().erase(itr);

			return 1;
		}

		bool _includes(__KeyConstRef key) const override
		{
			return _data().find(key) != _data().end();
		}

		virtual void _toString(stringstream& ss, const __DataType& pr) const override
		{
			tagSSTryLMove(ss) << '<' << pr.first << ", " << pr.second << '>';
		}

	public:
		template <typename T>
		JSMapT& operator+= (const T& rhs)
		{
			__Super::add(rhs);
			return *this;
		}

		JSMapT& operator+= (__InitList rhs)
		{
			__Super::add(rhs);
			return *this;
		}

		template <typename T>
		JSMapT& operator-= (const T& keys)
		{
			__Super::del(keys);
			return *this;
		}

		JSMapT& operator-= (__InitList_Key keys)
		{
			__Super::del(keys);
			return *this;
		}

		template <typename T>
		JSMapT operator+ (const T& rhs)
		{
			JSMapT map(*this);
			map += rhs;
			return map;
		}

		template <typename T>
		JSMapT operator- (const T& rhs)
		{
			JSMapT map(*this);
			map -= rhs;
			return map;
		}

		bool get(__KeyConstRef key, __CB_ValueR_KeyCR_void cb)
		{
			auto itr = _data().find(key);
			if (itr == _data().end())
			{
				return false;
			}

			if (cb)
			{
				cb(itr->second, itr->first);
			}

			return true;
		}

		bool get(__KeyConstRef key, __CB_ValueCR_KeyCR_void cb) const
		{
			auto itr = _data().find(key);
			if (itr == _data().end())
			{
				return false;
			}

			if (cb)
			{
				cb(itr->second, itr->first);
			}

			return true;
		}

		bool findValue(__CB_ValueCR_KeyCR_bool cb) const
		{
			if (!cb)
			{
				return false;
			}

			for (auto& pr : _data())
			{
				if (cb(pr.second, pr.first))
				{
					return true;
				}
			}

			return false;
		}
		
		JSArray<__KeyType> keys(__CB_KeyCR_ValueCR_bool cb = NULL) const
		{
			JSArray<__KeyType> arr;
			for (auto& pr : _data())
			{
				if (cb)
				{
					if (!cb(pr.first, pr.second))
					{
						continue;
					}
				}

				arr.push(pr.first);
			}

			return arr;
		}

		JSArray<__ValueType> values(__CB_KeyCR_ValueCR_bool cb = NULL) const
		{
			JSArray<__ValueType> arr;
			for (auto& pr : _data())
			{
				if (cb)
				{
					if (!cb(pr.first, pr.second))
					{
						continue;
					}
				}

				arr.push(pr.second);
			}

			return arr;
		}

		JSMapT& set(__KeyConstRef key, __ValueConstRef value)
		{
			_data()[key] = value;
			return *this;
		}

		template<typename T>
		TD_SizeType set(const T& container)
		{
			if (!__Super::checkIsSelf(container))
			{
				_data().insert(container.begin(), container.end());
			}

			return _data().size();
		}

		TD_SizeType set(__InitList initList)
		{
			return set<__InitList>(initList);
		}

		template <typename T>
		TD_SizeType set(const T& container, const function<__ValueType(__KeyType)>& cb)
		{
			if (cb)
			{
				for (auto& key : container)
				{
					this->set(key, cb(key));
				}
			}

			return _data().size();
		}

		TD_SizeType set(__InitList_Key keys, const function<__ValueType(__KeyType)>& cb)
		{
			return set<__InitList_Key>(keys, cb);
		}

	public:
		template <typename T>
		JSMapT<__KeyType, T, __MapType> map(__CB_KeyCR_ValueCR_T<T> cb) const
		{
			JSMapT<__KeyType, T, __MapType> map;

			if (cb)
			{
				for (auto& pr : _data())
				{
					map.set(pr.first, cb(pr.first, pr.second));
				}
			}

			return map;
		}

		template <typename CB, typename RET = decltype(declval<CB>()(__KeyType(), __ValueType()))>
		JSMapT<__KeyType, RET, __MapType> map(const CB& cb) const
		{
			return map<RET>(cb);
		}

		JSMapT filter(__CB_KeyCR_ValueCR_bool cb) const
		{
			JSMapT map;

			if (cb)
			{
				for (auto& pr : _data())
				{
					if (cb(pr.first, pr.second))
					{
						map.set(pr.first, pr.second);
					}
				}
			}

			return map;
		}
	};

	template <typename __KeyType, typename __ValueType>
	using JSMap = JSMapT<__KeyType, __ValueType, map>;

	template <typename __KeyType, typename __ValueType>
	using JSUnsortMap = JSMapT<__KeyType, __ValueType, unordered_map>;
}

#endif // __JSMap_H
