
#ifndef __ContainerType_H
#define __ContainerType_H

#include <sstream>

#include <list>

#include "_check.h"

#include "_util.h"

namespace NS_SSTL
{
	template<typename __DataType__, typename __ContainerType__, typename __KeyType = __DataType__>
	class ContainerT
	{
	protected:
		using __DataType = __DataType__;

		using __ContainerType = __ContainerType__;

		__ContainerType m_data;

		using __ItrType = decltype(m_data.begin());
		using __ItrConstType = decltype(m_data.cbegin());

		using __InitList = InitList_T<__DataType>;
		using __InitList_Key = InitList_T<__KeyType>;

		using __DataRef = __DataType&;
		using __DataConstRef = const __DataType&;
		using __KeyConstRef = const __KeyType&;

		using __CB_Ref_void = CB_T_void<__DataRef>;
		using __CB_Ref_bool = CB_T_bool<__DataRef>;

		using __CB_ConstRef_void = CB_T_void<__DataConstRef>;
		using __CB_ConstRef_bool = CB_T_bool<__DataConstRef>;

		using __CB_Ref_DelConfirm = CB_T_Ret<__DataRef, E_DelConfirm>;

	public:
		ContainerT()
		{
		}

		template<typename... args>
		explicit ContainerT(__DataConstRef data, const args&... others)
		{
			add(data, others...);
		}

		explicit ContainerT(__ContainerType&& container)
		{
			m_data.swap(container);
		}

		ContainerT(ContainerT&& container)
		{
			swap(container);
		}

		ContainerT(const ContainerT& container)
			: m_data(container.begin(), container.end())
		{
		}

		explicit ContainerT(__InitList initList)
			: m_data(initList.begin(), initList.end())
		{
		}

		template<typename T, typename = checkContainer_t<T>>
		explicit ContainerT(const T& container)
			: m_data(CItrVisitor<const T>(container).begin(), CItrVisitor<const T>(container).end())
		{
		}

		template<typename T, typename = checkContainer_t<T>>
		explicit ContainerT(T& container)
			: m_data(container.begin(), container.end())
		{
		}

		ContainerT& operator=(ContainerT&& container)
		{
			swap(container);
			return *this;
		}

		ContainerT& operator=(const ContainerT& container)
		{
			assign(container);
			return *this;
		}

		ContainerT& operator=(__InitList initList)
		{
			assign(initList);
			return *this;
		}

		template <typename T>
		ContainerT& operator=(const T&t)
		{
			assign(t);
			return *this;
		}

		template <typename T>
		ContainerT& operator=(T&t)
		{
			assign(t);
			return *this;
		}

	public:
		ContainerT& swap(ContainerT& container)
		{
			_swap(container.m_data);
			return *this;
		}

		ContainerT& swap(__ContainerType& container)
		{
			_swap(container);
			return *this;
		}

		template<typename... args>
		ContainerT& assign(__DataConstRef data, const args&... others)
		{
			clear();

			add(data, others...);

			return *this;
		}

		ContainerT assign(__ContainerType&& container)
		{
			m_data.swap(container);
		}

		ContainerT& assign(ContainerT&& container)
		{
			return swap(container);
		}

		ContainerT& assign(__InitList initList)
		{
			return assign<__InitList>(initList);
		}

		template<typename T, typename = checkContainer_t<T>>
		ContainerT& assign(const T& container)
		{
			if (checkIsSelf(container))
			{
				return *this;
			}

			//m_data = __ContainerType(container.begin(), container.end());

			//m_data.clear();
			//for (auto&data : container)
			//{
			//	_add(data);
			//}

			m_data.clear();

			CItrVisitor<const T> Visitor(container);
			new (&m_data) __ContainerType(Visitor.begin(), Visitor.end());

			return *this;
		}

		void add(__DataConstRef data)
		{
			_add(data);
		}

		template<typename... args>
		void add(__DataConstRef data, const args&... others)
		{
			(void)extractDataTypeArgs([&](__DataConstRef data) {
				_add(data);
				return true;
			}, data, others...);
		}

		void add(__InitList initList)
		{
			add<__InitList>(initList);
		}

		template<typename T, typename = checkContainer_t<T>>
		void add(const T& container)
		{
			if (checkIsSelf(container))
			{
				return;
			}

			for (auto&data : container)
			{
				_add(data);
			}
		}

	public:
		operator bool() const
		{
			return !m_data.empty();
		}

		void clear()
		{
			m_data.clear();
		}

		size_t size() const
		{
			return m_data.size();
		}

		bool getFront(__CB_Ref_void cb)
		{
			if (m_data.empty())
			{
				return false;
			}

			cb(*m_data.begin());

			return true;
		}

		bool getFront(__CB_ConstRef_void cb) const
		{
			if (m_data.empty())
			{
				return false;
			}

			cb(*m_data.begin());

			return true;
		}

		bool getFront(__DataRef data) const
		{
			if (m_data.empty())
			{
				return false;
			}

			data = *m_data.begin();

			return true;
		}

		bool getBack(__CB_Ref_void cb)
		{
			if (m_data.empty())
			{
				return false;
			}

			cb(*m_data.rbegin());
			
			return true;
		}

		bool getBack(__CB_ConstRef_void cb) const
		{
			if (m_data.empty())
			{
				return false;
			}

			cb(*m_data.rbegin());
		}

		bool getBack(__DataRef data) const
		{
			if (m_data.empty())
			{
				return false;
			}

			data = *m_data.rbegin();

			return true;
		}

		bool popFront(__CB_ConstRef_void cb = NULL)
		{
			auto itr = m_data.begin();
			if (itr == m_data.end())
			{
				return false;
			}

			if (cb)
			{
				cb(*itr);
			}

			m_data.erase(itr);

			return true;
		}

		bool popFront(__DataRef data)
		{
			auto itr = m_data.begin();
			if (itr == m_data.end())
			{
				return false;
			}

			data = *itr;
			m_data.erase(itr);

			return true;
		}

		bool includes(__KeyConstRef data) const
		{
			return _find(data) != m_data.end();
		}

		template<typename... args>
		bool includes(__KeyConstRef data, const args&... others) const
		{
			return tagDynamicArgsExtractor<__KeyConstRef>::extract([&](__KeyConstRef data) {
				return includes(data);
			}, data, others...);
		}

		template<typename T, typename = checkContainer_t<T>>
		bool includes(const T& container) const
		{
			if (checkIsSelf(container))
			{
				return true;
			}

			for (auto&data : container)
			{
				if (!includes(data))
				{
					return false;
				}
			}

			return true;
		}

		bool includes(__InitList_Key initList) const
		{
			return includes<__InitList_Key>(initList);
		}

		template<typename... args>
		size_t del(__KeyConstRef key, const args&... others)
		{
			size_t uRet = 0;

			(void)extractKeyTypeArgs([&](__KeyConstRef key) {
				uRet += _del(key);
				return true;
			}, key, others...);

			return uRet;
		}

		template<typename T, typename = checkContainer_t<T>>
		size_t del(const T& container)
		{
			if (checkIsSelf(container))
			{
				size_t uRet = this->size();
				this->clear();
				return uRet;
			}

			size_t uRet = 0;

			CItrVisitor<const T> Visitor(container);
			auto itrEnd = Visitor.end();
			for (auto itr = Visitor.begin(); itr != itrEnd; itr++)
			{
				if (m_data.empty())
				{
					break;
				}

				uRet += _del(*itr);
			}

			return uRet;
		}

		size_t del(__InitList_Key keys)
		{
			return del<__InitList_Key>(keys);
		}

		size_t del_if(__CB_Ref_DelConfirm cb)
		{
			size_t uRet = 0;

			for (auto itr = m_data.begin(); itr != m_data.end();)
			{
				auto eRet = cb(*itr);
				
				if (E_DelConfirm::DC_No == eRet)
				{
					itr++;
				}
				else
				{
					if (E_DelConfirm::DC_Yes == eRet || E_DelConfirm::DC_YesAbort == eRet)
					{
						itr = m_data.erase(itr);
						uRet++;
					}

					if (E_DelConfirm::DC_Abort == eRet || E_DelConfirm::DC_YesAbort == eRet)
					{
						break;
					}
				}
			}

			return uRet;
		}

		string toString(const string& strSplitor = ",") const
		{
			stringstream ss;
			ss << '[';
			for (auto itr = m_data.begin(); itr != m_data.end(); itr++)
			{
				if (itr != m_data.begin())
				{
					ss << strSplitor.c_str();
				}

				_toString(ss, *itr);
			}

			ss << ']';
			return ss.str();
		}

		bool every(__CB_ConstRef_bool cb) const
		{
			for (auto&data : m_data)
			{
				if (!cb(data))
				{
					return false;
				}
			}

			return true;
		}

		bool some(__CB_ConstRef_bool cb) const
		{
			for (auto&data : m_data)
			{
				if (cb(data))
				{
					return true;
				}
			}

			return false;
		}

		__DataType reduce(const function<__DataType(__DataConstRef, __DataConstRef)>& cb) const
		{
			return NS_SSTL::reduce<__DataType, __ContainerType >(m_data, cb);
		}

	public:
		__ItrType begin()
		{
			return m_data.begin();
		}
		__ItrType end()
		{
			return m_data.end();
		}

		__ItrConstType begin() const
		{
			return m_data.cbegin();
		}
		__ItrConstType end() const
		{
			return m_data.cend();
		}

		template <typename T, typename = checkIter_t<T>>
		T erase(const T& itr)
		{
			return m_data.erase(itr);
		}

	private:
		virtual __ItrConstType _find(__KeyConstRef key) const
		{
			return m_data.end();
		}
		
		virtual void _add(__DataConstRef data)
		{
			m_data.insert(m_data.end(), data);
		}

		virtual size_t _del(__KeyConstRef key)
		{
			return 0;
		}

		virtual void _toString(stringstream& ss, __DataConstRef data) const
		{
			tagSSTryLMove(ss) << data;
		}

	protected:
		template<typename T>
		bool checkIsSelf(const T& container) const
		{
			if (is_same<T, __ContainerType>::value)
			{
				return (__ContainerType*)&container == &m_data;
			}

			return (ContainerT*)&container == this;
		}

		template<typename... args>
		static bool extractDataTypeArgs(__CB_Ref_bool cb, __DataRef data, args&... others)
		{
			return tagDynamicArgsExtractor<__DataType>::extract([&](__DataRef data) {
				return cb(data);
			}, data, others...);
		}

		template<typename... args>
		static bool extractDataTypeArgs(__CB_ConstRef_bool cb, __DataConstRef data, const args&... others)
		{
			return tagDynamicArgsExtractor<const __DataType>::extract([&](__DataConstRef data) {
				return cb(data);
			}, data, others...);
		}

		template<typename... args>
		static void extractDataTypeArgs(vector<__DataType>& vecArgs, __DataConstRef data, const args&... others)
		{
			extractDataTypeArgs([&](__DataConstRef data) {
				vecArgs.push_back(data);
				return true;
			}, data, others...);
		}

		template<typename... args>
		static bool extractKeyTypeArgs(CB_T_bool<__KeyConstRef> cb, __KeyConstRef key, const args&... others)
		{
			return tagDynamicArgsExtractor<const __KeyType>::extract([&](__KeyConstRef key) {
				return cb(key);
			}, key, others...);
		}

		template<typename... args>
		static void extractKeyTypeArgs(vector<__KeyType>& vecArgs, __KeyConstRef key, const args&... others)
		{
			extractKeyTypeArgs([&](__KeyConstRef key) {
				vecArgs.push_back(key);
				return true;
			}, key, others...);
		}

	private:
		void _swap(__ContainerType& container)
		{
			if (&container != &m_data)
			{
				m_data.swap(container);
			}
		}

	private:
		template <typename DATA> class __ContainerOperatorT
		{
		public:
			__ContainerOperatorT(DATA& data)
				: m_data(data)
			{
			}

		private:
			DATA& m_data;

		public:
		};

		using __ContainerOperator = __ContainerOperatorT<__ContainerType>;
		__ContainerOperator m_ContainerOperator = __ContainerOperator(m_data);
		__ContainerOperator& _getOperator()
		{
			return m_ContainerOperator;
		}

		using __ContainerReader = __ContainerOperatorT<const __ContainerType>;
		__ContainerReader m_ContainerReader = __ContainerReader(m_data);
		__ContainerReader& _getOperator() const
		{
			return (__ContainerReader&)m_ContainerReader;
		}
	};

	template <template<typename...> typename __BaseType, class __DataType>
	using Container = ContainerT<__DataType, __BaseType<__DataType>>;

	template <class __DataType>
	using ListT = Container<list, __DataType>;
}

#endif //__ContainerType_H