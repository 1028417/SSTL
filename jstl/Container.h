
#ifndef __ContainerType_H
#define __ContainerType_H

#include "util.h"

namespace NS_JSTL
{
	typedef size_t TD_PosType;
	typedef size_t TD_SizeType;

	template <typename T>
	using __InitList = const initializer_list<T>&;

	template <typename T, typename __RET>
	using __CB_T_RET = const function<__RET(const T&v)>&;

	template <typename T>
	using __CB_T_void = __CB_T_RET<T, void>;

	template <typename T>
	using __CB_T_bool = __CB_T_RET<T, bool>;

	template <typename T>
	using __CB_Data_Pos = const function<bool(T, TD_PosType)>&;

	template<typename __DataType> class JSArray;

	template<typename __DataType, typename __ContainerType, typename __KeyType = __DataType>
	class ContainerT
	{
	private:
		using __Data_InitList = __InitList<__DataType>;
		using __Key_InitList = __InitList<__KeyType>;

		using __ConstDataRef = const __DataType&;
		using __ConstKeyRef = const __KeyType&;

		using __CB_void = __CB_T_void<__ConstDataRef>;
		using __CB_bool = __CB_T_bool<__ConstDataRef>;

		using __CB_ConstDataRef_Pos = __CB_Data_Pos<__ConstDataRef>;

	protected:
		ContainerT()
		{
		}

		template<typename T>
		explicit ContainerT(const T& container)
			: m_data(container.begin(), container.end())
		{
		}

	protected:
		__ContainerType m_data;

	protected:
		template<typename... args>
		bool extractDataTypeArgs(__CB_bool cb, __ConstDataRef data, const args&... others)
		{
			return tagDynamicArgsExtractor<const __DataType>::extract([&](__ConstDataRef data) {
				return cb(data);
			}, data, others...);
		}

		template<typename... args>
		void extractDataTypeArgs(vector<__DataType>& vecArgs, __ConstDataRef data, const args&... others)
		{
			extractDataTypeArgs([&](__ConstDataRef data) {
				vecArgs.push_back(data);
				return true;
			}, data, others...);
		}
		
		template<typename... args>
		bool extractKeyTypeArgs(__CB_T_bool<__ConstKeyRef> cb, __ConstKeyRef key, const args&... others)
		{
			return tagDynamicArgsExtractor<const __KeyType>::extract([&](__ConstKeyRef key) {
				return cb(key);
			}, key, others...);
		}

		template<typename... args>
		void extractKeyTypeArgs(vector<__KeyType>& vecArgs, __ConstKeyRef key, const args&... others)
		{
			extractKeyTypeArgs([&](__ConstKeyRef key) {
				vecArgs.push_back(key);
				return true;
			}, key, others...);
		}

		template<typename T>
		bool checkIsSelf(const T& container)
		{
			return ((void*)&container == (void*)this) || ((void*)&container == (void*)&m_data);
		}

	public:
		template<typename... args>
		ContainerT& assign(__ConstDataRef data, const args&... others)
		{
			clear();

			add(data, others...);

			return *this;
		}

		template<typename T>
		ContainerT& assign(const T& container)
		{
			if (checkIsSelf(container))
			{
				return *this;
			}

			m_data = __ContainerType(container.begin(), container.end());

			return *this;
		}

		ContainerT& assign(__Data_InitList initList)
		{
			return assign<__Data_InitList>(initList);
		}

		ContainerT& swap(ContainerT& container)
		{
			if ((void*)this != (void*)&container)
			{
				m_data.swap(container.m_data);
			}

			return *this;
		}

	public:
		TD_SizeType size() const
		{
			return m_data.size();
		}

		TD_SizeType length() const
		{
			return m_data.size();
		}

		decltype(m_data.begin()) begin()
		{
			return m_data.begin();
		}
		decltype(m_data.end()) end()
		{
			return m_data.end();
		}

		decltype(m_data.cbegin()) begin() const
		{
			return m_data.cbegin();
		}
		decltype(m_data.cbegin()) end() const
		{
			return m_data.cend();
		}

		bool front(__CB_void fn=NULL) const
		{
			auto itr = m_data.begin();
			if (itr == m_data.end())
			{
				return false;
			}

			if (fn)
			{
				fn(*itr);
			}

			return true;
		}

		bool back(__CB_void fn)
		{
			auto itr = m_data.rbegin();
			if (itr == m_data.rend())
			{
				return false;
			}

			if (fn)
			{
				fn(*itr);
			}

			return true;
		}

		bool popFront(__CB_void fn=NULL)
		{
			auto itr = m_data.begin();
			if (itr == m_data.end())
			{
				return false;
			}

			if (fn)
			{
				fn(*itr);
			}
			m_data.erase(itr);

			return true;
		}

		virtual bool _includes(__ConstKeyRef key) const = 0;

		template<typename... args>
		bool includes(__ConstDataRef data, const args&... others)
		{
			bool bRet = true;

			(void)extractDataTypeArgs([&](__ConstDataRef data) {
				return bRet = _includes(data);
			}, data, others...);

			return bRet;
		}

		template<typename T>
		decltype(checkContainer<T, bool>()) includes(const T& container) const
		{
			if (checkIsSelf(container))
			{
				return true;
			}

			for (auto&data : container)
			{
				if (!_includes(data))
				{
					return false;
				}
			}

			return true;
		}

		bool includes(__Data_InitList initList) const
		{
			return includes<__Data_InitList>(initList);
		}

		template<typename... args>
		vector<__KeyType> getInner(__ConstKeyRef key, const args&... others)
		{
			vector<__KeyType> vec;
			extractKeyTypeArgs(vec, key, others...);
			return getInner(vec);
		}

		template <typename T>
		T getInner(const T& container) const
		{
			T ret(container);

			if (!m_data.empty())
			{
				for (auto itr = ret.begin(); itr != ret.end(); )
				{
					if (!_includes(*itr))
					{
						itr = ret.erase(itr);
					}
					else
					{
						itr++;
					}
				}
			}

			return ret;
		}

		vector<__KeyType> getInner(__Key_InitList initList) const
		{
			vector<__KeyType> vec(initList);
			return getInner(vec);
		}

		template<typename... args>
		vector<__KeyType> getOuter(__ConstKeyRef key, const args&... others)
		{
			vector<__KeyType> vec;
			extractKeyTypeArgs(vec, key, others...);
			return getOuter(vec);
		}

		template <typename T>
		T getOuter(const T& container) const
		{
			T ret(container);

			if (!m_data.empty())
			{
				for (auto itr = ret.begin(); itr != ret.end(); )
				{
					if (_includes(*itr))
					{
						itr = ret.erase(itr);
					}
					else
					{
						itr++;
					}
				}
			}

			return ret;
		}

		vector<__KeyType> getOuter(__Key_InitList initList) const
		{
			vector<__KeyType> vec(initList);
			return getOuter(vec);
		}

		template <typename T>
		T erase(T& itr)
		{
			return itr = m_data.erase(itr);
		}

		template<typename... args>
		TD_SizeType del(__ConstKeyRef key, const args&... others)
		{
			TD_SizeType uRet = 0;

			(void)extractKeyTypeArgs([&](__ConstKeyRef key) {
				uRet += _del(key);
				return true;
			}, key, others...);

			return uRet;
		}

		template <typename T>
		decltype(checkContainer<T, TD_SizeType>()) del(const T& container)
		{
			if (checkIsSelf(container))
			{
				TD_SizeType uRet = this->size();
				this->clear();
				return uRet;
			}

			TD_SizeType uRet = 0;

			for (auto&data : container)
			{
				if (m_data.empty())
				{
					break;
				}

				uRet += _del(data);
			}

			return uRet;
		}

		TD_SizeType del(__Key_InitList initList)
		{
			return del<__Key_InitList>(initList);
		}

		TD_SizeType del(__CB_bool fn)
		{
			TD_SizeType uRet = 0;

			for (auto itr = m_data.begin(); itr != m_data.end();)
			{
				if (fn(*itr))
				{
					itr = m_data.erase(itr);
					uRet++;
				}
				else
				{
					itr++;
				}
			}

			return uRet;
		}

		void clear()
		{
			m_data.clear();
		}

		string toString(const string& strSplitor = ",")
		{
			stringstream ss;
			ss << '[';
			for (auto itr = m_data.begin(); itr != m_data.end(); itr++)
			{
				if (itr != m_data.begin())
				{
					ss << strSplitor.c_str();
				}

				_tostring(ss, *itr);
			}

			ss << ']';
			return ss.str();
		}

		//const __ContainerType& data() const
		//{
		//	return m_data;
		//}
		//__ContainerType& data()
		//{
		//	return m_data;
		//}

	protected:
		virtual TD_SizeType _add(__ConstDataRef data) = 0;

		template<typename... args>
		TD_SizeType add(__ConstDataRef data, const args&... others)
		{
			(void)extractDataTypeArgs([&](__ConstDataRef data) {
				_add(data);
				return true;
			}, data, others...);

			return size();
		}

		template<typename T>
		decltype(checkContainer<T, TD_SizeType>()) add(const T& container)
		{
			if (checkIsSelf(container))
			{
				return size();
			}

			for (auto&data : container)
			{
				_add(data);
			}

			return size();
		}

		TD_SizeType add(__Data_InitList initList)
		{
			return add<__Data_InitList>(initList);
		}

		virtual TD_SizeType _del(__ConstKeyRef keyk) = 0;

		virtual void _tostring(stringstream& ss, __ConstDataRef data) const
		{
			tagSSTryLMove(ss) << data;
		}

	public:
		void forEach(__CB_ConstDataRef_Pos fn) const
		{
			if (!fn)
			{
				return;
			}

			TD_PosType pos = 0;
			for (auto&data : m_data)
			{
				if (!fn(data, pos))
				{
					break;
				}

				pos++;
			}
		}

		bool find(__CB_ConstDataRef_Pos fn) const
		{
			if (!fn)
			{
				return false;
			}

			bool bRet = false;
			forEach([&](__ConstDataRef data, TD_PosType pos) {
				if (fn(data, pos))
				{
					bRet = true;
					return false;
				}

				return true;
			});

			return bRet;
		}

		bool every(__CB_bool fn)
		{
			for (auto&data : m_data)
			{
				if (!fn(data))
				{
					return false;
				}
			}

			return true;
		}

		bool some(__CB_bool fn)
		{
			for (auto&data : m_data)
			{
				if (fn(data))
				{
					return true;
				}
			}

			return false;
		}

		template<typename T, typename __Function>
		T reduce(const T& stat, __Function fn)
		{
			T ret = stat;
			for (auto&data : m_data)
			{
				ret = (T)fn(ret, data);
			}

			return ret;
		}
	};
}

#endif //__ContainerType_H
