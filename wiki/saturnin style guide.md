# Saturnin Style Guide

Loosely based on [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) and [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md)


- [General](#general)
- [Interfaces](#interfaces)
- [Functions](#functions)
- [Classes and classes hierarchies](#classes-and-classes-hierarchies)
- [Enumerations](#enumerations)
- [Resource management](#resource-management)
- [Expressions & statements](#expressions--statements)
- [Performance](#performance)
- [Concurrency & parallelism](#concurrency--parallelism)
- [Error handling](#error-handling)
- [Constants & immutability](#constants--immutability)
- [Templates & generic programming](#templates--generic-programming)
- [C-style programming](#c-style-programming)
- [Standard library](#standard-library)
- [Non-Rules and myths](#non-rules-and-myths)
- [Naming and layout rules](#naming-and-layout-rules)
- [Namespaces](#namespaces)
- [Comments](#comments)
- [Header files](#header-files)
- [Other C++ features](#other-c-features)


# Core (WIP)
## General
- Express ideas directly in code [[P.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#p1-express-ideas-directly-in-code)]
- Prefer compile-time checking to run-time checking [[P.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#p5-prefer-compile-time-checking-to-run-time-checking)]
- What cannot be checked at compile time should be checkable at run time [[P.6](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#p6-what-cannot-be-checked-at-compile-time-should-be-checkable-at-run-time)]
- Catch run-time errors early [[P.7](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#p7-catch-run-time-errors-early)]
- Don't leak any resources [[P.8](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#p8-dont-leak-any-resources)]
- Prefer immutable data to mutable data [[P.10](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#p10-prefer-immutable-data-to-mutable-data)]
- Encapsulate messy constructs, rather than spreading through the code [[P.11](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#p11-encapsulate-messy-constructs-rather-than-spreading-through-the-code)]
- Use support libraries as appropriate [[P.13](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#p13-use-support-libraries-as-appropriate)]
--------------

## Interfaces
- Make interfaces explicit [[I.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i1-make-interfaces-explicit)]
- Avoid non-const global variables [[I.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i2-avoid-non-const-global-variables)]
- Avoid singletons [[I.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i3-avoid-singletons)]
- Make interfaces precisely and strongly typed [[I.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i4-make-interfaces-precisely-and-strongly-typed)]
- Prefer Expects() for expressing preconditions [[I.6](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i6-prefer-expects-for-expressing-preconditions)]
- Prefer Ensures() for expressing postconditions [[I.8](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i8-prefer-ensures-for-expressing-postconditions)]
- Use exceptions to signal a failure to perform a required task [[I.10](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i10-use-exceptions-to-signal-a-failure-to-perform-a-required-task)]
- Never transfer ownership by a raw pointer (T*) or reference (T&) [[I.11](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i11-never-transfer-ownership-by-a-raw-pointer-t-or-reference-t)]
- Avoid complex initialization of global objects [[I.22](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i22-avoid-complex-initialization-of-global-objects)]
- Keep the number of function arguments low [[I.23](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i23-keep-the-number-of-function-arguments-low)]
- Prefer abstract classes as interfaces to class hierarchies [[I.25](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i25-prefer-abstract-classes-as-interfaces-to-class-hierarchies)]
- Encapsulate rule violations [[I.30](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#i30-encapsulate-rule-violations)]
---------------

## Functions
- "Package" meaningful operations as carefully named functions [[F.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f1-package-meaningful-operations-as-carefully-named-functions)]
- A function should perform a single logical operation [[F.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f2-a-function-should-perform-a-single-logical-operation)]
- Keep functions short and simple [[F.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f3-keep-functions-short-and-simple)]
- If a function may have to be evaluated at compile time, declare it constexpr [[F.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f4-if-a-function-may-have-to-be-evaluated-at-compile-time-declare-it-constexpr)]
- If a function is very small and time-critical, declare it inline [[F.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f5-if-a-function-is-very-small-and-time-critical-declare-it-inline)]
- If your function may not throw, declare it noexcept [[F.6](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f6-if-your-function-may-not-throw-declare-it-noexcept)]
- For general use, take T* or T& arguments rather than smart pointers [[F.7](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f7-for-general-use-take-t-or-t-arguments-rather-than-smart-pointers)]
- Unused parameters should be unnamed [[F.9](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f9-unused-parameters-should-be-unnamed)]
- When defining a function, parameter order is: inputs, then outputs.
- For "in" parameters, pass cheaply-copied types by value and others by reference to const [[F.16](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f16-for-in-parameters-pass-cheaply-copied-types-by-value-and-others-by-reference-to-const)]
- For "in-out" parameters, pass by reference to non-const [[F.17](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f17-for-in-out-parameters-pass-by-reference-to-non-const)]
- For "out" output values, prefer return values to output parameters [[F.20](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f20-for-out-output-values-prefer-return-values-to-output-parameters)]
- To return multiple "out" values, prefer returning a tuple or struct [[F.21](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f21-to-return-multiple-out-values-prefer-returning-a-tuple-or-struct)]
- Use a unique_ptr<T> to transfer ownership where a pointer is needed [[F.26](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f26-use-a-unique_ptrt-to-transfer-ownership-where-a-pointer-is-needed)]
- Use a shared_ptr<T> to share ownership [[F.27](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f27-use-a-shared_ptrt-to-share-ownership)]
- Never (directly or indirectly) return a pointer or a reference to a local object [[F.43](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f43-never-directly-or-indirectly-return-a-pointer-or-a-reference-to-a-local-object)]
- Return a T& when copy is undesirable and "returning no object" isn't needed [[F.44](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f44-return-a-t-when-copy-is-undesirable-and-returning-no-object-isnt-needed)]
- Don't return a T&& [[F.45](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f45-dont-return-a-t)]
- int is the return type for main() [[F.46](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f46-int-is-the-return-type-for-main)]
- Use a lambda when a function won't do (to capture local variables, or to write a local function) [[F.50
](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f50-use-a-lambda-when-a-function-wont-do-to-capture-local-variables-or-to-write-a-local-function)]
- Where there is a choice, prefer default arguments over overloading [[F.51](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f51-where-there-is-a-choice-prefer-default-arguments-over-overloading)]
- Prefer capturing by reference in lambdas that will be used locally, including passed to algorithms [[F.52](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f52-prefer-capturing-by-reference-in-lambdas-that-will-be-used-locally-including-passed-to-algorithms)]
 - Avoid capturing by reference in lambdas that will be used nonlocally, including returned, stored on the heap, or passed to another thread [[F.53](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f53-avoid-capturing-by-reference-in-lambdas-that-will-be-used-nonlocally-including-returned-stored-on-the-heap-or-passed-to-another-thread)]
- Don't use va_arg arguments [[F.55](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#f55-dont-use-va_arg-arguments)]
- When noexcept ?
If your function recursively meets the following criteria:
    * Doesn't explicitly throw an exception
    * Only constructs trivial types or types with noexcept constructors
    * Calls only noexcept or C functions (careful with operators!)
    * Will likely stay as such in the future
---------------

## Classes and classes hierarchies
- Organize related data into structures (structs or classes) [[C.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c1-organize-related-data-into-structures-structs-or-classes)]
- Use a struct only for passive objects that carry data; everything else is a class.
- Make data members private, unless they are static const.
- Declaration order:  
    Your class definition should start with its public: section, followed by its protected: section and then its private: section. If any of these sections are empty, omit them.
    Within each section, the declarations generally should be in the following order:

            Using-declarations, Typedefs and Enums
            Constants (static const data members)
            Constructors
            Destructor
            Methods, including static methods
            Data Members (except static const data members)

- Make a function a member only if it needs direct access to the representation of a class [[C.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c4-make-a-function-a-member-only-if-it-needs-direct-access-to-the-representation-of-a-class)]
- Place helper functions in the same namespace as the class they support [[C.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c5-place-helper-functions-in-the-same-namespace-as-the-class-they-support)]
- Don't define a class or enum and declare a variable of its type in the same statement [[C.7](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c7-dont-define-a-class-or-enum-and-declare-a-variable-of-its-type-in-the-same-statement)]
- Use class rather than struct if any member is non-public [[C.8](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c8-use-class-rather-than-struct-if-any-member-is-non-public)]
- Minimize exposure of members [[C.9](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c9-minimize-exposure-of-members)]
- If you can avoid defining default operations, do [[C.20](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c20-if-you-can-avoid-defining-default-operations-do)]
- Support copying and/or moving if it makes sense for your type. Otherwise, disable the implicitly generated special functions that perform copies and moves. [[more](https://google.github.io/styleguide/cppguide.html#Copyable_Movable_Types)]
- If you define or =delete any default operation, define or =delete them all [[C.21](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all)]
- Make default operations consistent [[C.22](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c22-make-default-operations-consistent)]
- Define a destructor if a class needs an explicit action at object destruction [[C.30](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c30-define-a-destructor-if-a-class-needs-an-explicit-action-at-object-destruction)]
- All resources acquired by a class must be released by the class's destructor [[C.31](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c31-all-resources-acquired-by-a-class-must-be-released-by-the-classs-destructor)]
- A base class destructor should be either public and virtual, or protected and nonvirtual [[C.35](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c35-a-base-class-destructor-should-be-either-public-and-virtual-or-protected-and-nonvirtual)]
- A destructor may not fail [[C.36](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c36-a-destructor-may-not-fail)]
- Define a constructor if a class has an invariant [[C.40](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c40-define-a-constructor-if-a-class-has-an-invariant)]
- A constructor should create a fully initialized object [[C.41](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c41-a-constructor-should-create-a-fully-initialized-object)]
- Avoid virtual method calls in constructors, prefer using *Initialize()* method. 
- If a constructor cannot construct a valid object, throw an exception [[C.42](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c42-if-a-constructor-cannot-construct-a-valid-object-throw-an-exception)]
- Prefer default constructors to be simple and non-throwing [[C.44](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c44-prefer-default-constructors-to-be-simple-and-non-throwing)]
- Don't define a default constructor that only initializes data members; use in-class member initializers instead [[C.45](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c45-dont-define-a-default-constructor-that-only-initializes-data-members-use-in-class-member-initializers-instead)]
- By default, declare single-argument constructors explicit [[C.46](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c46-by-default-declare-single-argument-constructors-explicit)]
- Define and initialize member variables in the order of member declaration [[C.47](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c47-define-and-initialize-member-variables-in-the-order-of-member-declaration)]
- Prefer in-class initializers to member initializers in constructors for constant initializers [[C.48](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c48-prefer-in-class-initializers-to-member-initializers-in-constructors-for-constant-initializers)]
- Prefer initialization to assignment in constructors [[C.49](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c49-prefer-initialization-to-assignment-in-constructors)]
- Use delegating constructors to represent common actions for all constructors of a class [[C.51](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c51-use-delegating-constructors-to-represent-common-actions-for-all-constructors-of-a-class)]
- Make copy assignment non-virtual, take the parameter by const&, and return by non-const& [[C.60](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c60-make-copy-assignment-non-virtual-take-the-parameter-by-const-and-return-by-non-const)]
- Make copy assignment safe for self-assignment [[C.62](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c62-make-copy-assignment-safe-for-self-assignment)]
- Make move assignment non-virtual, take the parameter by &&, and return by non-const & [[C.63](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c63-make-move-assignment-non-virtual-take-the-parameter-by--and-return-by-non-const-)]
- A move operation should move and leave its source in a valid state [[C.64](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c64-a-move-operation-should-move-and-leave-its-source-in-a-valid-state)]
- Make move assignment safe for self-assignment [[C.65](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c65-make-move-assignment-safe-for-self-assignment)]
- Make move operations noexcept [[C.66](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c66-make-move-operations-noexcept)]
- A base class should suppress copying, and provide a virtual clone instead if "copying" is desired [[C.67](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c67-a-base-class-should-suppress-copying-and-provide-a-virtual-clone-instead-if-copying-is-desired)]
- Use =default if you have to be explicit about using the default semantics [[C.80](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c80-use-default-if-you-have-to-be-explicit-about-using-the-default-semantics)]
- Use =delete when you want to disable default behavior (without wanting an alternative) [[C.81](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c81-use-delete-when-you-want-to-disable-default-behavior-without-wanting-an-alternative)]
- Don't call virtual functions in constructors and destructors [[C.82](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c82-dont-call-virtual-functions-in-constructors-and-destructors)]
- A swap function may not fail [[C.84](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c84-a-swap-function-may-not-fail)]
- Make swap noexcept [[C.85](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c85-make-swap-noexcept)]
- Make == symmetric with respect to operand types and noexcept [[C.86](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c86-make--symmetric-with-respect-to-operand-types-and-noexcept)]
- All inheritance should be public. If you want to do private inheritance, you should be including an instance of the base class as a member instead. [[more](https://google.github.io/styleguide/cppguide.html#Inheritance)]
- Beware of == on base classes [[C.87](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c87-beware-of--on-base-classes)]
- If a base class is used as an interface, make it a pure abstract class [[C.121](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c121-if-a-base-class-is-used-as-an-interface-make-it-a-pure-abstract-class)]
- An abstract class typically doesn't need a constructor [[C.126](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c126-an-abstract-class-typically-doesnt-need-a-constructor)]
- A class with a virtual function should have a virtual or protected destructor [[C.127](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c127-a-class-with-a-virtual-function-should-have-a-virtual-or-protected-destructor)]
- Virtual functions should specify exactly one of virtual, override, or final [[C.128](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c128-virtual-functions-should-specify-exactly-one-of-virtual-override-or-final)]
- Redefine or prohibit copying for a base class; prefer a virtual clone function instead [[C.130](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c130-redefine-or-prohibit-copying-for-a-base-class-prefer-a-virtual-clone-function-instead)]
- Avoid trivial getters and setters [[C.131](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c131-avoid-trivial-getters-and-setters)]
- Don't make a function virtual without reason [[C.132](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c132-dont-make-a-function-virtual-without-reason)]
- Avoid protected data [[C.133](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c133-avoid-protected-data)]
- Use virtual bases to avoid overly general base classes [[C.137](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c137-use-virtual-bases-to-avoid-overly-general-base-classes)]
- Create an overload set for a derived class and its bases with using [[C.138](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c138-create-an-overload-set-for-a-derived-class-and-its-bases-with-using)]
- Use final sparingly [[C.139](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c139-use-final-sparingly)]
- Do not provide different default arguments for a virtual function and an overrider [[C.140](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c140-do-not-provide-different-default-arguments-for-a-virtual-function-and-an-overrider)]
- Access polymorphic objects through pointers and references [[C.145](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c145-access-polymorphic-objects-through-pointers-and-references)]
- Use dynamic_cast where class hierarchy navigation is unavoidable [[C.146](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c146-use-dynamic_cast-where-class-hierarchy-navigation-is-unavoidable)]
- Use unique_ptr or shared_ptr to avoid forgetting to delete objects created using new [[C.149](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c149-use-unique_ptr-or-shared_ptr-to-avoid-forgetting-to-delete-objects-created-using-new)]
- Use make_unique() to construct objects owned by unique_ptrs [[C.150](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c150-use-make_unique-to-construct-objects-owned-by-unique_ptrs)]
- Use make_shared() to construct objects owned by shared_ptrs [[C.151](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c151-use-make_shared-to-construct-objects-owned-by-shared_ptrs)]
- Prefer virtual function to casting [[C.153](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c153-prefer-virtual-function-to-casting)]
- Define operators primarily to mimic conventional usage [[C.160](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c160-define-operators-primarily-to-mimic-conventional-usage)]
- Use nonmember functions for symmetric operators [[C.161](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c161-use-nonmember-functions-for-symmetric-operators)]
- Overload only for operations that are roughly equivalent [[C.163](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c163-overload-only-for-operations-that-are-roughly-equivalent)]
- Use using for customization points [[C.165](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c165-use-using-for-customization-points)]
- Use unions to save memory [C.180](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c180-use-unions-to-save-memory)]
- Don't use a union for type punning [[C.183](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c183-dont-use-a-union-for-type-punning)]

---------------

## Enumerations
- Prefer enumerations over macros [[Enum.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#enum1-prefer-enumerations-over-macros)]
- Prefer class enums over "plain" enums [[Enum.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#enum3-prefer-class-enums-over-plain-enums)]
- Don't use ALL_CAPS for enumerators [[Enum.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#enum5-dont-use-all_caps-for-enumerators)]
- Avoid unnamed enumerations [[Enum.6](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#enum6-avoid-unnamed-enumerations)]
- Specify the underlying type of an enumeration only when necessary [[Enum.7](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#enum7-specify-the-underlying-type-of-an-enumeration-only-when-necessary)]
- Specify enumerator values only when necessary [[Enum.8](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#enum8-specify-enumerator-values-only-when-necessary)]
---------------

## Resource management
- Manage resources automatically using resource handles and RAII (Resource Acquisition Is Initialization) [[R.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r1-manage-resources-automatically-using-resource-handles-and-raii-resource-acquisition-is-initialization)]
- In interfaces, use raw pointers to denote individual objects (only)[[R.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r2-in-interfaces-use-raw-pointers-to-denote-individual-objects-only)]
- A raw pointer (a T*) is non-owning [[R.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r3-a-raw-pointer-a-t-is-non-owning)]
- A raw reference (a T&) is non-owning [[R.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r4-a-raw-reference-a-t-is-non-owning)]
- Prefer scoped objects, don't heap-allocate unnecessarily [[R.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r5-prefer-scoped-objects-dont-heap-allocate-unnecessarily)]
- Avoid non-const global variables [[R.6](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r6-avoid-non-const-global-variables)]
- Avoid malloc() and free() [[R.10](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r10-avoid-malloc-and-free)]
- Avoid calling new and delete explicitly [[R.10](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r11-avoid-calling-new-and-delete-explicitly)]
- Immediately give the result of an explicit resource allocation to a manager object [[R.12](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r12-immediately-give-the-result-of-an-explicit-resource-allocation-to-a-manager-object)]
- Perform at most one explicit resource allocation in a single expression statement [[R.13](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r13-perform-at-most-one-explicit-resource-allocation-in-a-single-expression-statement)]
- Use unique_ptr or shared_ptr to represent ownership [[R.20](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r20-use-unique_ptr-or-shared_ptr-to-represent-ownership)]
- Prefer unique_ptr over shared_ptr unless you need to share ownership [[R.21](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r21-prefer-unique_ptr-over-shared_ptr-unless-you-need-to-share-ownership)]
- Use make_shared() to make shared_ptrs [[R.22](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r22-use-make_shared-to-make-shared_ptrs)]
- Use make_unique() to make unique_ptrs [[R.23](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r23-use-make_unique-to-make-unique_ptrs)]
- Take smart pointers as parameters only to explicitly express lifetime semantics [[R.30](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r30-take-smart-pointers-as-parameters-only-to-explicitly-express-lifetime-semantics)]
- Take a unique_ptr<widget> parameter to express that a function assumes ownership of a widget [[R.32](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r32-take-a-unique_ptrwidget-parameter-to-express-that-a-function-assumes-ownership-of-a-widget)]
- Take a unique_ptr<widget>& parameter to express that a function reseats thewidget [[R.33](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r33-take-a-unique_ptrwidget-parameter-to-express-that-a-function-reseats-thewidget)]
- Take a shared_ptr<widget> parameter to express that a function is part owner [[R.34](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r34-take-a-shared_ptrwidget-parameter-to-express-that-a-function-is-part-owner)]
- Take a shared_ptr<widget>& parameter to express that a function might reseat the shared pointer [[R.35](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#r35-take-a-shared_ptrwidget-parameter-to-express-that-a-function-might-reseat-the-shared-pointer)]
---------------

## Expressions & statements
- General rules
    - Prefer the standard library to other libraries and to "handcrafted code" [[ES.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es1-prefer-the-standard-library-to-other-libraries-and-to-handcrafted-code)]
    - Prefer suitable abstractions to direct use of language features [[ES.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es2-prefer-suitable-abstractions-to-direct-use-of-language-features)]
- Declaration rules
    - Keep scopes small [[ES.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es5-keep-scopes-small)]
    - Declare names in for-statement initializers and conditions to limit scope [[ES.6](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es6-declare-names-in-for-statement-initializers-and-conditions-to-limit-scope)]
    - Keep common and local names short, and keep uncommon and nonlocal names longer [[ES.7](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es7-keep-common-and-local-names-short-and-keep-uncommon-and-nonlocal-names-longer)]
    - Avoid ALL_CAPS names [[ES.9](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es9-avoid-all_caps-names)]
    - Declare one name (only) per declaration [[ES.10](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es10-declare-one-name-only-per-declaration)]
    - Use auto to avoid redundant repetition of type names [ES.11](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es11-use-auto-to-avoid-redundant-repetition-of-type-names)]
    - Do not reuse names in nested scopes [[ES.12](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es12-do-not-reuse-names-in-nested-scopes)]
    - Always initialize an object [[ES.20](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es20-always-initialize-an-object)]
    - Don't introduce a variable (or constant) before you need to use it [[ES.21](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es21-dont-introduce-a-variable-or-constant-before-you-need-to-use-it)]
    - Don't declare a variable until you have a value to initialize it with [[ES.22](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es22-dont-declare-a-variable-until-you-have-a-value-to-initialize-it-with)]
    - Prefer the {} initializer syntax [[ES.23](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es23-prefer-the--initializer-syntax)]
    - Use a unique_ptr<T> to hold pointers [[ES.24](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es24-use-a-unique_ptrt-to-hold-pointers)]
    - Declare an object const or constexpr unless you want to modify its value later on [[ES.25](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es25-declare-an-object-const-or-constexpr-unless-you-want-to-modify-its-value-later-on)]
    - Don't use a variable for two unrelated purposes [[ES.26](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es26-dont-use-a-variable-for-two-unrelated-purposes)]
    - Use std::array or stack_array for arrays on the stack [[ES.27](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es27-use-stdarray-or-stack_array-for-arrays-on-the-stack)]
    - Use lambdas for complex initialization, especially of const variables [[ES.28](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es28-use-lambdas-for-complex-initialization-especially-of-const-variables)]
    - Don't use macros for program text manipulation [[ES.30](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es30-dont-use-macros-for-program-text-manipulation)]
    - Don't use macros for constants or "functions" [[ES.31](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es31-dont-use-macros-for-constants-or-functions)]
    - Use ALL_CAPS for all macro names [[ES.32](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es32-use-all_caps-for-all-macro-names)]
    - If you must use macros, give them unique names [[ES.33](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es33-if-you-must-use-macros-give-them-unique-names)]
    - Don't define a (C-style) variadic function [[ES.34](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#-es34-dont-define-a-c-style-variadic-function)]
- Expression rules
    - Avoid complicated expressions [[ES.40](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es40-avoid-complicated-expressions)]
    - If in doubt about operator precedence, parenthesize [[ES.41](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es41-if-in-doubt-about-operator-precedence-parenthesize)]
    - Keep use of pointers simple and straightforward [[ES.42](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es42-keep-use-of-pointers-simple-and-straightforward)]
    - Avoid expressions with undefined order of evaluation [[ES.43](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es43-avoid-expressions-with-undefined-order-of-evaluation)]
    - Don't depend on order of evaluation of function arguments [[ES.44](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es44-dont-depend-on-order-of-evaluation-of-function-arguments)]
    - Avoid "magic constants"; use symbolic constants [[ES.45](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es45-avoid-magic-constants-use-symbolic-constants)]
    - Avoid lossy (narrowing, truncating) arithmetic conversions [[ES.46](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es46-avoid-lossy-narrowing-truncating-arithmetic-conversions)]
    - Use nullptr rather than 0 or NULL [[ES.47](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es47-use-nullptr-rather-than-0-or-null)]
    - Avoid casts [[ES.48](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es48-avoid-casts)]
    - If you must use a cast, use a named cast [[ES.49](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es49-if-you-must-use-a-cast-use-a-named-cast)]
    - Don't cast away const [[ES.50](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es50-dont-cast-away-const)]
    - Avoid the need for range checking [[ES.55](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es55-avoid-the-need-for-range-checking)]
    - Write std::move() only when you need to explicitly move an object to another scope [[ES.56](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es56-write-stdmove-only-when-you-need-to-explicitly-move-an-object-to-another-scope)]
    - Avoid new and delete outside resource management functions [[ES.60](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es60-avoid-new-and-delete-outside-resource-management-functions)]
    - Delete arrays using delete[] and non-arrays using delete [[ES.61](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es61-delete-arrays-using-delete-and-non-arrays-using-delete)]
    - Don't compare pointers into different arrays [[ES.62](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es62-dont-compare-pointers-into-different-arrays)]
    - Don't slice [[ES.63](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es63-dont-slice)]
    - Use the T{e}notation for construction [[ES.64](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es64-use-the-tenotation-for-construction)]
    - Don't dereference an invalid pointer [[ES.65](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es65-dont-dereference-an-invalid-pointer)]
- Statement rules
    - Prefer a switch-statement to an if-statement when there is a choice [[ES.70](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es70-prefer-a-switch-statement-to-an-if-statement-when-there-is-a-choice)]
    - Prefer a range-for-statement to a for-statement when there is a choice [[ES.71](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es71-prefer-a-range-for-statement-to-a-for-statement-when-there-is-a-choice)]
    - Prefer a for-statement to a while-statement when there is an obvious loop variable [[ES.72](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es72-prefer-a-for-statement-to-a-while-statement-when-there-is-an-obvious-loop-variable)]
    - Prefer a while-statement to a for-statement when there is no obvious loop variable [[ES.73](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es73-prefer-a-while-statement-to-a-for-statement-when-there-is-no-obvious-loop-variable)]
    - Avoid do-statements [[ES.75](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es75-avoid-do-statements)]
    - Avoid goto [[ES.76](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es76-avoid-goto)]
    - Use default to handle common cases (only) [[ES.79](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es79-use-default-to-handle-common-cases-only)]
    - Avoid modifying loop control variables inside the body of raw for-loops [[ES.86](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es86-avoid-modifying-loop-control-variables-inside-the-body-of-raw-for-loops)]
    - Don't add redundant == or != to conditions [[ES.87](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es87-dont-add-redundant--or--to-conditions)]
- Arithmetic rules
    - Don't mix signed and unsigned arithmetic [[ES.100](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es100-dont-mix-signed-and-unsigned-arithmetic)]
    - Use unsigned types for bit manipulation [[ES.101](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es101-use-unsigned-types-for-bit-manipulation)]
    - Use signed types for arithmetic [[ES.102](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es102-use-signed-types-for-arithmetic)]
    - Don't overflow [[ES.103](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es103-dont-overflow)]
    - Don't underflow [[ES.104](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es104-dont-underflow)]
    - Don't divide by zero [[ES.105](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es105-dont-divide-by-zero)]
    - Don't try to avoid negative values by using unsigned [[ES.106](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es106-dont-try-to-avoid-negative-values-by-using-unsigned)]
---------------

## Performance
- Don't optimize without reason [[PEr.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per1-dont-optimize-without-reason)]
- Don't optimize prematurely [[Per.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per2-dont-optimize-prematurely)]
- Don't optimize something that's not performance critical [[Per.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per3-dont-optimize-something-thats-not-performance-critical)]
- Don't assume that complicated code is necessarily faster than simple code [[Per.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per4-dont-assume-that-complicated-code-is-necessarily-faster-than-simple-code)]
- Don't assume that low-level code is necessarily faster than high-level code [[Per.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per5-dont-assume-that-low-level-code-is-necessarily-faster-than-high-level-code)]
- Don't make claims about performance without measurements [[Per.6](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per6-dont-make-claims-about-performance-without-measurements)]
- Design to enable optimization [[Per.7](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per7-design-to-enable-optimization)]
- Rely on the static type system [[Per.10](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per10-rely-on-the-static-type-system)]
- Move computation from run time to compile time [[Per.11](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per11-move-computation-from-run-time-to-compile-time)]
- Do not allocate on a critical branch [[Per.15](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per15-do-not-allocate-on-a-critical-branch)]
- Use compact data structures [[Per.16](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per16-use-compact-data-structures)]
- Declare the most used member of a time-critical struct first [[Per.17](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per17-declare-the-most-used-member-of-a-time-critical-struct-first)]
- Access memory predictably [[Per.19](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#per19-access-memory-predictably)]
- Use prefix form of the increment (++i) and decrement (--i) operators.
- Temporary Objects
    * Rewriting expressions can reduce or eliminate the need for temporary objects. For example, if a, b, and 
c are objects of class Matrix: 
    
            #!C++
            Matrix a;     // inefficient: don't create an object before it is really needed; 
                          // default initialization can be expensive
            a = b + c;    // inefficient: (b + c) creates a temporary
                          // object and then assigns it to a
            Matrix a = b; // better: no default initialization
            a += c;       // better: no temporary objects created

    * Use the return value optimization to give the compiler a hint that temporary objects can be eliminated.

            #!C++
            const Rational operator * (Rational const & lhs, 
                           Rational const & rhs) 
            { 
                return Rational(lhs.numerator() * rhs.numerator(), lhs.denominator() * rhs.denominator()); 
            } 
Less carefully written code might create a local Rational variable to hold the result of the calculation, use the assignment operator to copy it to a temporary variable holding the return value, then copy that into a variable in the calling function. 

            #!C++
            // not this way ... 
            const Rational operator * (Rational const & lhs, Rational const & rhs) { 
                Rational tmp;   // calls the default constructor (if any)
                tmp.my_numerator   = lhs.numerator()   * rhs.numerator(); 
                tmp.my_denominator = lhs.denominator() * rhs.denominator(); 
                return tmp;     // copies tmp to the return value, which is 
                                // then copied into the receiving variable
            } 

    * Sometimes it is helpful to “widen” the interface for a class with functions that take different data types to prevent automatic conversions (such as adding an overload on char * to a function which takes an 
std::string parameter)

* **Function Inlining**
    * As a rule of thumb, functions consisting of only one or two lines are generally good candidates for inlining.
    * Inline functions can help performance, as inlining exposes more sequential code to the optimizer. Manual techniques, such as avoiding conditional code and unrolling short loops, also help the optimizer do a better job. 
    * Virtual functions should be used only when run-time polymorphic behavior is desired.  Not every function needs to be virtual and not every class should be designed to be a base class. 
    * Use function objects with the Standard Library algorithms rather than function pointers. Function pointers defeat the data flow analyzers of many optimizers, but function objects are passed by value and optimizers can easily handle inline functions used on objects.
---------------

## Concurrency & parallelism
- Assume that your code will run as part of a multi-threaded program [[CP.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp1-assume-that-your-code-will-run-as-part-of-a-multi-threaded-program)]
- Avoid data races [[CP.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp2-avoid-data-races)]
- Minimize explicit sharing of writable data [[CP.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp3-minimize-explicit-sharing-of-writable-data)]
- Think in terms of tasks, rather than threads [[CP.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp4-think-in-terms-of-tasks-rather-than-threads)]
- Don't try to use volatile for synchronization [[CP.8](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp8-dont-try-to-use-volatile-for-synchronization)]
- Whenever feasible use tools to validate your concurrent code [[CP.9](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp9-whenever-feasible-use-tools-to-validate-your-concurrent-code)]
- Use RAII, never plain lock()/unlock() [[CP.20](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp20-use-raii-never-plain-lockunlock)]
- Use std::lock() or std::scoped_lock to acquire multiple mutexes [[CP.21](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp21-use-stdlock-or-stdscoped_lock-to-acquire-multiple-mutexes)]
- Never call unknown code while holding a lock (e.g., a callback) [[CP.22](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp22-never-call-unknown-code-while-holding-a-lock-eg-a-callback)]
- Think of a thread as a global container [[CP.24](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp24-think-of-a-thread-as-a-global-container)]
- gsl::joining_thread over std::thread [[CP.25](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp25-prefer-gsljoining_thread-over-stdthread)]
- Don't detach() a thread [[CP.26](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp26-dont-detach-a-thread)]
- Pass small amounts of data between threads by value, rather than by reference or pointer [[CP.31](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp31-pass-small-amounts-of-data-between-threads-by-value-rather-than-by-reference-or-pointer)]
- To share ownership between unrelated threads use shared_ptr [[CP.32](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp32-to-share-ownership-between-unrelated-threads-use-shared_ptr)]
- Minimize context switching [[CP.40](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp40-minimize-context-switching)]
- Minimize thread creation and destruction [[CP.41](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp41-minimize-thread-creation-and-destruction)]
- Don't wait without a condition [[CP.42](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp42-dont-wait-without-a-condition)]
- Minimize time spent in a critical section [[CP.43](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp43-minimize-time-spent-in-a-critical-section)]
- Remember to name your lock_guards and unique_locks [[CP.44](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp44-remember-to-name-your-lock_guards-and-unique_locks)]
- Define a mutex together with the data it guards. Use synchronized_value<T> where possible [[CP.50](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp50-define-a-mutex-together-with-the-data-it-guards-use-synchronized_valuet-where-possible)]
- Use a future to return a value from a concurrent task [[CP.60](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp60-use-a-future-to-return-a-value-from-a-concurrent-task)]
- Use a async() to spawn a concurrent task [[CP.61](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cp61-use-a-async-to-spawn-a-concurrent-task)]
---------------

## Error handling
- Develop an error-handling strategy early in a design [[E.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e1-develop-an-error-handling-strategy-early-in-a-design)]
- Throw an exception to signal that a function can't perform its assigned task [[E.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e2-throw-an-exception-to-signal-that-a-function-cant-perform-its-assigned-task)]
- Use exceptions for error handling only [[E.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e3-use-exceptions-for-error-handling-only)]
- Design your error-handling strategy around invariants [[E.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e4-design-your-error-handling-strategy-around-invariants)]
- Let a constructor establish an invariant, and throw if it cannot [[E.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e5-let-a-constructor-establish-an-invariant-and-throw-if-it-cannot)]
- Use RAII to prevent leaks [[E.6](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e6-use-raii-to-prevent-leaks)]
- State your preconditions [[E.7](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e7-state-your-preconditions)]
- State your postconditions [[E.8](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e8-state-your-postconditions)]
- Use noexcept when exiting a function because of a throw is impossible or unacceptable [[E.12](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e12-use-noexcept-when-exiting-a-function-because-of-a-throw-is-impossible-or-unacceptable)]
- Never throw while being the direct owner of an object [[E.13](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e13-never-throw-while-being-the-direct-owner-of-an-object)]
- Use purpose-designed user-defined types as exceptions (not built-in types) [[E.14](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e14-use-purpose-designed-user-defined-types-as-exceptions-not-built-in-types)]
- Catch exceptions from a hierarchy by reference [[E.15](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e15-catch-exceptions-from-a-hierarchy-by-reference)]
- Destructors, deallocation, and swap must never fail [[E.16](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e16-destructors-deallocation-and-swap-must-never-fail)]
- Don't try to catch every exception in every function [[E.17](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e17-dont-try-to-catch-every-exception-in-every-function)]
- Minimize the use of explicit try/catch [[E.18](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e18-minimize-the-use-of-explicit-trycatch)]
- Use a final_action object to express cleanup if no suitable resource handle is available [[E.19](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e19-use-a-final_action-object-to-express-cleanup-if-no-suitable-resource-handle-is-available)]
- If you can't throw exceptions, simulate RAII for resource management [[E.25](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e25-if-you-cant-throw-exceptions-simulate-raii-for-resource-management)]
- If you can't throw exceptions, consider failing fast [[E.26](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e26-if-you-cant-throw-exceptions-consider-failing-fast)]
- If you can't throw exceptions, use error codes systematically [[E.27](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e27-if-you-cant-throw-exceptions-use-error-codes-systematically)]
- Avoid error handling based on global state (e.g. errno) [[E.28](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e28-avoid-error-handling-based-on-global-state-eg-errno)]
- Don't use exception specifications [[E.30](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e30-dont-use-exception-specifications)]
- Properly order your catch-clauses [[E.31](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#e31-properly-order-your-catch-clauses)]
---------------

## Constants & immutability
- By default, make objects immutable [[Con.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#con1-by-default-make-objects-immutable)]
- By default, make member functions const [[Con.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#con2-by-default-make-member-functions-const)]
- By default, pass pointers and references to consts [[Con.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#con3-by-default-pass-pointers-and-references-to-consts)]
- Use const to define objects with values that do not change after construction [[Con.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#con4-use-const-to-define-objects-with-values-that-do-not-change-after-construction)]
- Use constexpr for values that can be computed at compile time [[Con.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#con5-use-constexpr-for-values-that-can-be-computed-at-compile-time)]
---------------

## Templates & generic programming
- Use templates to raise the level of abstraction of code [[T.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#t1-use-templates-to-raise-the-level-of-abstraction-of-code)]
- Use templates to express algorithms that apply to many argument types [[T.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#t2-use-templates-to-express-algorithms-that-apply-to-many-argument-types)]
---------------

## C-style programming
- Prefer C++ to C [[CPL.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cpl1-prefer-c-to-c)]
- If you must use C, use the common subset of C and C++, and compile the C code as C++ [[CPL.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cpl2-if-you-must-use-c-use-the-common-subset-of-c-and-c-and-compile-the-c-code-as-c)]
- If you must use C for interfaces, use C++ in the calling code using such interfaces [[CPL.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#cpl3-if-you-must-use-c-for-interfaces-use-c-in-the-calling-code-using-such-interfaces)]
---------------

## Standard library
- Prefer using STL array or vector instead of a C array [[SL.con.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#slcon1-prefer-using-stl-array-or-vector-instead-of-a-c-array)]
- Prefer using STL vector by default unless you have a reason to use a different container [[SL.con.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#slcon2-prefer-using-stl-vector-by-default-unless-you-have-a-reason-to-use-a-different-container)]
- Avoid bounds errors [[SL.con.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#slcon3-avoid-bounds-errors)]
- Use std::string to own character sequences [[SL.str.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#slstr1-use-stdstring-to-own-character-sequences)]
- Use std::string_view or gsl::string_span to refer to character sequences [[SL.str.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#slstr2-use-stdstring_view-or-gslstring_span-to-refer-to-character-sequences)]
- Use char* to refer to a single character [[SL.str.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#slstr4-use-char-to-refer-to-a-single-character)]
- Use std::byte to refer to byte values that do not necessarily represent characters [[SL.str.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#slstr5-use-stdbyte-to-refer-to-byte-values-that-do-not-necessarily-represent-characters)]
- Use the s suffix for string literals meant to be standard-library strings [[SL.str.12](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#slstr12-use-the-s-suffix-for-string-literals-meant-to-be-standard-library-strings)]
- Prefer iostreams for I/O [[SL.io.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#slio3-prefer-iostreams-for-io)]
- Avoid endl [[SL.io.50](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#slio50-avoid-endl)]
---------------

## Non-Rules and myths
- **Don't**: All declarations should be at the top of a function [[NR.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nr1-dont-all-declarations-should-be-at-the-top-of-a-function)]
- **Don't**: Have only a single return-statement in a function [[NR.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nr2-dont-have-only-a-single-return-statement-in-a-function)]
- **Don't**: Don't use exceptions [[NR.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nr3-dont-dont-use-exceptions)]
- **Don't**: Place each class declaration in its own source file [[NR.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nr4-dont-place-each-class-declaration-in-its-own-source-file)]
- **Don't**: Don't do substantive work in a constructor; instead use two-phase initialization [[NR.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nr5-dont-dont-do-substantive-work-in-a-constructor-instead-use-two-phase-initialization)]
- **Don't**: Place all cleanup actions at the end of a function and goto exit [[NR.6](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nr6-dont-place-all-cleanup-actions-at-the-end-of-a-function-and-goto-exit)]
- **Don't**: Make all data members protected [[NR.7](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nr7-dont-make-all-data-members-protected)]
---------------

## Naming and layout rules
- Don't say in comments what can be clearly stated in code [[NL.1](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl1-dont-say-in-comments-what-can-be-clearly-stated-in-code)]
- State intent in comments [[NL.2](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl2-state-intent-in-comments)]
-  Keep comments crisp [[NL.3](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl3-keep-comments-crisp)]
- Maintain a consistent indentation style [[NL.4](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl4-maintain-a-consistent-indentation-style)]
- Avoid encoding type information in names [[NL.5](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl5-avoid-encoding-type-information-in-names)]
- Make the length of a name roughly proportional to the length of its scope [[NL.7](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl7-make-the-length-of-a-name-roughly-proportional-to-the-length-of-its-scope)]
- Use a consistent naming style [[NL.8](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl8-use-a-consistent-naming-style)]
- Use ALL_CAPS for macro names only [[NL.9](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl9-use-all_caps-for-macro-names-only)]
- Prefer underscore_style names [[NL.10](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl10-prefer-underscore_style-names)]
- Use spaces sparingly [[NL.15](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl15-use-spaces-sparingly)]
- Make literals readable [[NL.11](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl11-make-literals-readable)]
- Use a conventional class member declaration order [[NL.16](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl16-use-a-conventional-class-member-declaration-order)]
- Use K&R-derived layout [[NL.17](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl17-use-kr-derived-layout)]
- Use C++-style declarator layout [[NL.18](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl18-use-c-style-declarator-layout)]
- Avoid names that are easily misread [[NL.19](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl19-avoid-names-that-are-easily-misread)]
- Don't place two statements on the same line [[NL.20](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl20-dont-place-two-statements-on-the-same-line)]
- Declare one name (only) per declaration [[NL.21](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl21-declare-one-name-only-per-declaration)]
- Don't use void as an argument type [[NL.25](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl25-dont-use-void-as-an-argument-type)]
- Use conventional const notation [[NL.26](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#nl26-use-conventional-const-notation)]
- **Line Length:** each line of text in your code should be at most 130 characters long.
- **Spaces vs. Tabs:** use only spaces. You should set your editor to emit spaces when you hit the tab key.
- **Function Declarations and Definitions:** return type on the same line as function name, parameters on the same line if they fit. Wrap parameter lists which do not fit on a single line as you would wrap arguments in a function call.
- **Braced Initializer List Format:** format a braced initializer list exactly like you would format a function call in its place.
- **Loops and Switch Statements:** switch statements may use braces for blocks. Annotate non-trivial fall-through between cases. Braces are optional for single-statement loops. Empty loop bodies should use {} or continue.
- **Pointer and Reference Expressions:** 
    When declaring a pointer variable or argument, place the asterisk / ampersand adjacent to the type:
    
        #!C++
        const string& str;

* **General Naming Rules:** 
names should be descriptive; avoid abbreviation.  
Give as descriptive a name as possible, within reason. Do not worry about saving horizontal space as it is far more important to make your code immediately understandable by a new reader. Do not use abbreviations that are ambiguous or unfamiliar to readers outside your project, and do not abbreviate by deleting letters within a word. [[examples](https://google.github.io/styleguide/cppguide.html#General_Naming_Rules)]
* **File Names:** 
filenames should be all lowercase and can include underscores (_).
* **Type Names:** 
type names start with a capital letter and have a capital letter for each new word, with no underscores: MyExcitingClass, MyExcitingEnum.  
    The names of all types — classes, structs, type aliases, enums, and type template parameters — have the same naming convention. Type names should start with a capital letter and have a capital letter for each new word. No underscores.
* **Variable Names:** 
the names of variables and data members are all lowercase, with underscores between words. Data members of classes (but not structs) additionally have trailing underscores. For instance: *a_local_variable*, *a_struct_data_member*, *a_class_data_member_*.

        #!C++
        // Common variable names
        string table_name;  // OK - uses underscore.
        string tablename;   // OK - all lowercase.
        string tableName;   // Bad - mixed case.


        // Class Data Members
        class TableInfo {
          ...
        private:
          string table_name_;  // OK - underscore at end.
          string tablename_;   // OK.
          static Pool<TableInfo>* pool_;  // OK.
        };

    
        // Struct Data Members
        struct UrlTableProperties {
          string name;
          int num_entries;
          static Pool<UrlTableProperties>* pool;
        };


* **Constant Names:** 
variables declared _constexpr_ or _const_, and whose value is fixed for the duration of the program, are all lowercase with underscores between words. For example:

        #!C++
        const int elan_doree = 7;  

    It's also encouraged to put them in a specific namespace

* **Function Names:**
regular functions have mixed case.  
Ordinarily, functions should use "camel case". Such names should not have underscores. Prefer to capitalize acronyms as single words (i.e. _StartRpc()_, not _StartRPC()_).

        #!C++
        addTableEntry()
        deleteUrl()
        openFileOrDie()

* **Almost Always Auto:**
use _auto_ whenever you can.  

        #!C++
        auto val = getValue(); // type deduction
        auto s = std::string{"a string"}; // auto to stick : s is sure to be a std::string and not a const char *
        auto getValue() -> int; // function declaration
        

* **Auto and const:**
place _auto_ and  _const_ at the left most part of a declaration.  
When both are present, start with _auto_.
        
        #!C++
        auto i = int{0};
        auto const j = 25.f;


---------------

## Namespaces
- Do not use *using namespace* directive in .h files
- Use project and modules namespaces

        #!C++
        namespace saturnin {
        namespace core {
        void foo();
        }  
        }  

- Use unnamed namespace in *.cpp* files (to avoid link time naming conflicts), not in *.h* files.
- Prefer grouping functions with a namespace instead of using a class as if it were a namespace.
- The contents of namespaces are not indented. Namespaces do not add an extra level of indentation. 
- Namespace names are all lower-case. Top-level namespace names are based on the project name.

        #!C++
        namespace saturnin {
        namespace cdblock {
        
        void RunCdBlock();
        
        }
        }

-------------

## Comments
When writing your comments, write for your audience: the next contributor who will need to understand your code. Be generous — the next one may be you!

- **Comment Style:**
use the // syntax.
- **File Comments:**
    * start each file with license boilerplate, followed by a description of its contents.
    * every file should contain license boilerplate (Saturnin uses Apache License 2.0 for instance).
    * every file should have a comment at the top describing its contents. Do not duplicate comments in both the .h and the .cpp, as duplicated comments diverge.
- **Class Comments:**
every class definition should have an accompanying comment that describes what it is for and how it should be used. [[more](https://google.github.io/styleguide/cppguide.html#Class_Comments)]
- **Function comments:**
declaration comments describe use of the function; comments at the definition of a function describe operation. [[more](https://google.github.io/styleguide/cppguide.html#Function_Comments)]
- **Variable comments:**
in general the actual name of the variable should be descriptive enough to give a good idea of what the variable is used for. In certain cases, more comments are required
    *  Class Data members: each class data member (also called an instance variable or member variable) should have a comment describing what it is used for. If the variable can take sentinel values with special meanings, such as a null pointer or -1, document this. For example:

            #!C++
            private:
             // Keeps track of the total number of entries in the table.
             // Used to ensure we do not go over the limit. -1 means
             // that we don't yet know how many entries the table has.
             int num_total_entries_;

    *  Global variables: as with data members, all global variables should have a comment describing what they are and what they are used for. For example:

            #!C++
            const std::uint8_t intructions_number = 142;	///< Total number of SH2 instructions used.

- **Implementation comments:**
In your implementation you should have comments in tricky, non-obvious, interesting, or important parts of your code.
    * Explanatory Comments: tricky or complicated code blocks should have comments before them.
    * Line Comments: lines that are non-obvious should get a comment at the end of the line. These end-of-line comments should be separated from the code by 2 spaces.
    * Don'ts: do not state the obvious. In particular, don't literally describe what code does, unless the behavior is nonobvious to a reader who understands C++ well. Instead, provide higher level comments that describe why the code does what it does, or make the code self describing.

- **Punctuation, Spelling and Grammar:** pay attention to punctuation, spelling, and grammar; it is easier to read well-written comments than badly written ones.

    Comments should be as readable as narrative text, with proper capitalization and punctuation. In many cases, complete sentences are more readable than sentence fragments. Shorter comments, such as comments at the end of a line of code, can sometimes be less formal, but you should be consistent with your style.

- **TODO Comments:** use TODO comments for code that is temporary, a short-term solution, or good-enough but not perfect.

---------------

## Header files
* **Self-contained Headers:**
textual inclusion files should use *.inc* extension instead of *.h*.
* **The #define Guard:**
all header files should have #define guards to prevent multiple inclusion. The format of the symbol name should be <PROJECT>\_<FILE>\_H\_.
* **Order of includes:**  

        #!C++
        // SYSTEM INCLUDES
        C files
        C++ files
        // PROJECT INCLUDES

------------
## Other C++ Features
* **sizeof:** 
prefer sizeof(varname) to sizeof(type).
