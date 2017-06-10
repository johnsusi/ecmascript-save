#include "runtime.h"

// // 10.2.1.1.1 HasBinding(N)
// Boolean DeclarativeEnvironmentRecord::HasBinding(const String& N) const
// // The concrete environment record method HasBinding for declarative
// environment
// // records simply determines if the argument identifier is one of the
// // identifiers bound by the record:
// {
//   // 1. Let envRec be the declarative environment record for which the method
//   // was invoked.
//   auto envRec = this;
//   // 2. If envRec has a binding for the name that is the value of N, return
//   // true.
//   auto it = envRec->bindings.find(N);
//   if (it != envRec->bindings.end()) return true;
//   // 3. If it does not have such a binding, return false.
//   return false;
// }

// // 10.2.1.1.2 CreateMutableBinding (N, D)
// void DeclarativeEnvironmentRecord::CreateMutableBinding(
//     const String& N, const Boolean& D)
// // The concrete Environment Record method CreateMutableBinding for
// declarative
// // environment records creates a new mutable binding for the name N that is
// // initialised to the value undefined. A binding must not already exist in
// this
// // Environment Record for N. If Boolean argument D is provided and has the
// value
// // true the new binding is marked as being subject to deletion.
// {
//   // 1. Let envRec be the declarative environment record for which the method
//   // was invoked.
//   auto envRec = this;
//   // 2. Assert: envRec does not already have a binding for N.
//   auto it = envRec->bindings.find(N);
//   if (it != envRec->bindings.end())
//     ; // TODO assert

//   // 3. Create a mutable binding in envRec for N and set its bound value to
//   // undefined. If D is true record that the newly created binding may be
//   // deleted by a subsequent DeleteBinding call.
//   envRec->bindings.insert({N, Binding{Undefined{}, D, true, true}});
// }

// // 10.2.1.1.3 SetMutableBinding (N,V,S)
// void DeclarativeEnvironmentRecord::SetMutableBinding(
//     const String& N, const Type& V, const Boolean& S)
// // The concrete Environment Record method SetMutableBinding for declarative
// // environment records attempts to change the bound value of the current
// // binding of the identifier whose name is the value of the argument N to
// // the value of argument V. A binding for N must already exist. If the
// // binding is an immutable binding, a TypeError is thrown if S is true.
// {
//   // 1. Let envRec be the declarative environment record for which the method
//   // was invoked.
//   auto envRec = this;

//   // 2. Assert: envRec must have a binding for N.
//   auto it = envRec->bindings.find(N);
//   // TODO assert(it != envRec.bindings.end());

//   // 3. If the binding for N in envRec is a mutable binding, change its bound
//   // value to V.
//   if (it->second.is_mutable) it->second.value = V;

//   // 4. Else this must be an attempt to change the value of an immutable
//   binding
//   // so if S if true throw a TypeError exception.
//   // TODO throw TypeError
// }

// // 10.2.1.1.4 GetBindingValue(N,S)
// Type DeclarativeEnvironmentRecord::GetBindingValue(
//     const String& N, const Boolean& S) const
// // The concrete Environment Record method GetBindingValue for declarative
// // environment records simply returns the value of its bound identifier whose
// // name is the value of the argument N. The binding must already exist. If S
// is
// // true and the binding is an uninitialised immutable binding throw a
// // ReferenceError exception.
// {
//   // 1. Let envRec be the declarative environment record for which the method
//   // was invoked.
//   auto envRec = this;

//   // 2. Assert: envRec has a binding for N.
//   auto it = envRec->bindings.find(N);
//   // TODO: assert(it != envRec.bindings.end());

//   // 3. If the binding for N in envRec is an uninitialised immutable binding,
//   // then
//   // a. If S is false, return the value undefined, otherwise throw a
//   // ReferenceError exception.
//   if (!it->second.is_mutable && it->second.value == Undefined{}) {
//     if (!S) {
//       return Undefined{};
//     }
//     else {
//       // Throw ReferenceError
//     }
//   }

//   // 4. Else, return the value currently bound to N in envRec.
//   return it->second.value;
// }

// // 10.2.1.1.5 DeleteBinding (N)
// Boolean DeclarativeEnvironmentRecord::DeleteBinding(const String& N)
// // The concrete Environment Record method DeleteBinding for declarative
// // environment records can only delete bindings that have been explicitly
// // designated as being subject to deletion.
// {
//   // 1. Let envRec be the declarative environment record for which the method
//   // was invoked.
//   auto envRec = this;

//   // 2. If envRec does not have a binding for the name that is the value of
//   N,
//   // return true.
//   auto it = envRec->bindings.find(N);
//   if (it == envRec->bindings.end()) return true;

//   // 3. If the binding for N in envRec is cannot be deleted, return false.
//   if (!it->second.is_deletable) return false;

//   // 4. Remove the binding for N from envRec.
//   envRec->bindings.erase(it);

//   // 5. Return true.
//   return true;
// }

// // 10.2.1.1.6 ImplicitThisValue()
// Type DeclarativeEnvironmentRecord::ImplicitThisValue() const
// // Declarative Environment Records always return undefined as their
// // ImplicitThisValue.
// {
//   return Undefined{};
// }

// // 10.2.1.1.7 CreateImmutableBinding (N)
// void DeclarativeEnvironmentRecord::CreateImmutableBinding(const String& N)
// // The concrete Environment Record method CreateImmutableBinding for
// declarative
// // environment records creates a new immutable binding for the name N that is
// // initialised to the value undefined. A binding must not already exist in
// this
// // environment record for N.
// {
//   // 1. Let envRec be the declarative environment record for which the method
//   // was invoked.
//   auto envRec = this;

//   // 2. Assert: envRec does not already have a binding for N.
//   auto it = envRec->bindings.find(N);
//   // assert (it == envRec->bindings.end());

//   // 3. Create an immutable binding in envRec for N and record that it is
//   // uninitialised.
//   envRec->bindings[N] = Binding{Undefined{}, false, false, false};
// }

// // 10.2.1.1.8 InitializeImmutableBinding (N,V)
// void DeclarativeEnvironmentRecord::InitializeImmutableBinding(
//     const String& N, const Type& V)
// // The concrete Environment Record method InitializeImmutableBinding for
// // declarative environment records is used to set the bound value of the
// current
// // binding of the identifier whose name is the value of the argument N to the
// // value of argument V. An uninitialised immutable binding for N must already
// // exist.
// {
//   // 1. Let envRec be the declarative environment record for which the method
//   // was invoked.
//   auto envRec = this;

//   // 2. Assert: envRec must have an uninitialised immutable binding for N.
//   auto it = envRec->bindings.find(N);
//   // assert(it != envRec->bindings.end() && !it->is_initialized);

//   // 3. Set the bound value for N in envRec to V.
//   it->second.value = V;

//   // 4. Record that the immutable binding for N in envRec has been
//   initialised.
//   it->second.is_initialized = true;
// }

// // 10.2.1.2.1 HasBinding(N)
// Boolean ObjectEnvironmentRecord::HasBinding(const String& N) const
// // The concrete Environment Record method HasBinding for object environment
// // records determines if its associated binding object has a property whose
// name
// // is the value of the argument N:
// {

//   // 1. Let envRec be the object environment record for which the method was
//   // invoked.
//   auto envRec = this;

//   // 2. Let bindings be the binding object for envRec.
//   auto bindings = envRec->binding;

//   // 3. Return the result of calling the [[HasProperty]] internal method of
//   // bindings, passing N as the property name.

//   return bindings.HasProperty(N);
// }

// // 10.2.1.2.2 CreateMutableBinding (N, D)
// void ObjectEnvironmentRecord::CreateMutableBinding(
//     const String& N, const Boolean& D)
// // The concrete Environment Record method CreateMutableBinding for object
// // environment records creates in an environment record‘s associated binding
// // object a property whose name is the String value and initialises it to the
// // value undefined. A property named N must not already exist in the binding
// // object. If Boolean argument D is provided and has the value true the new
// // property‘s [[Configurable]] attribute is set to true, otherwise it is set
// to
// // false.
// {
//   // 1. Let envRec be the object environment record for which the method was
//   // invoked.
//   auto envRec = this;

//   // 2. Let bindings be the binding object for envRec.
//   auto bindings = envRec->binding;

//   // 3. Assert: The result of calling the [[HasProperty]] internal method of
//   // bindings, passing N as the property name, is false.

//   // assert(bindings.HasProperty(N) == false);

//   // 4. If D is true then let configValue be true otherwise let configValue
//   be
//   // false.
//   auto configValue = D ? true : false;

//   // 5. Call the [[DefineOwnProperty]] internal method of bindings, passing
//   N,
//   // Property Descriptor {[[Value]]:undefined, [[Writable]]: true,
//   // [[Enumerable]]: true , [[Configurable]]: configValue}, and true as
//   // arguments.

//   // bindings.DefineOwnProperty(N, PropertyDescriptor {
//   //   Undefined{}, true,
//   // });
// }

// // 10.2.1.2.3 SetMutableBinding (N,V,S)
// void ObjectEnvironmentRecord::SetMutableBinding(
//     const String& N, const Type& V, const Boolean& S)
// // The concrete Environment Record method SetMutableBinding for object
// // environment records attempts to set the value of the environment record‘s
// // associated binding object‘s property whose name is the value of the
// argument
// // N to the value of argument V. A property named N should already exist but
// if
// // it does not or is not currently writable, error handling is determined by
// the
// // value of the Boolean argument S.
// {
//   // 1. Let envRec be the object environment record for which the method was
//   // invoked.
//   auto envRec = this;

//   // 2. Let bindings be the binding object for envRec.
//   auto bindings = envRec->binding;

//   // 3. Call the [[Put]] internal method of bindings with arguments N, V, and
//   S.
//   bindings.Put(N, V, S);
// }

// // 10.2.1.2.4 GetBindingValue(N,S)
// Type ObjectEnvironmentRecord::GetBindingValue(
//     const String& N, const Boolean& S) const
// // The concrete Environment Record method GetBindingValue for object
// environment
// // records returns the value of its associated binding object‘s property
// whose
// // name is the String value of the argument identifier N. The property should
// // already exist but if it does not the result depends upon the value of the
// S
// // argument:
// {
//   // 1. Let envRec be the object environment record for which the method was
//   // invoked.
//   auto envRec = this;

//   // 2. Let bindings be the binding object for envRec.
//   auto bindings = envRec->binding;

//   // 3. Let value be the result of calling the [[HasProperty]] internal
//   method
//   // of bindings, passing N as the property name.
//   auto value = bindings.HasProperty(N);

//   // 4. If value is false, then
//   if (!value) {
//     // a. If S is false, return the value undefined,
//     if (!S) return Undefined{};
//     // otherwise throw a ReferenceError exception.
//     // else throw ReferenceError
//   }
//   // 5. Return the result of calling the [[Get]] internal method of bindings,
//   // passing N for the argument.
//   return bindings.Get(N);
// }

// // 10.2.1.2.5 DeleteBinding (N)
// Boolean ObjectEnvironmentRecord::DeleteBinding(const String& N)
// // The concrete Environment Record method DeleteBinding for object
// environment
// // records can only delete bindings that correspond to properties of the
// // environment object whose [[Configurable]] attribute have the value true.
// {
//   // 1. Let envRec be the object environment record for which the method was
//   // invoked.
//   auto envRec = this;

//   // 2. Let bindings be the binding object for envRec.
//   auto bindings = envRec->binding;

//   // 3. Return the result of calling the [[Delete]] internal method of
//   bindings,
//   // passing N and false as arguments.
//   return bindings.Delete(N, false);
// }

// // 10.2.1.2.6 ImplicitThisValue()
// Type ObjectEnvironmentRecord::ImplicitThisValue() const
// // Object Environment Records return undefined as their ImplicitThisValue
// unless
// // their provideThis flag is true.
// {
//   // 1. Let envRec be the object environment record for which the method was
//   // invoked.
//   auto envRec = this;

//   // 2. If the provideThis flag of envRec is true, return the binding object
//   for
//   // envRec.
//   if (envRec->provideThis) return envRec->binding;

//   // 3. Otherwise, return undefined.
//   return Undefined();
// }

// // 10.2.2.1 GetIdentifierReference (lex, name, strict)
// Reference GetIdentifierReference(
//     LexicalEnvironment* lex, const String& name, const Boolean& strict)
// // The abstract operation GetIdentifierReference is called with a Lexical
// // Environment lex, an identifier String name, and a Boolean flag strict. The
// // value of lex may be null. When called, the following steps are performed:
// {
//   // 1. If lex is the value null, then
//   if (!lex) {
//     // a. Return a value of type Reference whose base value is undefined,
//     whose
//     // referenced name is name, and whose strict mode flag is strict.
//     return {Undefined{}, name, strict};
//   }
//   // 2. Let envRec be lex‘s environment record.
//   auto& envRec = lex->envRec;

//   // 3. Let exists be the result of calling the HasBinding(N) concrete method
//   of
//   // envRec passing name as the argument N.
//   auto exists = envRec->HasBinding(name);

//   // 4. If exists is true, then
//   if (exists) {
//     // a. Return a value of type Reference whose base value is envRec, whose
//     // referenced name is name, and whose strict mode flag is strict.
//     return {envRec, name, strict};
//   }
//   // 5. Else
//   // a. Let outer be the value of lex’s outer environment reference.
//   auto outer = lex->outer;
//   // b. Return the result of calling GetIdentifierReference passing outer,
//   name,
//   // and strict as arguments.
//   return GetIdentifierReference(outer, name, strict);
// }

// // 10.2.2.2 NewDeclarativeEnvironment(LexicalEnvironment* E)
// std::shared_ptr<LexicalEnvironment>
// NewDeclarativeEnvironment(LexicalEnvironment* E)
// // When the abstract operation NewDeclarativeEnvironment is called with
// either
// // a
// // Lexical Environment or null as argument E the following steps are
// // performed:
// {
//   // 1. Let env be a new Lexical Environment.
//   auto env = std::make_shared<LexicalEnvironment>();

//   // 2. Let envRec be a new declarative environment record containing no
//   // bindings.
//   auto envRec = std::make_unique<DeclarativeEnvironmentRecord>();

//   // 3. Set env’s environment record to be envRec.
//   env->envRec = std::move(envRec);

//   // 4. Set the outer lexical environment reference of env to E.
//   env->outer = E;

//   // 5. Return env.
//   return std::move(env);
// }

// // 10.2.2.3 NewObjectEnvironment (O, E)
// std::shared_ptr<LexicalEnvironment>
// NewObjectEnvironment(const Object& O, LexicalEnvironment* E)
// // When the abstract operation NewObjectEnvironment is called with an Object
// O
// // and a Lexical Environment E (or null) as arguments, the following steps
// are
// // performed:
// {
//   // 1. Let env be a new Lexical Environment.
//   auto env = std::make_unique<LexicalEnvironment>();

//   // 2. Let envRec be a new object environment record containing O as the
//   // binding object.

//   auto envRec = std::make_unique<ObjectEnvironmentRecord>(O);

//   // 3. Set env’s environment record to be envRec.
//   env->envRec = std::move(envRec);

//   // 4. Set the outer lexical environment reference of env to E.
//   env->outer = E;

//   // 5. Return env.
//   return std::move(env);
// }
