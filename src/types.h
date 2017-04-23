#include <cmath>

#include <string>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

struct Undefined {};
struct Null {};
struct Boolean { bool value; };
struct String { std::u16string value; };
struct Number
{
  double value;
  bool isNaN() const { return std::isnan(value); }
  bool isZero() const { return std::fpclassify(value) == FP_ZERO; }
  bool isInfinity() const { return std::fpclassify(value) == FP_INFINITE; }
};

struct Object;

// struct ReferenceSpecification { };
// struct ListSpecification { };
// struct CompletionSpecification { };
// struct PropertyDescriptorSpecification { };
// struct PropertyIdentifierSpecification {};
// struct LexicalEnvironment {};
// struct EnvironmentRecordSpecification {};

using Type = boost::variant<
  Undefined,
  Null,
  Boolean,
  String,
  Number,
  boost::recursive_wrapper<Object>
>;

struct NamedDataProperty
{
  Type value;
  bool writable = false;
  bool enumerable = false;
  bool configurable = false;
};

struct NamedAccessorProperty
{
  boost::optional<Object> get;
  boost::optional<Object> set;
  bool enumerable = false;
  bool configurable = false;
}

struct Object { };

Type GetBase(Type key);
Type PutValue(Type key, Type value);
Type GetReferencedName()

Type ToPrimitive(Type input, boost::optional<Type> preferred_type);
Boolean ToBoolean(Type input);
Number ToNumber(Type input);
Number ToInteger(Type input);
Number ToInt32(Type input);
Number ToUint32(Type input);
Number ToUint16(Type input);
String ToString(Type input);
Object ToObject(Type input);
Type CheckObjectCoercible(Type input);
Boolean IsCallable(Type input);
Boolean SameValue(Type input);

