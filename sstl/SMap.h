
#ifndef __SMap_H
#define __SMap_H

namespace NS_SSTL
{
#define __SMapSuper SContainerT<__BaseType<__KeyType, __ValueType>, __KeyType>

	template<typename __KeyType, typename __ValueType, template<typename...> class __BaseType>
	class SMapT : public __SMapSuper
	{
	private:
		__UsingSuper(__SMapSuper)

		__UsingSuperType(__DataType)

		__UsingSuperType(__KeyConstRef)

		__UsingSuperType(__InitList_Key)

		using __ValueRef = __ValueType&;
		using __ValueConstRef = const __ValueType&;

		template <typename T>
		using __CB_ValueCR_T = const function<T(__ValueConstRef)>&;

		using __CB_KeyCR_ValueCR_bool = const function<bool(__KeyConstRef, __ValueConstRef)>&;

	private:
		template <class T = __ContainerType>
		class __MapOperator
		{
		public:
			__MapOperator(T& data)
				: m_data(data)
			{
			}

		private:
			T& m_data;

			using __ValueRef = decltype(m_data.begin()->second)&;
		public:
			template <typename CB, typename = checkCBBool_t<CB, __KeyConstRef, __ValueRef>>
			void forEach(const CB& cb)
			{
				for (auto& pr : m_data)
				{
					if (!cb(pr.first, pr.second))
					{
						break;
					}
				}
			}

			template <typename CB, typename = checkCBVoid_t<CB, __KeyConstRef, __ValueRef>, typename = void>
			void forEach(const CB& cb)
			{
				for (auto& pr : m_data)
				{
					cb(pr.first, pr.second);
				}
			}

			template <typename CB, typename = checkCBVoid_t<CB, __ValueRef>, typename = void, typename = void>
			void forEach(const CB& cb)
			{
				for (auto& pr : m_data)
				{
					cb(pr.second);
				}
			}

			template <typename CB, typename = checkCBBool_t<CB, __ValueRef>, typename = void, typename = void, typename = void>
			void forEach(const CB& cb)
			{
				for (auto& pr : m_data)
				{
					if (!cb(pr.second))
					{
						break;
					}
				}
			}
		};

		__MapOperator<> m_MapOperator = __MapOperator<>(m_data);
		__MapOperator<>& _getOperator()
		{
			return m_MapOperator;
		}
		__MapOperator<const __ContainerType>& _getOperator() const
		{
			return (__MapOperator<const __ContainerType>&)m_MapOperator;
		}

		const bool m_bMulti = is_same<__ContainerType, std::multimap<__KeyType, __ValueType>>::value
			|| is_same<__ContainerType, std::unordered_multimap<__KeyType, __ValueType>>::value;
		
	public:
		SMapT() = default;

		template <typename T>
		explicit SMapT(const T& keys, const function<__ValueType(__KeyConstRef)>& cb)
		{
			set(keys, cb);
		}

		explicit SMapT(__InitList_Key keys, const function<__ValueType(__KeyConstRef)>& cb)
		{
			set(keys, cb);
		}

		explicit SMapT(__ContainerType&& container)
		{
			__Super::swap(container);
		}

		SMapT(SMapT&& map)
		{
			__Super::swap(map);
		}

		SMapT(const SMapT& map)
			: __Super(map)
		{
		}

		explicit SMapT(__InitList initList)
			: __Super(initList)
		{
		}

		template<typename T, typename = checkContainer_t<T>>
		explicit SMapT(const T& container)
			: __Super(container)
		{
		}

		//template<typename T, typename = checkContainer_t<T>>
		//explicit SMapT(T& container)
		//	: __Super(container)
		//{
		//}

		SMapT& operator=(__ContainerType&& container)
		{
			__Super::swap(container);
			return *this;
		}

		SMapT& operator=(SMapT&& map)
		{
			__Super::swap(map);
			return *this;
		}

		SMapT& operator=(const SMapT& map)
		{
			__Super::assign(map);
			return *this;
		}

		SMapT& operator=(__InitList initList)
		{
			__Super::assign(initList);
			return *this;
		}

		template<typename T>
		SMapT& operator=(const T&t)
		{
			__Super::assign(t);
			return *this;
		}

		template<typename T>
		SMapT& operator=(T&t)
		{
			__Super::assign(t);
			return *this;
		}

		template<typename CB>
		void operator() (const CB& cb)
		{
			_getOperator().forEach(cb);
		}

		template<typename CB>
		void operator() (const CB& cb) const
		{
			_getOperator().forEach(cb);
		}

	public:
		template <typename CB>
		size_t get(__KeyConstRef key, const CB& cb)
		{
			return _find(key, [&](__ItrType& itr) {
				cb(itr->second);

				return true;
			});
		}

		template <typename CB>
		size_t get(__KeyConstRef key, const CB& cb) const
		{
			return _find(key, [&](__CItrType itr) {
				cb(itr->second);
				
				return true;
			});
		}
		
		template <typename CB, checkCBBool_t<CB, __ValueRef>>
		size_t get(__KeyConstRef key, const CB& cb)
		{
			return _find(key, [&](__ItrType& itr) {
				return cb(itr->second);
			});
		}

		template <typename CB, checkCBBool_t<CB, __ValueRef>>
		bool get(__KeyConstRef key, const CB& cb) const
		{
			return _find(key, [&](__CItrType& itr) {
				return cb(itr->second);
			});
		}

		__ValueType& insert(__KeyConstRef key, __ValueConstRef value= __ValueType())
		{
			return _insert(key, value);
		}

		__ValueType& set(__KeyConstRef key, __ValueConstRef value)
		{
			auto itr = m_data.find(key);
			if (itr != m_data.end())
			{
				return itr->second = value;
			}
			else
			{
				return _insert(key, value);
			}
		}
		
		template<typename T>
		void set(const T& container)
		{
			if (!__Super::checkIsSelf(container))
			{
				m_data.insert(container.begin(), container.end());
			}
		}

		void set(__InitList initList)
		{
			set<__InitList>(initList);
		}

		void set(__InitList_Key keys, const function<__ValueType(__KeyConstRef)>& cb)
		{
			set<__InitList_Key>(keys, cb);
		}

		template <typename T>
		void set(const T& container, const function<__ValueType(__KeyConstRef)>& cb)
		{
			for (auto& key : container)
			{
				this->set(key, cb(key));
			}
		}

	public:
		SArray<__KeyType> keys(__CB_KeyCR_ValueCR_bool cb = NULL) const
		{
			SArray<__KeyType> arr;
			for (auto& pr : m_data)
			{
				if (cb && !cb(pr.first, pr.second))
				{
					continue;
				}

				arr.add(pr.first);
			}

			return arr;
		}

		SArray<__ValueType> values(__CB_KeyCR_ValueCR_bool cb = NULL) const
		{
			SArray<__ValueType> arr;
			for (auto& pr : m_data)
			{
				if (cb && !cb(pr.first, pr.second))
				{
					continue;
				}

				arr.add(pr.second);
			}

			return arr;
		}

		template <typename T>
		SMapT<__KeyType, T, __BaseType> map(__CB_ValueCR_T<T> cb) const
		{
			SMapT<__KeyType, T, __BaseType> map;

			for (auto& pr : m_data)
			{
				map.set(pr.first, cb(pr.second));
			}
			
			return map;
		}

		template <typename CB, typename RET = decltype(declval<CB>()(__ValueType()))>
		SMapT<__KeyType, RET, __BaseType> map(const CB& cb) const
		{
			return map<RET>(cb);
		}

		SMapT filter(__CB_ValueCR_T<bool> cb) const
		{
			SMapT map;

			for (auto& pr : m_data)
			{
				if (cb(pr.second))
				{
					map.set(pr.first, pr.second);
				}
			}

			return map;
		}

		SMapT filter(__CB_KeyCR_ValueCR_bool cb) const
		{
			SMapT map;

			for (auto& pr : m_data)
			{
				if (cb(pr.first, pr.second))
				{
					map.set(pr.first, pr.second);
				}
			}

			return map;
		}

	private:
		virtual void _toString(stringstream& ss, const __DataType& pr) const override
		{
			tagSSTryLMove(ss) << '<' << pr.first << ", " << pr.second << '>';
		}

		void _add(const __DataType& pr) override
		{
			m_data.insert(pr);
		}

		size_t _del(__KeyConstRef key, CB_Del cb) override
		{
			auto itr = m_data.find(key);
			if (itr == m_data.end())
			{
				return 0;
			}

			size_t uRet = 0;
			do
			{
				E_DelConfirm eRet = cb(*itr);
				if (E_DelConfirm::DC_Abort == eRet)
				{
					break;
				}
				else if (E_DelConfirm::DC_No == eRet)
				{
					++itr;
				}
				else
				{
					itr = __Super::erase(itr);
					uRet++;

					if (E_DelConfirm::DC_YesAbort == eRet)
					{
						break;
					}
				}
			} while (m_bMulti && itr != m_data.end() && itr->first == key);

			return uRet;
		}

		size_t _find(__KeyConstRef key, CB_T_Ret<__ItrType&, bool> cb = NULL)
		{
			auto itr = m_data.find(key);
			if (itr == m_data.end())
			{
				return 0;
			}

			if (!cb)
			{
				return 1;
			}

			size_t uRet = 0;
			do
			{
				uRet++;

				auto lpValue = &itr->second;
				if (!cb(itr) || !m_bMulti)
				{
					break;
				}

				if (&itr->second == lpValue)
				{
					++itr;
				}
			} while (itr != m_data.end() && itr->first == key);

			return uRet;
		}

		bool _includes(__KeyConstRef key) const override
		{
			return m_data.find(key) != m_data.end();
		}

		template <typename _V>
		auto _insert(__KeyConstRef key, const _V& value)->decltype(m_data.insert({ key, value }).first->second)&
		{
			return m_data.insert({ key, value }).first->second;
		}

		template <typename _V>
		auto _insert(__KeyConstRef key, const _V& value)->decltype(m_data.insert({ key, value })->second)&
		{
			return m_data.insert({ key, value })->second;
		}
	};
}

#endif // __SMap_H
