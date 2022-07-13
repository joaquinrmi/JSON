/*
   JSON v.1.0
   Joaquín Ruaimi
   https://github.com/joaquinrmi/JSON
*/

#pragma once

#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <vector>
#include <array>
#include <map>
#include <string>
#include <functional>
#include <sstream>
#include <fstream>

namespace dnc
{
   /*
      namespace StringUtils
   */
   namespace StringUtils
   {
      template<typename T>
      inline std::string toString(const T& element)
      {
         std::stringstream s;
         s << element;
         return s.str();
      }

      template<>
      inline std::string toString(const std::string& element)
      {
         return element;
      }
   }

   /*
      class UTF8Analyzer
   */
   class UTF8Analyzer
   {
   public:
      static bool countNextChar(const std::string& utf8_chars, int& target, uint32_t pos);
      static bool readNextByte(const std::string& utf8_chars, uint32_t pos);

   private:
      UTF8Analyzer();

      static const uint8_t ASCII_HEADER_MIN;
      static const uint8_t ASCII_HEADER_MAX;
      static const uint8_t TWO_BYTES_HEADER_MAX;
      static const uint8_t THREE_BYTES_HEADER_MAX;
      static const uint8_t FOUR_BYTES_HEADER_MAX;
      static const uint8_t NEXT_BYTE_HEADER_MIN;
      static const uint8_t NEXT_BYTE_HEADER_MAX;
   };

   const uint8_t UTF8Analyzer::ASCII_HEADER_MIN = 0;
   const uint8_t UTF8Analyzer::ASCII_HEADER_MAX = 128;
   const uint8_t UTF8Analyzer::TWO_BYTES_HEADER_MAX = 224;
   const uint8_t UTF8Analyzer::THREE_BYTES_HEADER_MAX = 240;
   const uint8_t UTF8Analyzer::FOUR_BYTES_HEADER_MAX = 247;
   const uint8_t UTF8Analyzer::NEXT_BYTE_HEADER_MIN = 128;
   const uint8_t UTF8Analyzer::NEXT_BYTE_HEADER_MAX = 192;

   UTF8Analyzer::UTF8Analyzer()
   {}

   bool UTF8Analyzer::countNextChar(const std::string& utf8_chars, int& target, uint32_t pos)
   {
      if(pos >= utf8_chars.size()) return false;
      
      uint8_t c = utf8_chars[pos];
      
      if(c < ASCII_HEADER_MAX)
      {
         target = 1;
         return true;
      }
      else if(c < TWO_BYTES_HEADER_MAX)
      {
         ++pos;
         if(!readNextByte(utf8_chars, pos)) return false;
         target = 2;
         return true;
      }
      else if(c < THREE_BYTES_HEADER_MAX)
      {
         for(int j = 0; j < 2; ++j)
         {
            ++pos;
            if(!readNextByte(utf8_chars, pos)) return false;
         }
         target = 3;
         return true;
      }
      else if(c < FOUR_BYTES_HEADER_MAX)
      {
         for(int j = 0; j < 3; ++j)
         {
            ++pos;
            if(!readNextByte(utf8_chars, pos)) return false;
         }
         target = 4;
         return true;
      }

      return false;
   }

   bool UTF8Analyzer::readNextByte(const std::string& utf8_chars, uint32_t pos)
   {
      if(pos >= utf8_chars.size()) return false;

      uint8_t c = utf8_chars[pos];
      
      if(c >= NEXT_BYTE_HEADER_MIN && c < NEXT_BYTE_HEADER_MAX)
      {
         return true;
      }
      
      return false;
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
      return JSONOutOfRange(std::string("La posición " + StringUtils::toString(position) + " se encuentra fuera de los límites del arreglo").c_str());
   }

   JSONOutOfRange JSONOutOfRange::Object(const std::string& key) noexcept
   {
      return JSONOutOfRange(std::string("La clave " + key + " no forma parte del objeto").c_str());
   }

   JSONOutOfRange::JSONOutOfRange(const char* message) :
      JSONException(message)
   {}

   JSONOutOfRange::~JSONOutOfRange()
   {}

   /*
      class JSONParseStatus
   */
   class JSONParseStatus
   {
   public:
      JSONParseStatus();
      JSONParseStatus(std::string&& message);
      virtual ~JSONParseStatus();

      bool ok() const noexcept;
      const std::string& what() const noexcept;

   private:
      bool status;
      std::string message;
   };

   JSONParseStatus::JSONParseStatus() :
      status(true)
   {}

   JSONParseStatus::JSONParseStatus(std::string&& message) :
      status(false),
      message(std::move(message))
   {}

   JSONParseStatus::~JSONParseStatus()
   {}

   bool JSONParseStatus::ok() const noexcept
   {
      return status;
   }

   const std::string& JSONParseStatus::what() const noexcept
   {
      return message;
   }

   /*
      class UnexpectedToken
   */
   class UnexpectedToken : public JSONParseStatus
   {
   public:
      UnexpectedToken(const std::string& token, int position);
      ~UnexpectedToken();
   };

   UnexpectedToken::UnexpectedToken(const std::string& token, int position) :
      JSONParseStatus(std::move("unexpected token '" + token + "' at position " + StringUtils::toString(position)))
   {}

   UnexpectedToken::~UnexpectedToken()
   {}

   /*
      class CannotOpenFile
   */
   class CannotOpenFile : public JSONParseStatus
   {
   public:
      CannotOpenFile(const std::string& filename);
      ~CannotOpenFile();
   };

   CannotOpenFile::CannotOpenFile(const std::string& filename) :
      JSONParseStatus(std::move("cannot open file " + filename))
   {}

   CannotOpenFile::~CannotOpenFile()
   {}

   /*
      class InvalidUTF8Byte
   */
   class InvalidUTF8Byte : public JSONParseStatus
   {
   public:
      InvalidUTF8Byte(int position);
      ~InvalidUTF8Byte();
   };

   InvalidUTF8Byte::InvalidUTF8Byte(int position) :
      JSONParseStatus(std::move("utf8 invalid byte at position " + StringUtils::toString(position)))
   {}

   InvalidUTF8Byte::~InvalidUTF8Byte()
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
      typedef std::map<std::string, JSON> object_t;

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
      uint32_t size() const;

      JSONParseStatus parse(const std::string& text);
      JSONParseStatus parseFromFile(const std::string& filename);

      bool saveToFile(const std::string& filename) const;

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

      std::string toString() const;

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

   template<typename T>
   T& JSON::get()
   {
      throw JSONBadType("El tipo solicitado no es un tipo fundamental de JSON");
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

      auto& arr = getArray();

      if(position >= arr.size())
      {
         throw JSONOutOfRange::Array(position);
      }

      return arr[position]->get<T>();
   }

   template<typename T>
   const T& JSON::get(uint32_t position) const
   {
      if(type != ARRAY)
      {
         throw JSONBadType::Array();
      }

      auto& arr = getArray();

      if(position >= arr.size())
      {
         throw JSONOutOfRange::Array(position);
      }

      return arr[position]->get<T>();
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

      auto& obj = getObject();
      auto found = obj.find(key);
      if(found == obj.end())
      {
         throw JSONOutOfRange::Object(key);
      }

      return found->second.get<T>();
   }

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
      TextStatus
   */
   class TextStatus
   {
   public:
      TextStatus();
      TextStatus(const std::string& message);

      bool ok() const noexcept;

   private:
      bool status;
      std::string message;
   };

   TextStatus::TextStatus() :
      status(true)
   {}
   
   TextStatus::TextStatus(const std::string& message) :
      status(false),
      message(message)
   {}

   bool TextStatus::ok() const noexcept
   {
      return status;
   }

   /*
      class TextToken
   */
   class TextToken
   {
   public:
      enum Type
      {
         SPACE,
         SYMBOL,
         WORD,
         NUMBER
      };

      TextToken();
      TextToken(Type type, const std::string& value);
      TextToken(Type type, std::string&& value);

      Type type;
      std::string value;
   };

   TextToken::TextToken() :
      type(SPACE),
      value("")
   {}

   TextToken::TextToken(Type type, const std::string& value) :
      type(type),
      value(value)
   {}

   TextToken::TextToken(Type type, std::string&& value) :
      type(type),
      value(std::move(value))
   {}

   enum CharType
   {
      C_BLANK,
      C_SYMBOL,
      C_NUMBER,
      C_LETTER
   };

   /*
      class UTF8Tokenizator
   */
   class UTF8Tokenizator
   {
   public:
      static TextStatus getToken(const std::string& text, uint32_t pos, TextToken& target);

   private:
      static const std::vector<CharType> CHAR_TYPES;
      static const uint8_t NUMBER_POINT;

      UTF8Tokenizator();
   };

   const std::vector<CharType> UTF8Tokenizator::CHAR_TYPES({
      C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK,
      C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK,
      C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK,
      C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK,
      C_BLANK, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL,
      C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL,
      C_NUMBER, C_NUMBER, C_NUMBER, C_NUMBER, C_NUMBER, C_NUMBER, C_NUMBER, C_NUMBER,
      C_NUMBER, C_NUMBER, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL,
      C_SYMBOL, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL,
      C_SYMBOL, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_BLANK
   });

   const uint8_t UTF8Tokenizator::NUMBER_POINT = '.';

   UTF8Tokenizator::UTF8Tokenizator()
   {}

   TextStatus UTF8Tokenizator::getToken(const std::string& text, uint32_t pos, TextToken& target)
   {
      int32_t char_count = 0;
      if(!UTF8Analyzer::countNextChar(text, char_count, pos))
      {
         return TextStatus("");
      }

      if(char_count == 1)
      {
         uint8_t c = text[pos];
         auto char_type = CHAR_TYPES[c];

         switch(char_type)
         {
         case C_BLANK:
         {
            target.type = TextToken::SPACE;
            target.value = text.substr(pos, char_count);

            auto pos2 = pos + 1;
            while(true)
            {
               int32_t char_count_2 = 0;
               if(!UTF8Analyzer::countNextChar(text, char_count_2, pos2))
               {
                  return TextStatus();
               }

               if(char_count_2 > 1)
               {
                  return TextStatus();
               }

               uint8_t c2 = text[pos2];
               if(CHAR_TYPES[c2] != C_BLANK)
               {
                  return TextStatus();
               }

               target.value += text.substr(pos2, char_count_2);
               ++pos2;
            }

            break;
         }

         case C_SYMBOL:
         {
            target.type = TextToken::SYMBOL;
            target.value = text.substr(pos, char_count);
            return TextStatus();
         }

         case C_NUMBER:
         {
            target.type = TextToken::NUMBER;
            target.value = text.substr(pos, char_count);

            bool added_point = false;

            auto pos2 = pos + 1;
            while(true)
            {
               int32_t char_count_2 = 0;
               if(!UTF8Analyzer::countNextChar(text, char_count_2, pos2))
               {
                  return TextStatus();
               }

               if(char_count_2 > 1)
               {
                  return TextStatus();
               }

               uint8_t c2 = text[pos2];
               if(CHAR_TYPES[c2] != C_NUMBER && CHAR_TYPES[c2] != C_SYMBOL)
               {
                  return TextStatus();
               }

               if(CHAR_TYPES[c2] == C_SYMBOL)
               {
                  if(c2 == NUMBER_POINT && !added_point)
                  {
                     added_point = true;
                  }
                  else return TextStatus();
               }

               target.value += text.substr(pos2, char_count_2);
               ++pos2;
            }

            break;
         }

         case C_LETTER:
            target.type = TextToken::WORD;
            target.value = text.substr(pos, char_count);

            auto pos2 = pos + 1;
            while(true)
            {
               int32_t char_count_2 = 0;
               if(!UTF8Analyzer::countNextChar(text, char_count_2, pos2))
               {
                  return TextStatus();
               }

               if(char_count_2 > 1)
               {
                  return TextStatus();
               }

               uint8_t c2 = text[pos2];
               if(CHAR_TYPES[c2] != C_LETTER)
               {
                  return TextStatus();
               }

               target.value += text.substr(pos2, char_count_2);
               ++pos2;
            }

            break;
         }
      }

      target.type = TextToken::SYMBOL;
      target.value = text.substr(pos, char_count);
      return TextStatus();
   }

   /*
      class JSONParser
   */
   class JSONParser
   {
   public:
      static JSONParseStatus parse(const std::string& text, JSON& target);
      static JSONParseStatus parse(const std::string& text, uint32_t pos, JSON& target);
      static JSONParseStatus parseFromFile(const std::string& filename, JSON& target);

   private:
      static const uint8_t BRACKET_OPEN;
      static const uint8_t BRACKET_CLOSE;
      static const uint8_t SQUARE_BRACKET_OPEN;
      static const uint8_t SQUARE_BRACKET_CLOSE;
      static const uint8_t QUOTATION_MARK;
      static const uint8_t COMMA;
      static const uint8_t COLON;
      static const uint8_t SEMICOLON;
      static const uint8_t HYPHEN;
      static const uint8_t ESCAPE_BAR;
      static const uint8_t TAB_LETTER;
      static const uint8_t RETURN_LETTER;
      static const uint8_t NEW_LINE_LETTER;

      JSONParser();

      static JSONParseStatus getJSONElement(const std::string& text, uint32_t& pos, JSON& target);
      static JSONParseStatus getJSONString(const std::string& text, uint32_t& pos, JSON& target);
      static JSONParseStatus getJSONArray(const std::string& text, uint32_t& pos, JSON& target);
      static JSONParseStatus getJSONObject(const std::string& text, uint32_t& pos, JSON& target);
   };

   const uint8_t JSONParser::BRACKET_OPEN = '{';
   const uint8_t JSONParser::BRACKET_CLOSE = '}';
   const uint8_t JSONParser::SQUARE_BRACKET_OPEN = '[';
   const uint8_t JSONParser::SQUARE_BRACKET_CLOSE = ']';
   const uint8_t JSONParser::QUOTATION_MARK = '\"';
   const uint8_t JSONParser::COMMA = ',';
   const uint8_t JSONParser::COLON = ':';
   const uint8_t JSONParser::SEMICOLON = ';';
   const uint8_t JSONParser::HYPHEN = '-';
   const uint8_t JSONParser::ESCAPE_BAR = '\\';
   const uint8_t JSONParser::TAB_LETTER = 't';
   const uint8_t JSONParser::RETURN_LETTER = 'r';
   const uint8_t JSONParser::NEW_LINE_LETTER = 'n';

   JSONParser::JSONParser()
   {}

   JSONParseStatus JSONParser::parse(const std::string& text, JSON& target)
   {
      JSON temp;

      uint32_t position = 0;
      auto status = getJSONElement(text, position, temp);
      if(!status.ok())
      {
         return status;
      }

      target = std::move(temp);

      return JSONParseStatus();
   }

   JSONParseStatus JSONParser::parse(const std::string& text, uint32_t pos, JSON& target)
   {
      return getJSONElement(text, pos, target);
   }

   JSONParseStatus JSONParser::parseFromFile(const std::string& filename, JSON& target)
   {
      std::ifstream file(filename, std::ios::binary | std::ios::ate);
      if(!file.is_open())
      {
         return CannotOpenFile(filename);
      }

      const uint32_t file_size = file.tellg();
      file.seekg(0);

      std::string text(file_size, ' ');
      for(uint32_t i = 0; i < file_size; ++i)
      {
         char c;
         file.read(&c, sizeof(char));

         text[i] = c;
      }

      return parse(text, 0, target);
   }

   JSONParseStatus JSONParser::getJSONElement(const std::string& text, uint32_t& pos, JSON& target)
   {
      TextToken token;
      auto status = UTF8Tokenizator::getToken(text, pos, token);
      if(!status.ok())
      {
         return InvalidUTF8Byte(pos);
      }

      pos += token.value.size();

      switch(token.type)
      {
      case TextToken::SPACE:
      {
         return getJSONElement(text, pos, target);
      }

      case TextToken::SYMBOL:
      {
         uint8_t c = token.value[0];

         switch(c)
         {
         case BRACKET_OPEN:
            return getJSONObject(text, pos, target);

         case SQUARE_BRACKET_OPEN:
            return getJSONArray(text, pos, target);

         case QUOTATION_MARK:
            return getJSONString(text, pos, target);

         case HYPHEN:
            TextToken ntoken;
            auto status = UTF8Tokenizator::getToken(text, pos, ntoken);
            if(!status.ok())
            {
               return InvalidUTF8Byte(pos);
            }

            pos += ntoken.value.size();

            if(ntoken.type == TextToken::NUMBER)
            {
               std::string number = token.value + ntoken.value;
               target = std::atof(number.c_str());
               return JSONParseStatus();
            }

            break;
         }

         return UnexpectedToken(token.value, pos - token.value.size());
      }

      case TextToken::WORD:
      {
         if(token.value == "true")
         {
            target = true;
            return JSONParseStatus();
         }
         else if(token.value == "false")
         {
            target = false;
            return JSONParseStatus();
         }

         return UnexpectedToken(token.value, pos - token.value.size());
      }

      case TextToken::NUMBER:
         target = std::atof(token.value.c_str());
         return JSONParseStatus();
      }
   }

   JSONParseStatus JSONParser::getJSONString(const std::string& text, uint32_t& pos, JSON& target)
   {
      std::string value;

      bool active_escape_bar = false;

      while(true)
      {
         int char_count;
         if(!UTF8Analyzer::countNextChar(text, char_count, pos))
         {
            return InvalidUTF8Byte(pos);
         }

         if(char_count == 1)
         {
            uint8_t c = text[pos];
            pos += 1;

            switch(c)
            {
            case QUOTATION_MARK:
            {
               if(active_escape_bar)
               {
                  active_escape_bar = false;
                  value.push_back(c);
                  continue;
               }

               target = value;
               return JSONParseStatus();
            }

            case ESCAPE_BAR:
            {
               if(active_escape_bar)
               {
                  active_escape_bar = false;
                  value.push_back(c);
                  continue;
               }

               active_escape_bar = true;

               break;
            }

            default:
               if(active_escape_bar)
               {
                  switch(c)
                  {
                  case TAB_LETTER:
                     active_escape_bar = false;
                     value += "\\t";
                     continue;

                  case RETURN_LETTER:
                     active_escape_bar = false;
                     value += "\\r";
                     continue;

                  case NEW_LINE_LETTER:
                     active_escape_bar = false;
                     value += "\\n";
                     continue;

                  default:
                     return UnexpectedToken(text.substr(pos - 1, 1), pos - 1);
                  }
               }

               value.push_back(c);
               break;
            }

            continue;
         }

         if(active_escape_bar)
         {
            return UnexpectedToken(text.substr(pos, char_count), pos);
         }

         value += text.substr(pos, char_count);
         pos += char_count;
      }

      return JSONParseStatus();
   }

   JSONParseStatus JSONParser::getJSONArray(const std::string& text, uint32_t& pos, JSON& target)
   {
      target = JSON::Array();

      while(true)
      {
         JSON temp;

         uint32_t last_pos = pos;
         auto status = getJSONElement(text, pos, temp);
         if(!status.ok())
         {
            TextToken token;
            auto text_status = UTF8Tokenizator::getToken(text, last_pos, token);
            if(!text_status.ok())
            {
               return InvalidUTF8Byte(pos);
            }

            last_pos += token.value.size();

            if(token.type == TextToken::SPACE)
            {
               text_status = UTF8Tokenizator::getToken(text, last_pos, token);
               if(!text_status.ok())
               {
                  return InvalidUTF8Byte(pos);
               }
            }

            if(token.value[0] == SQUARE_BRACKET_CLOSE)
            {
               ++pos;
               break;
            }

            return status;
         }

         target.pushBack(std::move(temp));

         TextToken token;
         auto text_status = UTF8Tokenizator::getToken(text, pos, token);
         if(!text_status.ok())
         {
            return InvalidUTF8Byte(pos);
         }

         if(token.value[0] == COMMA)
         {
            pos += 1;
            continue;
         }

         if(token.value[0] == SQUARE_BRACKET_CLOSE)
         {
            pos += 1;
            break;
         }
      }

      return JSONParseStatus();
   }

   JSONParseStatus JSONParser::getJSONObject(const std::string& text, uint32_t& pos, JSON& target)
   {
      target = JSON::Object();

      while(true)
      {
         JSON key;
         JSON value;

         uint32_t last_pos = pos;
         auto status = getJSONElement(text, pos, key);
         if(!status.ok())
         {
            TextToken token;
            auto text_status = UTF8Tokenizator::getToken(text, last_pos, token);
            if(!text_status.ok())
            {
               return InvalidUTF8Byte(pos);
            }

            last_pos += token.value.size();

            if(token.type == TextToken::SPACE)
            {
               text_status = UTF8Tokenizator::getToken(text, last_pos, token);
               if(!text_status.ok())
               {
                  return InvalidUTF8Byte(pos);
               }
            }

            if(token.value[0] == BRACKET_CLOSE)
            {
               ++pos;
               break;
            }

            return status;
         }

         TextToken token;
         auto text_status = UTF8Tokenizator::getToken(text, pos, token);
         if(!text_status.ok())
         {
            return InvalidUTF8Byte(pos);
         }

         if(token.value[0] != COLON)
         {
            return UnexpectedToken(token.value, pos);
         }
         ++pos;

         last_pos = pos;
         status = getJSONElement(text, pos, value);
         if(!status.ok())
         {
            TextToken token;
            auto text_status = UTF8Tokenizator::getToken(text, last_pos, token);
            if(!text_status.ok())
            {
               return InvalidUTF8Byte(pos);
            }

            if(token.value[0] == BRACKET_CLOSE)
            {
               ++pos;
               break;
            }

            return status;
         }

         target[key.get<std::string>()] = std::move(value);

         text_status = UTF8Tokenizator::getToken(text, pos, token);
         if(!text_status.ok())
         {
            return InvalidUTF8Byte(pos);
         }

         if(token.value[0] == COMMA)
         {
            pos += 1;
            continue;
         }

         if(token.value[0] == BRACKET_CLOSE)
         {
            pos += 1;
            break;
         }
      }

      return JSONParseStatus();
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

   uint32_t JSON::size() const
   {
      if(type != ARRAY)
      {
         throw JSONBadType::Array();
      }

      return getArray().size();
   }

   JSONParseStatus JSON::parse(const std::string& text)
   {
      return JSONParser::parse(text, *this);
   }

   JSONParseStatus JSON::parseFromFile(const std::string& filename)
   {
      return JSONParser::parseFromFile(filename, *this);
   }

   bool JSON::saveToFile(const std::string& filename) const
   {
      std::ofstream file(filename, std::ios::binary);
      if(!file.is_open())
      {
         return false;
      }

      const std::string text = toString();
      for(uint32_t i = 0; i < text.size(); ++i)
      {
         char c = text[i];
         file.write(&c, sizeof(char));
      }

      return true;
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
      {
         if(type != ARRAY || position == end())
         {
            return end();
         }

         delete position.json;

         return iterator(getArray().erase(position.array_it));
      }

      case OBJECT:
      {
         if(type != OBJECT || position == end())
         {
            return end();
         }

         return iterator(getObject().erase(position.object_it));
      }

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

      auto& arr = getArray();

      if(position >= arr.size())
      {
         throw JSONOutOfRange::Array(position);
      }

      return *arr[position];
   }

   const JSON& JSON::operator[](uint32_t position) const
   {
      if(type != ARRAY)
      {
         throw JSONBadType::Array();
      }

      auto& arr = getArray();

      if(position >= arr.size())
      {
         throw JSONOutOfRange::Array(position);
      }

      return *arr[position];
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

      auto& obj = getObject();
      auto found = obj.find(key);
      if(found == obj.end())
      {
         throw JSONOutOfRange::Object(key);
      }

      return found->second;
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

   std::string JSON::toString() const
   {
      std::string result;

      switch(type)
      {
      case OBJECT:
      {
         const std::string QM = "\"";

         result += "{";

         auto& obj = getObject();

         auto it = obj.begin();
         if(it != obj.end())
         {
            result += QM + it->first + QM + ": " + it->second.toString();
            ++it;
         }
         while(it != obj.end())
         {
            result += ", " + QM + it->first + QM + ": " + it->second.toString();
            ++it;
         }

         result += "}";
         break;
      }

      case ARRAY:
      {
         result += "[";

         auto& arr = getArray();
         if(arr.size() > 0)
         {
            result += arr[0]->toString();
         }
         for(uint32_t i = 1; i < arr.size(); ++i)
         {
            result += ", " + arr[i]->toString();
         }

         result += "]";
         break;
      }

      case STRING:
      {
         result = "\"" + getString() + "\"";
         break;
      }

      case NUMBER:
      {
         result = StringUtils::toString(getNumber());
         break;
      }

      case BOOL_TYPE:
      {
         if(getBool()) result = "true";
         else result = "false";
         break;
      }

      case NULL_TYPE:
         result = "null";
         break;
      }

      return result;
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

      getObject()[key] = element;
   }

   void JSON::forceAddElement(const std::string& key, JSON&& element)
   {
      if(type != OBJECT)
      {
         throw JSONBadType::Object();
      }

      getObject().emplace(key, std::move(element));
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
