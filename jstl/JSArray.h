
#ifndef __JSArray_H
#define __JSArray_H

#include "Container.h"

#include <deque>
#include <map>

#include <memory.h>

namespace NS_JSTL
{
	template<typename __KeyType, typename __ValueType, template<typename...> class __Container = map> class JSMap;

	template<typename __DataType
		, typename __SueperClass = ContainerT<__DataType, deque<__DataType>>
	> class JSArray : public  __SueperClass
	{
	private:
		using __JSArray_InitList = __InitList<__DataType>;

		using __JSArray_CB = __FN_CB<__DataType>;
		using __JSArray_Cond = __FN_Cond<__DataType>;

	public:
		JSArray(TD_SizeType size, const __DataType&v)
		{
			__SueperClass::m_data.assign(size, v);
		}

		JSArray()
		{
		}

		explicit JSArray(const JSArray& arr)
			: __SueperClass(arr)
		{
		}

		JSArray(JSArray&& arr)
		{
			__SueperClass::swap(arr);
		}

		explicit JSArray(const __JSArray_InitList& initList)
			: __SueperClass(initList)
		{
		}

		template<typename T>
		explicit JSArray(const T& container)
			: __SueperClass(container)
		{
		}

		JSArray& operator=(const JSArray& arr)
		{
			__SueperClass::assign(arr);
			return *this;
		}

		JSArray& operator=(JSArray&& arr)
		{
			__SueperClass::swap(arr);
			return *this;
		}

		JSArray& operator=(const __JSArray_InitList& initList)
		{
			__SueperClass::assign(initList);
			return *this;
		}

		template <typename T>
		JSArray& operator=(const T& container)
		{
			__SueperClass::assign(container);
			return *this;
		}

	private:
		virtual TD_SizeType _add(const __DataType&v) override
		{
			__SueperClass::m_data.push_back(v);
			return __SueperClass::m_data.size();
		}

	public:
		__DataType& operator[](TD_PosType pos)
		{
			return __SueperClass::m_data[pos];
		}
		const __DataType& operator[](TD_PosType pos) const
		{
			return __SueperClass::m_data[pos];
		}

		bool has(const __DataType&v) const override
		{
			return indexOf(v)>=0;
		}

		int indexOf(const __DataType&v) const
		{
			int uIdx = 0;
			for (auto& data : __SueperClass::m_data)
			{				
				if (tagTryCompare<__DataType>().compare(data, v))
				{
					return uIdx;
				}
				uIdx++;
			}

			return -1;
		}

		int lastIndexOf(const __DataType&v) const
		{
			int uIdx = 1;
			for (auto& data : __SueperClass::m_data)
			{
				if (tagTryCompare<__DataType>().compare(data, v))
				{
					return __SueperClass::m_data.size()-uIdx;
				}
				uIdx++;
			}

			return -1;
		}

		bool get(TD_PosType pos, __JSArray_CB fn=NULL) const
		{
			if (pos >= __SueperClass::m_data.size())
			{
				return false;
			}

			if (fn)
			{
				fn(__SueperClass::m_data[pos]);
			}

			return true;
		}

		template <typename __Function>
		int find(__Function fn, TD_PosType stratPos = 0) const
		{
			for (TD_PosType pos = stratPos; pos < __SueperClass::m_data.size(); pos++)
			{
				if (fn(pos, __SueperClass::m_data[pos]))
				{
					return pos;
				}

				pos++;
			}

			return -1;
		}

		TD_SizeType push(const __DataType&v)
		{
			return _add(v);
		}

		template<typename T>
		TD_SizeType push(const T& container)
		{
			__SueperClass::m_data.insert(__SueperClass::m_data.end(), container.begin(), container.end());

			return __SueperClass::m_data.size();
		}

		TD_SizeType push(const __JSArray_InitList& initList)
		{
			return push<__JSArray_InitList>(initList);
		}

		template<typename T>
		JSArray concat(const T& container)
		{
			JSArray ret(*this);
			ret.push(container);
			return ret;
		}

		JSArray concat(const __JSArray_InitList& initList)
		{
			JSArray ret(*this);
			ret.push(initList);
			return ret;
		}

		JSArray sub(TD_PosType pos, TD_SizeType count) const
		{
			JSArray ret;

			for (TD_PosType idx = 0; idx < count; idx++)
			{
				if (!this->get(pos + idx, [&](const __DataType&v) {
					ret.push(v);
				}))
				{
					break;
				}
			}

			return ret;
		}

		bool pop(__JSArray_CB fn=NULL)
		{
			if (__SueperClass::m_data.empty())
			{
				return false;
			}

			if (fn)
			{
				fn(__SueperClass::m_data.back());
			}

			__SueperClass::m_data.pop_back();

			return true;
		}

		bool shift(__JSArray_CB fn=NULL)
		{
			return pop_begin(fn);
		}

		template<typename T>
		TD_SizeType unshift(const T& container)
		{
			__SueperClass::m_data.insert(__SueperClass::m_data.begin(), container.begin(), container.end());

			return __SueperClass::m_data.size();
		}

		TD_SizeType unshift(const __JSArray_InitList& initList)
		{
			return unshift<__JSArray_InitList>(initList);
		}

		template<typename T>
		JSArray& splice(TD_PosType pos, TD_SizeType nRemove, const T& container)
		{
			auto itr = __SueperClass::m_data.end();
			if (pos < __SueperClass::m_data.size())
			{
				itr = __SueperClass::m_data.begin() + pos;
			}

			while (itr != __SueperClass::m_data.end() && nRemove)
			{
				itr = __SueperClass::m_data.erase(itr);
				nRemove--;
			}

			__SueperClass::m_data.insert(itr, container.begin(), container.end());

			return *this;
		}

		JSArray& splice(TD_PosType pos, TD_SizeType nRemove, const __JSArray_InitList& initList)
		{
			return splice<__JSArray_InitList>(pos, nRemove, initList);
		}

		TD_SizeType remove(const __DataType& v) override
		{
			TD_SizeType uRet = 0;

			auto itr = __SueperClass::m_data.begin();
			while (itr != __SueperClass::m_data.end())
			{
				if (tagTryCompare<__DataType>().compare(*itr, v))
				{
					itr = __SueperClass::m_data.erase(itr);
					uRet++;
				}
				else
				{
					itr++;
				}
			}

			return uRet;
		}

		JSArray& sort(__FN_Sort<__DataType> fn = NULL)
		{
			std::sort(__SueperClass::m_data.begin(), __SueperClass::m_data.end(), tagTrySort<__DataType>(fn));

			return *this;
		}

		JSArray& reverse()
		{
			std::reverse(__SueperClass::m_data.begin(), __SueperClass::m_data.end());

			return *this;
		}

	public:
		template <typename __Function>
		auto map(__Function fn) const ->JSArray<decltype(fn(__DataType()))>
		{
			JSArray<decltype(fn(__DataType()))> ret;

			for (auto& v : __SueperClass::m_data)
			{
				ret.push(fn(v));
			}

			return ret;
		}

		JSArray filter(__JSArray_Cond fn) const
		{
			JSArray ret;

			for (auto& v : __SueperClass::m_data)
			{
				if (fn(v))
				{
					ret.push(v);
				}
			}

			return ret;
		}

		JSMap<__DataType, TD_SizeType> itemSum() const
		{
			JSMap<__DataType, TD_SizeType> mapItemSum;

			for (auto& v : __SueperClass::m_data)
			{
				mapItemSum[v]++;
			}

			return mapItemSum;
		}

		void sum(JSMap<__DataType, TD_SizeType>& mapItemSum, JSMap<TD_SizeType, JSArray<__DataType>>& mapSumItem) const
		{
			mapItemSum = itemSum();

			for (auto& pr : mapItemSum)
			{
				mapSumItem[pr.second].push(pr.first);
			}
		}

		JSMap<TD_SizeType, JSArray<__DataType>> sumItem() const
		{
			JSMap<__DataType, TD_SizeType> mapItemSum;
			JSMap<TD_SizeType, JSArray<__DataType>> mapSumItem;
			sum(mapItemSum, mapSumItem);
			return mapSumItem;
		}
	};
}

#endif // __JSArray_H
