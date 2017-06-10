#ifndef ECMASCRIPT_RUNTIME_H
#define ECMASCRIPT_RUNTIME_H

#include "type.h"

#include <memory>
#include <unordered_map>

namespace std {
template <>
struct hash<String> : hash<std::u16string> {
};
//   std::size_t operator()(const String& key) const
//   {
//     return std::hash<std::u16string>{}(key);
//   }
// };
}

struct Binding {
  Type value;
  bool is_deletable   = true;
  bool is_mutable     = true;
  bool is_initialized = false;
};

struct Reference {
  Type    base;
  String  name;
  Boolean strict;
};

struct EnvironmentRecord {
  using StringRef = const String&;

  virtual ~EnvironmentRecord() = default;

  virtual Boolean HasBinding(const String& N) const = 0;
  virtual void CreateMutableBinding(const String&, const Boolean&) = 0;
  virtual void SetMutableBinding(StringRef, const Type&, const Boolean&) = 0;
  virtual Type GetBindingValue(const String& N, const Boolean& S) const = 0;
  virtual Boolean DeleteBinding(const String& N) = 0;
  virtual Type ImplicitThisValue() const         = 0;
};

struct DeclarativeEnvironmentRecord : EnvironmentRecord {
  Boolean HasBinding(const String& N) const final;
  void CreateMutableBinding(const String& N, const Boolean& D) final;
  void SetMutableBinding(StringRef, const Type&, const Boolean&) final;
  Type GetBindingValue(const String&, const Boolean& S) const final;
  Boolean DeleteBinding(const String&) final;
  Type    ImplicitThisValue() const final;

  void CreateImmutableBinding(const String&);
  void InitializeImmutableBinding(const String&, const Type&);

private:
  std::unordered_map<String, Binding> bindings;
};

struct ObjectEnvironmentRecord : EnvironmentRecord {

  ObjectEnvironmentRecord(const Object& binding) : binding(binding) {}

  Boolean HasBinding(const String& N) const final;
  void    CreateMutableBinding(const String&, const Boolean&) final;
  void    SetMutableBinding(const String&, const Type&, const Boolean&) final;
  Type    GetBindingValue(const String&, const Boolean&) const final;
  Boolean DeleteBinding(const String&) final;
  Type    ImplicitThisValue() const final;

private:
  bool   provideThis = false;
  Object binding;
};

struct LexicalEnvironment {
  std::unique_ptr<EnvironmentRecord> envRec;
  LexicalEnvironment*                outer;
};

Reference
GetIdentifierReference(LexicalEnvironment*, const String&, const Boolean&);

std::shared_ptr<LexicalEnvironment>
NewDeclarativeEnvironment(LexicalEnvironment*);

std::shared_ptr<LexicalEnvironment>
NewObjectEnvironment(const Object&, LexicalEnvironment*);

#endif