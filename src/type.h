#ifndef ECMASCRIPT_TYPE_H
#define ECMASCRIPT_TYPE_H

#include "types/boolean.h"
#include "types/null.h"
#include "types/number.h"
#include "types/object.h"
#include "types/string.h"
#include "types/undefined.h"

class Type {

  enum class Tag : std::size_t {
    UNDEFINED_VALUE = 0,
    NULL_VALUE      = 1,
    BOOLEAN_VALUE   = 2,
    NUMBER_VALUE    = 3,
    STRING_VALUE    = 4,
    OBJECT_VALUE    = 5,
    // REFERENCE_VALUE           = 6,
    // LIST_VALUE                = 7,
    // COMPLETION_VALUE          = 8,
    // PROPERTY_DESCRIPTOR_VALUE = 9,
    // PROPERTY_IDENTIFIER_VALUE = 10,
    // LEXICAL_ENVIRONMENT_VALUE = 11,
    // ENVIRONMENT_RECORD_VALUE  = 12,
  };

  struct Value {
    Tag tag;
    union {
      Undefined undefined_value;
      Null      null_value;
      Boolean   boolean_value;
      Number    number_value;
      String    string_value;
      Object    object_value;
      // Reference reference_value;
    };
    Value();
    Value(Undefined);
    Value(Null);
    explicit Value(Boolean);
    explicit Value(Number);
    Value(String);
    Value(Object);
    Value(const Value&);
    Value& operator=(const Value&);

    ~Value() {}
  } value;

public:
  Type();

  template <typename T>
  Type(const T& value) : value(value)
  {
  }

  bool IsUndefined() const;
  bool IsNull() const;
  bool IsBoolean() const;
  bool IsNumber() const;
  bool IsString() const;
  bool IsObject() const;
  bool IsReference() const;

  friend Type    ToPrimitive(const Type&, const char*);
  friend Boolean ToBoolean(const Type&);
  friend Number  ToNumber(const Type&);
  friend Number  ToInteger(const Type&);
  friend Number  ToInt32(const Type&);
  friend Number  ToUint32(const Type&);
  friend Number  ToUint16(const Type&);
  friend String  ToString(const Type&);
  friend Object  ToObject(const Type&);
  friend Type    CheckObjectCoercible(const Type&);
  friend bool    IsCallable(const Type&);
  friend bool    SameValue(const Type&, const Type&);
  friend bool    AbstractEqualityComparison(const Type&, const Type&);
  friend bool    StrictEqualityComparison(const Type&, const Type&);
};

Type ToPrimitive(const Type&, const char* = nullptr);

Boolean ToBoolean(const Type&);
Number  ToNumber(const Type&);
Number  ToInteger(const Type&);
Number  ToInt32(const Type&);
Number  ToUint32(const Type&);
Number  ToUint16(const Type&);
String  ToString(const Type&);
Object  ToObject(const Type&);

Type CheckObjectCoercible(const Type&);
bool IsCallable(const Type&);
bool SameValue(const Type&, const Type&);

bool AbstractEqualityComparison(const Type&, const Type&);
bool StrictEqualityComparison(const Type&, const Type&);

bool operator==(const Type&, const Type&);

#endif