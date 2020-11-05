#pragma once

#include <exception>
#include <vector>
#include <array>
#include <string>
#include <functional>
#include <sstream>

namespace dnc
{
	/*
		class SequentialMap
	*/
	template<typename KeyT, typename ValT>
	class SequentialMap : public std::vector<std::pair<KeyT, ValT>>
	{
	public:
		typedef std::pair<KeyT, ValT> pair_t;
		typedef std::vector<pair_t> base_t;
		
		SequentialMap();
		SequentialMap(std::initializer_list<pair_t> init_list);
		SequentialMap(const SequentialMap<KeyT, ValT>& seq_map);
		SequentialMap(SequentialMap<KeyT, ValT>&& seq_map);
		virtual ~SequentialMap();

		ValT& operator[](const KeyT& key);
		const ValT& at(const KeyT& key) const;

		typename base_t::iterator find(const KeyT& key);
		typename base_t::const_iterator find(const KeyT& key) const;

		typename base_t::iterator erase(const KeyT& key);

		void pushBack(const KeyT& key, const ValT& value);
	};

	template<typename KeyT, typename ValT>
	SequentialMap<KeyT, ValT>::SequentialMap()
	{}

	template<typename KeyT, typename ValT>
	SequentialMap<KeyT, ValT>::SequentialMap(std::initializer_list<pair_t> init_list) :
		base_t(init_list)
	{}

	template<typename KeyT, typename ValT>
	SequentialMap<KeyT, ValT>::SequentialMap(const SequentialMap<KeyT, ValT>& seq_map) :
		base_t(seq_map)
	{}

	template<typename KeyT, typename ValT>
	SequentialMap<KeyT, ValT>::SequentialMap(SequentialMap<KeyT, ValT>&& seq_map) :
		base_t(std::move(seq_map))
	{}

	template<typename KeyT, typename ValT>
	SequentialMap<KeyT, ValT>::~SequentialMap()
	{}

	template<typename KeyT, typename ValT>
	ValT& SequentialMap<KeyT, ValT>::operator[](const KeyT& key)
	{
		for(auto it = base_t::begin(); it != base_t::end(); ++it)
		{
			if(it->first == key)
			{
				return it->second;
			}
		}

		auto& elem = base_t::insert(base_t::end(), std::make_pair(key, std::move(ValT())))->second;

		return elem;
	}

	template<typename KeyT, typename ValT>
	const ValT& SequentialMap<KeyT, ValT>::at(const KeyT& key) const
	{
		for(auto it = base_t::begin(); it != base_t::end(); ++it)
		{
			if(it->first == key)
			{
				return it->second;
			}
		}
		
		throw std::logic_error("");
	}

	template<typename KeyT, typename ValT>
	typename SequentialMap<KeyT, ValT>::base_t::iterator SequentialMap<KeyT, ValT>::find(const KeyT& key)
	{
		for(auto it = base_t::begin(); it != base_t::end(); ++it)
		{
			if(it->first == key)
			{
				return it;
			}
		}

		return base_t::end();
	}

	template<typename KeyT, typename ValT>
	typename SequentialMap<KeyT, ValT>::base_t::const_iterator SequentialMap<KeyT, ValT>::find(const KeyT& key) const
	{
		for(auto it = base_t::begin(); it != base_t::end(); ++it)
		{
			if(it->first == key)
			{
				return it;
			}
		}

		return base_t::end();
	}

	template<typename KeyT, typename ValT>
	typename SequentialMap<KeyT, ValT>::base_t::iterator SequentialMap<KeyT, ValT>::erase(const KeyT& key)
	{
		auto position = find(key);

		return base_t::erase(position);
	}

	template<typename KeyT, typename ValT>
	void SequentialMap<KeyT, ValT>::pushBack(const KeyT& key, const ValT& value)
	{
		base_t::insert(base_t::end(), std::make_pair(key, value))->second;
	}

	/*
		class JSONException
	*/
	class JSONException : public std::exception
	{
	public:
		JSONException(const char* message);
		virtual ~JSONException();

		const char* what() const noexcept override;
		
	protected:
		template<typename T>
		static std::string toString(const T& element);

	private:
		const char* message;
	};

	JSONException::JSONException(const char* message) :
		message(message)
	{}

	JSONException::~JSONException()
	{
		delete[] message;
	}

	const char* JSONException::what() const noexcept
	{
		return message;
	}

	template<typename T>
	std::string JSONException::toString(const T& element)
	{
		std::stringstream s;
		s << element;
		return s.str();
	}

	template<>
	inline std::string JSONException::toString(const std::string& element)
	{
		return element;
	}

	/*
		class JSONBadType
	*/
	class JSONBadType : public JSONException
	{
	public:
		static JSONBadType Bool() noexcept;
		static JSONBadType Number() noexcept;
		static JSONBadType String() noexcept;
		static JSONBadType Array() noexcept;
		static JSONBadType Object() noexcept;

		JSONBadType(const char* what);
		virtual ~JSONBadType();
	};

	JSONBadType JSONBadType::Bool() noexcept
	{
		return JSONBadType("El objeto json debe ser de tipo 'bool'");
	}

	JSONBadType JSONBadType::Number() noexcept
	{
		return JSONBadType("El objeto json debe ser de tipo 'number'");
	}

	JSONBadType JSONBadType::String() noexcept
	{
		return JSONBadType("El objeto json debe ser de tipo 'string'");
	}

	JSONBadType JSONBadType::Array() noexcept
	{
		return JSONBadType("El objeto json debe ser de tipo 'array'");
	}

	JSONBadType JSONBadType::Object() noexcept
	{
		return JSONBadType("El objeto json debe ser de tipo 'object'");
	}

	JSONBadType::JSONBadType(const char* message) :
		JSONException(message)
	{}

	JSONBadType::~JSONBadType()
	{}

	/*
		class JSONOutOfRange
	*/
	class JSONOutOfRange : public JSONException
	{
	public:
		static JSONOutOfRange Array(uint32_t position) noexcept;
		static JSONOutOfRange Object(const std::string& key) noexcept;

		JSONOutOfRange(const char* what);
		virtual ~JSONOutOfRange();
	};

	JSONOutOfRange JSONOutOfRange::Array(uint32_t position) noexcept
	{
		return JSONOutOfRange(std::string("La posición " + toString(position) + " se encuentra fuera de los límites del arreglo").c_str());
	}

	JSONOutOfRange JSONOutOfRange::Object(const std::string& key) noexcept
	{
		return JSONOutOfRange(std::string("La clave " + key + " no forma parte del objecto").c_str());
	}

	JSONOutOfRange::JSONOutOfRange(const char* message) :
		JSONException(message)
	{}

	JSONOutOfRange::~JSONOutOfRange()
	{}

	/*
		class JSON
	*/
	class JSON
	{
	public:
		enum Type
		{
			NULL_TYPE,
			BOOL_TYPE,
			NUMBER,
			STRING,
			ARRAY,
			OBJECT
		};

		typedef bool bool_t;
		typedef double number_t;
		typedef std::string string_t;
		typedef std::vector<JSON*> array_t;
		typedef SequentialMap<std::string, JSON> object_t;

		class JSONPair
		{
		public:
			JSONPair(const std::string& key, const JSON& value);
			JSONPair(const std::string& key, JSON&& value);
			~JSONPair();

		private:
			friend class JSON;

			mutable std::string key;
			mutable JSON* value;
		};

		class iterator;
		class const_iterator;

		static JSON Array(std::initializer_list<JSON> init_list = {});
		static JSON Object(std::initializer_list<JSONPair> init_list = {});

		JSON();
		JSON(const JSON& json);
		JSON(JSON&& json);
		JSON(Type type);

		JSON(bool element);
		JSON(int8_t element);
		JSON(int16_t element);
		JSON(int32_t element);
		JSON(int64_t element);
		JSON(float element);
		JSON(double element);
		JSON(const char* element);
		JSON(const std::string& element);
		JSON(std::string&& element);

		JSON(std::initializer_list<JSONPair> init_list);

		~JSON();

		JSON& operator=(const JSON& json);
		JSON& operator=(JSON&& json);

		Type getType() const;

		iterator begin();
		iterator end();

		const_iterator begin() const;
		const_iterator end() const;

		iterator find(const std::string& key);
		const_iterator find(const std::string& key) const;

		iterator erase(iterator position);

		JSON& operator[](uint32_t position);
		const JSON& operator[](uint32_t position) const;

		JSON& operator[](const std::string& key);
		const JSON& operator[](const std::string& key) const;

		void pushBack(const JSON& element);
		void pushBack(JSON&& element);

		template<typename T>
		T& get();

		template<typename T>
		const T& get() const;

		template<typename T>
		T& get(uint32_t position);

		template<typename T>
		const T& get(uint32_t position) const;

		template<typename T>
		T& get(const std::string& key);

		template<typename T>
		const T& get(const std::string& key) const;

		void clear();

	private:
		typedef std::function<void*(const void*)> CopyFunction;
		typedef std::function<void(const void*)> DeleteFunction;

		static const std::array<CopyFunction, 6> COPY_FUNCTION_ARRAY;
		static const std::array<DeleteFunction, 6> DELETE_FUNCTION_ARRAY;

		Type type;
		void* data;

		bool_t& getBool();
		number_t& getNumber();
		string_t& getString();
		array_t& getArray();
		object_t& getObject();

		const bool_t& getBool() const;
		const number_t& getNumber() const;
		const string_t& getString() const;
		const array_t& getArray() const;
		const object_t& getObject() const;

		void forceAddElement(const std::string& key, const JSON& element);
		void forceAddElement(const std::string& key, JSON&& element);
	};

	class JSON::iterator
	{
	public:
		iterator();
		iterator(JSON* json);
		iterator(array_t::iterator array_it);
		iterator(object_t::iterator object_it);
		~iterator();

		bool operator==(const iterator& it) const;
		bool operator!=(const iterator& it) const;

		bool operator==(const const_iterator& it) const;
		bool operator!=(const const_iterator& it) const;

		const std::string& key() const;

		JSON* operator->();
		JSON& operator*();

		iterator operator++();
		iterator operator++(int);

		iterator operator--();
		iterator operator--(int);

	private:
		friend class JSON;
		friend class const_iterator;

		Type type;
		union
		{
			JSON* json;
			array_t::iterator array_it;
			object_t::iterator object_it;
		};
	};

	class JSON::const_iterator
	{
	public:
		const_iterator();
		const_iterator(const JSON* json);
		const_iterator(array_t::const_iterator array_it);
		const_iterator(object_t::const_iterator object_it);
		~const_iterator();

		bool operator==(const const_iterator& it) const;
		bool operator!=(const const_iterator& it) const;

		bool operator==(const iterator& it) const;
		bool operator!=(const iterator& it) const;

		const std::string& key() const;

		const JSON* operator->() const;
		const JSON& operator*() const;

		const_iterator operator++();
		const_iterator operator++(int);

		const_iterator operator--();
		const_iterator operator--(int);

	private:
		friend class JSON;
		friend class iterator;

		Type type;
		union
		{
			const JSON* json;
			array_t::const_iterator array_it;
			object_t::const_iterator object_it;
		};
	};

	/*
		class JSON::iterator
	*/
	JSON::iterator::iterator() :
		type(NULL_TYPE),
		json(nullptr)
	{}

	JSON::iterator::iterator(JSON* json) :
		type(json->type),
		json(json)
	{}

	JSON::iterator::iterator(array_t::iterator array_it) :
		type(ARRAY),
		array_it(array_it)
	{}

	JSON::iterator::iterator(object_t::iterator object_it) :
		type(OBJECT),
		object_it(object_it)
	{}

	JSON::iterator::~iterator()
	{}

	bool JSON::iterator::operator==(const iterator& it) const
	{
		if(type != it.type)
		{
			return false;
		}

		switch(type)
		{
		case NULL_TYPE:
			return true;

		case BOOL_TYPE:
			return json == it.json;

		case NUMBER:
			return json == it.json;

		case STRING:
			return json == it.json;

		case ARRAY:
			return array_it == it.array_it;

		case OBJECT:
			return object_it == it.object_it;
		}
	}

	bool JSON::iterator::operator!=(const iterator& it) const
	{
		return !(*this == it);
	}

	bool JSON::iterator::operator==(const const_iterator& it) const
	{
		if(type != it.type)
		{
			return false;
		}

		switch(type)
		{
		case NULL_TYPE:
			return true;

		case BOOL_TYPE:
			return json == it.json;

		case NUMBER:
			return json == it.json;

		case STRING:
			return json == it.json;

		case ARRAY:
			return array_it == it.array_it;

		case OBJECT:
			return object_it == it.object_it;
		}
	}

	bool JSON::iterator::operator!=(const const_iterator& it) const
	{
		return !(*this == it);
	}

	const std::string& JSON::iterator::key() const
	{
		if(type != OBJECT)
		{
			throw JSONBadType::Object();
		}

		return object_it->first;
	}

	JSON* JSON::iterator::operator->()
	{
		switch(type)
		{
		case NULL_TYPE:
			return json;

		case BOOL_TYPE:
			return json;

		case NUMBER:
			return json;

		case STRING:
			return json;

		case ARRAY:
			return *array_it;

		case OBJECT:
			return &(object_it->second);
		}
	}

	JSON& JSON::iterator::operator*()
	{
		switch(type)
		{
		case NULL_TYPE:
			return *json;

		case BOOL_TYPE:
			return *json;

		case NUMBER:
			return *json;

		case STRING:
			return *json;

		case ARRAY:
			return *(*array_it);

		case OBJECT:
			return object_it->second;
		}
	}

	JSON::iterator JSON::iterator::operator++()
	{
		switch(type)
		{
		case NULL_TYPE:
			json = nullptr;
			break;

		case BOOL_TYPE:
			json = nullptr;
			break;

		case NUMBER:
			json = nullptr;
			break;

		case STRING:
			json = nullptr;
			break;

		case ARRAY:
			++array_it;
			break;

		case OBJECT:
			++object_it;
			break;
		}

		return *this;
	}

	JSON::iterator JSON::iterator::operator++(int)
	{
		iterator temp = *this;

		++(*this);

		return temp;
	}

	JSON::iterator JSON::iterator::operator--()
	{
		switch(type)
		{
		case NULL_TYPE:
			json = nullptr;
			break;

		case BOOL_TYPE:
			json = nullptr;
			break;

		case NUMBER:
			json = nullptr;
			break;

		case STRING:
			json = nullptr;
			break;

		case ARRAY:
			--array_it;
			break;

		case OBJECT:
			--object_it;
			break;
		}

		return *this;
	}

	JSON::iterator JSON::iterator::operator--(int)
	{
		iterator temp = *this;

		--(*this);

		return temp;
	}

	/*
		class JSON::const_iterator
	*/
	JSON::const_iterator::const_iterator() :
		type(NULL_TYPE),
		json(nullptr)
	{}

	JSON::const_iterator::const_iterator(const JSON* json) :
		type(json->type),
		json(json)
	{}

	JSON::const_iterator::const_iterator(array_t::const_iterator array_it) :
		type(ARRAY),
		array_it(array_it)
	{}

	JSON::const_iterator::const_iterator(object_t::const_iterator object_it) :
		type(OBJECT),
		object_it(object_it)
	{}

	JSON::const_iterator::~const_iterator()
	{}

	bool JSON::const_iterator::operator==(const const_iterator& it) const
	{
		if(type != it.type)
		{
			return false;
		}

		switch(type)
		{
		case NULL_TYPE:
			return true;

		case BOOL_TYPE:
			return json == it.json;

		case NUMBER:
			return json == it.json;

		case STRING:
			return json == it.json;

		case ARRAY:
			return array_it == it.array_it;

		case OBJECT:
			return object_it == it.object_it;
		}
	}

	bool JSON::const_iterator::operator!=(const const_iterator& it) const
	{
		return !(*this == it);
	}

	bool JSON::const_iterator::operator==(const iterator& it) const
	{
		if(type != it.type)
		{
			return false;
		}

		switch(type)
		{
		case NULL_TYPE:
			return true;

		case BOOL_TYPE:
			return json == it.json;

		case NUMBER:
			return json == it.json;

		case STRING:
			return json == it.json;

		case ARRAY:
			return array_it == it.array_it;

		case OBJECT:
			return object_it == it.object_it;
		}
	}

	bool JSON::const_iterator::operator!=(const iterator& it) const
	{
		return !(*this == it);
	}

	const std::string& JSON::const_iterator::key() const
	{
		if(type != OBJECT)
		{
			throw JSONBadType::Object();
		}

		return object_it->first;
	}

	const JSON* JSON::const_iterator::operator->() const
	{
		switch(type)
		{
		case NULL_TYPE:
			return json;

		case BOOL_TYPE:
			return json;

		case NUMBER:
			return json;

		case STRING:
			return json;

		case ARRAY:
			return *array_it;

		case OBJECT:
			return &(object_it->second);
		}
	}

	const JSON& JSON::const_iterator::operator*() const
	{
		switch(type)
		{
		case NULL_TYPE:
			return *json;

		case BOOL_TYPE:
			return *json;

		case NUMBER:
			return *json;

		case STRING:
			return *json;

		case ARRAY:
			return *(*array_it);

		case OBJECT:
			return object_it->second;
		}
	}

	JSON::const_iterator JSON::const_iterator::operator++()
	{
		switch(type)
		{
		case NULL_TYPE:
			json = nullptr;
			break;

		case BOOL_TYPE:
			json = nullptr;
			break;

		case NUMBER:
			json = nullptr;
			break;

		case STRING:
			json = nullptr;
			break;

		case ARRAY:
			++array_it;
			break;

		case OBJECT:
			++object_it;
			break;
		}

		return *this;
	}

	JSON::const_iterator JSON::const_iterator::operator++(int)
	{
		const_iterator temp = *this;

		++(*this);

		return temp;
	}

	JSON::const_iterator JSON::const_iterator::operator--()
	{
		switch(type)
		{
		case NULL_TYPE:
			json = nullptr;
			break;

		case BOOL_TYPE:
			json = nullptr;
			break;

		case NUMBER:
			json = nullptr;
			break;

		case STRING:
			json = nullptr;
			break;

		case ARRAY:
			--array_it;
			break;

		case OBJECT:
			--object_it;
			break;
		}

		return *this;
	}

	JSON::const_iterator JSON::const_iterator::operator--(int)
	{
		const_iterator temp = *this;

		--(*this);

		return temp;
	}

	/*
		class JSON
	*/
	const std::array<JSON::CopyFunction, 6> JSON::COPY_FUNCTION_ARRAY({
		// null
		[](const void* data) -> void* {
			return nullptr;
		},

		// bool
		[](const void* data) -> void* {
			return new bool_t(*reinterpret_cast<const bool_t*>(data));
		},

		// number
		[](const void* data) -> void* {
			return new number_t(*reinterpret_cast<const number_t*>(data));
		},

		// string
		[](const void* data) -> void* {
			return new string_t(*reinterpret_cast<const string_t*>(data));
		},

		// array
		[](const void* data) -> void* {
			const auto& arr_data = *reinterpret_cast<const array_t*>(data);

			array_t* result = new array_t(arr_data.size());

			for(uint32_t i = 0; i < result->size(); ++i)
			{
				(*result)[i] = new JSON(*arr_data[i]);
			}

			return result;
		},

		// object
		[](const void* data) -> void* {
			return new object_t(*reinterpret_cast<const object_t*>(data));
		},
	});

	const std::array<JSON::DeleteFunction, 6> JSON::DELETE_FUNCTION_ARRAY({
		// null
		[](const void* data) {},

		// bool
		[](const void* data) {
			delete reinterpret_cast<const bool_t*>(data);
		},

		// number
		[](const void* data) {
			delete reinterpret_cast<const number_t*>(data);
		},

		// string
		[](const void* data) {
			delete reinterpret_cast<const string_t*>(data);
		},

		// array
		[](const void* data) {
			auto arr_data = reinterpret_cast<const array_t*>(data);

			for(auto json : *arr_data)
			{
				delete json;
			}

			delete arr_data;
		},

		// object
		[](const void* data) {
			delete reinterpret_cast<const object_t*>(data);
		},
	});

	JSON JSON::Array(std::initializer_list<JSON> init_list)
	{
		JSON json(ARRAY);
		auto& arr = json.getArray();

		for(auto& json : init_list)
		{
			arr.push_back(new JSON(std::move(json)));
		}

		return json;
	}

	JSON JSON::Object(std::initializer_list<JSON::JSONPair> init_list)
	{
		return JSON(init_list);
	}

	JSON::JSON() :
		type(NULL_TYPE),
		data(nullptr)
	{}

	JSON::JSON(const JSON& json) :
		type(NULL_TYPE),
		data(nullptr)
	{
		*this = json;
	}

	JSON::JSON(JSON&& json) :
		type(NULL_TYPE),
		data(nullptr)
	{
		*this = std::move(json);
	}

	JSON::JSON(Type type) :
		type(type)
	{
		switch(type)
		{
		case NULL_TYPE:
			data = nullptr;
			break;

		case BOOL_TYPE:
			data = new bool_t(false);
			break;

		case NUMBER:
			data = new number_t(0);
			break;

		case STRING:
			data = new string_t();
			break;

		case ARRAY:
			data = new array_t();
			break;

		case OBJECT:
			data = new object_t();
			break;
		}
	}

	JSON::JSON(bool element) :
		type(BOOL_TYPE),
		data(new bool_t(element))
	{}

	JSON::JSON(int8_t element) :
		type(NUMBER),
		data(new number_t(element))
	{}

	JSON::JSON(int16_t element) :
		type(NUMBER),
		data(new number_t(element))
	{}

	JSON::JSON(int32_t element) :
		type(NUMBER),
		data(new number_t(element))
	{}

	JSON::JSON(int64_t element) :
		type(NUMBER),
		data(new number_t(element))
	{}

	JSON::JSON(float element) :
		type(NUMBER),
		data(new number_t(element))
	{}

	JSON::JSON(double element) :
		type(NUMBER),
		data(new number_t(element))
	{}

	JSON::JSON(const char* element) :
		type(STRING),
		data(new string_t(element))
	{}

	JSON::JSON(const std::string& element) :
		type(STRING),
		data(new string_t(element))
	{}

	JSON::JSON(std::string&& element) :
		type(STRING),
		data(new string_t(element))
	{}

	JSON::JSON(std::initializer_list<JSONPair> init_list) :
		type(OBJECT),
		data(new object_t())
	{
		auto& obj = getObject();

		for(auto& json_pair : init_list)
		{
			obj[json_pair.key] = std::move(*json_pair.value);
		}
	}

	JSON::~JSON()
	{
		clear();
	}

	JSON& JSON::operator=(const JSON& json)
	{
		clear();

		type = json.type;
		data = COPY_FUNCTION_ARRAY[type](json.data);

		return *this;
	}

	JSON& JSON::operator=(JSON&& json)
	{
		clear();

		type = json.type;
		data = json.data;

		json.type = NULL_TYPE;
		json.data = nullptr;

		return *this;
	}

	JSON::Type JSON::getType() const
	{
		return type;
	}

	JSON::iterator JSON::begin()
	{
		switch(type)
		{
		case ARRAY:
			return iterator(getArray().begin());

		case OBJECT:
			return iterator(getObject().begin());

		default:
			return iterator(this);
		}
	}

	JSON::iterator JSON::end()
	{
		switch(type)
		{
		case ARRAY:
			return iterator(getArray().end());

		case OBJECT:
			return iterator(getObject().end());

		default:
			iterator it(this);
			++it;
			return it;
		}
	}

	JSON::const_iterator JSON::begin() const
	{
		switch(type)
		{
		case ARRAY:
			return const_iterator(getArray().begin());

		case OBJECT:
			return const_iterator(getObject().begin());

		default:
			return const_iterator(this);
		}
	}

	JSON::const_iterator JSON::end() const
	{
		switch(type)
		{
		case ARRAY:
			return const_iterator(getArray().end());

		case OBJECT:
			return const_iterator(getObject().end());

		default:
			const_iterator it(this);
			++it;
			return it;
		}
	}

	JSON::iterator JSON::find(const std::string& key)
	{
		if(type != OBJECT)
		{
			throw JSONBadType::Object();
		}

		return iterator(getObject().find(key));
	}

	JSON::const_iterator JSON::find(const std::string& key) const
	{
		if(type != OBJECT)
		{
			throw JSONBadType::Object();
		}

		return const_iterator(getObject().find(key));
	}

	JSON::iterator JSON::erase(iterator position)
	{
		switch(position.type)
		{
		case ARRAY:
			if(type != ARRAY || position == end())
			{
				return end();
			}

			delete position.json;

			return iterator(getArray().erase(position.array_it));

		case OBJECT:
			if(type != OBJECT || position == end())
			{
				return end();
			}

			return iterator(getObject().erase(position.key()));

		default:
			throw JSONBadType::Object();
		}
	}

	JSON& JSON::operator[](uint32_t position)
	{
		if(type != ARRAY)
		{
			throw JSONBadType::Array();
		}

		return *getArray()[position];
	}

	const JSON& JSON::operator[](uint32_t position) const
	{
		if(type != ARRAY)
		{
			throw JSONBadType::Array();
		}

		return *getArray()[position];
	}

	JSON& JSON::operator[](const std::string& key)
	{
		if(type != OBJECT)
		{
			throw JSONBadType::Object();
		}

		return getObject()[key];
	}

	const JSON& JSON::operator[](const std::string& key) const
	{
		if(type != OBJECT)
		{
			throw JSONBadType::Object();
		}

		return getObject().at(key);
	}

	void JSON::pushBack(const JSON& element)
	{
		if(type != ARRAY)
		{
			throw JSONBadType::Array();
		}

		getArray().push_back(new JSON(element));
	}

	void JSON::pushBack(JSON&& element)
	{
		if(type != ARRAY)
		{
			throw JSONBadType::Array();
		}

		getArray().push_back(new JSON(std::move(element)));
	}

	void JSON::clear()
	{
		DELETE_FUNCTION_ARRAY[type](data);

		type = NULL_TYPE;
		data = nullptr;
	}

	JSON::bool_t& JSON::getBool()
	{
		return *reinterpret_cast<bool_t*>(data);
	}

	JSON::number_t& JSON::getNumber()
	{
		return *reinterpret_cast<number_t*>(data);
	}

	JSON::string_t& JSON::getString()
	{
		return *reinterpret_cast<string_t*>(data);
	}

	JSON::array_t& JSON::getArray()
	{
		return *reinterpret_cast<array_t*>(data);
	}

	JSON::object_t& JSON::getObject()
	{
		return *reinterpret_cast<object_t*>(data);
	}

	const JSON::bool_t& JSON::getBool() const
	{
		return *reinterpret_cast<const bool_t*>(data);
	}

	const JSON::number_t& JSON::getNumber() const
	{
		return *reinterpret_cast<const number_t*>(data);
	}

	const JSON::string_t& JSON::getString() const
	{
		return *reinterpret_cast<const string_t*>(data);
	}

	const JSON::array_t& JSON::getArray() const
	{
		return *reinterpret_cast<const array_t*>(data);
	}

	const JSON::object_t& JSON::getObject() const
	{
		return *reinterpret_cast<const object_t*>(data);
	}

	void JSON::forceAddElement(const std::string& key, const JSON& element)
	{
		if(type != OBJECT)
		{
			throw JSONBadType::Object();
		}

		getObject().pushBack(key, element);
	}

	void JSON::forceAddElement(const std::string& key, JSON&& element)
	{
		if(type != OBJECT)
		{
			throw JSONBadType::Object();
		}

		getObject().pushBack(key, std::move(element));
	}

	template<typename T>
	T& JSON::get()
	{
		throw JSONBadType("El tipo solicitado es un tipo fundamental de JSON");
	}

	template<>
	inline bool& JSON::get()
	{
		if(type != BOOL_TYPE)
		{
			throw JSONBadType::Bool();
		}

		return getBool();
	}

	template<>
	inline JSON::number_t& JSON::get()
	{
		if(type != NUMBER)
		{
			throw JSONBadType::Number();
		}

		return getNumber();
	}

	template<>
	inline std::string& JSON::get()
	{
		if(type != STRING)
		{
			throw JSONBadType::String();
		}

		return getString();
	}

	template<typename T>
	const T& JSON::get() const
	{
		throw JSONBadType("El tipo solicitado es un tipo fundamental de JSON");
	}

	template<>
	inline const bool& JSON::get() const
	{
		if(type != BOOL_TYPE)
		{
			throw JSONBadType::Bool();
		}

		return getBool();
	}

	template<>
	inline const JSON::number_t& JSON::get() const
	{
		if(type != NUMBER)
		{
			throw JSONBadType::Number();
		}

		return getNumber();
	}

	template<>
	inline const std::string& JSON::get() const
	{
		if(type != STRING)
		{
			throw JSONBadType::String();
		}

		return getString();
	}

	template<typename T>
	T& JSON::get(uint32_t position)
	{
		if(type != ARRAY)
		{
			throw JSONBadType::Array();
		}

		return getArray()[position]->get<T>();
	}

	template<typename T>
	const T& JSON::get(uint32_t position) const
	{
		if(type != ARRAY)
		{
			throw JSONBadType::Array();
		}

		return getArray()[position]->get<T>();
	}

	template<typename T>
	T& JSON::get(const std::string& key)
	{
		if(type != OBJECT)
		{
			throw JSONBadType::Object();
		}

		return getObject()[key].get<T>();
	}

	template<typename T>
	const T& JSON::get(const std::string& key) const
	{
		if(type != OBJECT)
		{
			throw JSONBadType::Object();
		}

		return getObject().at(key).get<T>();
	}

	/*
		class JSON::JSONPair
	*/
	JSON::JSONPair::JSONPair(const std::string& key, const JSON& json) :
		key(key),
		value(new JSON(json))
	{}

	JSON::JSONPair::JSONPair(const std::string& key, JSON&& json) :
		key(key),
		value(new JSON(std::move(json)))
	{}

	JSON::JSONPair::~JSONPair()
	{
		delete value;
	}
}
