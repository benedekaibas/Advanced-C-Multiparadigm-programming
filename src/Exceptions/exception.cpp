// ERRNO ERROR HANDLING

#include <cerrno>
#include <cstdio> // std::fopen
#include <cstring> // std::strerror

struct record { ... };
struct record rec;

extern int errno; /* preprocessor macro: thread-local since C++11 */
int myerrno; /* my custom error code */

void f() {
std::FILE *fp;
if ( NULL == (fp = std::fopen( "fname", "r")) ) /* try to open the file */
{
std::fprintf( stderr, "can't open file %s\n", "fname");
std::fprintf( stderr, "reason: %s\n", std::strerror(errno)); /* perror(NULL) */
myerrno = 1;
}
else if ( ! std::fseek( fp, n*sizeof(rec), SEEK_SET) ) /* pos to record */
{
std::fprintf( stderr, "can't find record %d\n", n);
myerrno = 2;
}
else if ( 1 != std::fread( &r, sizeof(r), 1, fp) ) /* try to read a record */
{
std::fprintf( stderr, "can't read record\n");
myerrno = 3;
}
else /* everything was successful up to now */
{
//...
}
}

/*
Doing exception handling can be more efficient than checking function's return value every single time.
While checking function's return value (often leads to a chain of functions therefore more checking is needed)
the performance of our code gets worse. On the other hand we can do exception handling at some point(s) in our code
which would lead to better performance.
*/

extern int errno; // preprocessor macro which is threadsafe
// It is used to indicate error conditions and also it expands to a modifiable lvalue of type int



// IOSTREAM ERROR HANDLING

void f()
{
std::ifstream file("input.txt");
if ( ! file ) /* before C++11: void*, since C++11: bool */
{
std::cerr << "file opening failed\n";
return;
}
for( int n; file >> n; ) /* while ( ! cin.fail() ) */
{
std::cout << n << '\n';
}
if ( file.bad() )
{
std::cerr << "i/o error while reading\n";
}
else if ( file.eof() )
{
std::cerr << "eof reached\n";
}
else if ( file.fail() )
{
std::cerr << "non-integer\n";
}
}

/*
iostream has different flags for error handling. For example, if we want to see that we reached the end of a file
we can use the flag eof() to see if it happened or not.
The iostream error handling mostly relies on stream state flags, and optionally, exceptions.
It can be used for files management, but we can also use it to check for example the if the user input matches
what we expect in our code.

In our code detecting eof(), fail() bits is harder that is why it is possible that a programmer reads a file in and then
when they know the actual size of the file they try to rewrite it, but because of the eof() bit it won't happen since the
file is in a stage of having possible bugs. Hence it is not safe to rewrite it, so the compiler terminates.
*/



// ASSERT ERROR HANDLING

#include <cassert> /* assert.h in C */
void open_file(std::string fname)
{
assert(fname.length() > 0);
std::ifstream f(fname.c_str());
. . .
}

/*
Checking errors with assert we can set logical condition(s) in our code. The problem is that with assert we can give error messages
and our program just terminates. Imagine if it happens in a Boeing airplane. The program just terminates, but does not try to recover
in any way that would lead to crash of the plane.

If we are in the development phase of a project then doing assert can be a good option, but we have to replace it to assertions when we
want to release or product.

Assert does the checking in runtime. THI IS AN IMPORTANT NOTE! Good for checking for nullptr, for example.
*/



// STATIC ASSERT ERROR HANDLING

#include <type_traits>
template <typename T>

void swap(T &x, T &y)
{
static_assert( std::is_nothrow_move_constructible<T>::value, &&
std::is_nothrow_move_assignable<T>::value, "Swap may throw" );
auto tmp = x;
x = y;
y = tmp;
}

#if __STDC_HOSTED__ != 1
# error "Not a hosted implementation"
#endif

#if __cplusplus >= 202302L
# warning "Using #warning as a standard feature"
#endif

/*
Since C++ 11 we have static assert. The key difference between static assert and assert is that static assert does the checking in
compilation time instead of runtime. In some cases stopping the code at compilation time instead of runtime can be more beneficial. That
is the reason behind using static assert in some cases.

Static assert takes parameters -as seen in the code above-, but these paramaters only can be parameters that are known in compilation time.
*/



// EXCEPTION HANDLING WITH SETJMP/LONGJMP IN C

// ONE C SOURCE FILE

#include <setjmp.h>
#include <stdio.h>
jmp_buf x;
int main()
{
int i = 0;
if ( (i = setjmp(x)) == 0 ) // try
{
f();
}
else // catch
{
switch( i )
{
case 1: handler1(); break;
case 2: handler2(); break;
default: fprintf( stdout, "error code = %d\n", i); break;
}
}
return 0;
}

// ANOTHER C SOURCE FILE

// perhaps in another source file
#include <setjmp.h>
extern jmp_buf x;
void f()
{
    // ...
    g();
}
void g()
{
if ( something_wrong() )
{
longjmp(x,2); // throw
}
}

/*
Exception handling is an important part of developing softwares. It gives us the chance to store and cetegories bugs and also
understand when a bug happens and why did it happen.

In the C standard library there were setjmp/longjmp macros that are used by creating a jump buffer variable and when the control flow started
we called the setjmp macro and gave the already created variable to it as a parameter. This macro is basically represented the current state of the
program. The variable was basically the definition of the stack heap.

The setjmp/longjmp pair was used frequently in C programs. For example, in parser codes it was a good way to handle exceptions.
The longjmp has it's name because it not only can do "go to" in the source file, but between source files as well.
*/



// EXCEPTIONS IN C++

try // dangerous area
{
    f(); // someth
}
catch (T1 e1) { /* handler for T1 */ }
catch (T2 e2) { /* handler for T2 */ }
catch (T3 e3) { /* handler for T3 */ }
void f()
{
    //...
    T e;
    throw e; /* throws exception of type T */
    // or:
    throw T(); /* throws default value of T */
}

/*
When we throw an exception in C++ than that object will be copied into a static memory space. Static, const variables will be copied as well
into a static memory space. This is crucial because in the code above if we throw an exception without this mechanic the first object that would
be thrown is e1, so the program would crash. Copying object to a static memory space is the key when we are doing exceptions handling in C++.

Since we have the move semantic (C++11) they would be moved. This is a better and more efficient approach.

The key is that we don't catch the object e, but it's copied version that is stored in the static memory space. If we cannot move or copy an object
then we cannot throw that object as an exception.

The interesting part is that since e has a copy in the static memory space (so there is two of it) hence the e1 at the catch period also has a copy on
the static memory space. This can lead to more memory overhead, but we can call e1 by a reference and that reference would refer to the object located
in the static memory space.
*/



// HOW DOES HANDLING WORKS

/*
A handler of type H catches the exception of type E if:
    H and E is the same type
    H is unambiguous base type of E
    H and E are pointers or references and some of the above
    stands
*/

// EXCEPTION HIERARCHIES

class Base { … };
class Der1 : public Base { … };
class Der2 : public Base { … };
class Der3 : public Der2 { … };
try
{
    f();
// …
}
catch (Der3 &e1) { /* handler for Der3 */ } /* the most derived handler first */
catch (Der2 &e2) { /* handler for Der2 */ }
catch (Der1 &e3) { /* handler for Der1 */ }
catch (Base &e3) { /* handler for Base */ } /* the base handler last */
void f()
{
    if ( ... )
        throw Der3(); /* throw the most derived type */
}

/*
We should pay attention to how to catch the created object (because the stack works in a LIFO system) we have to do the catch in a
reverse system.

Imagine that we throw the base handler first then we won't be able to get access to the other handlers, so they would not get to work
meaning we won't use them. If we throw Der3 handler first than we can get to every handler and at the end for the base class handler.

Also pay attention that the objects in the catches are called with reference. This is for getting access to the object that is created in the
static memory space, so we avoid memory overhead.
*/



// EXCEPTION HIERARCHIES - 2

class net_error { ... };
class file_error { ... };
class nfs_error : public net_error, public file_error { ... };
void f()
{
    try
{
    …
}
    catch( nfs_error nfs ) { ... }
    catch( file_error fe ) { ... }
    catch( net_error ne ) { ... }
}

/*
Doing inheritance -or multiple inheritance- with exceptions is possible and sometimes even beneficial. If I want to catch a bug with handler a and handler b
then it can be very useful to do inheritance (Network Filesystems, for example). In network filesystem we can catch filesystem errors and networks errors with
different handlers. In case of inheritance it can be a very useful approach.
*/



// RE-THROW

class Base { … };
class Der1 : public Base { … };
void g()
{
    throw Der1; // throw derived exception Der1
}
void f()
try // function block itself can be try block
{
    g();
}
catch ( Base b ) // catch Base by value: copied, since C++11 can be moved
{
    must_do_at_exception(b);
    throw; // re-throw original exception Der1
}
catch ( ... ) // catch all
{
    must_do_at_exception();
    throw; // re-throw original exception
}

/*
Re-throw can be useful when a handler cannot handle the given job so it throws it to another handler in a higher hierarchy that will do the handling.
Also if a handle only can solve a part of it then it is possible to throw it, so another handler can solve the rest of the job (it has to work with)
the original object and not the sliced one! In this case it is important to note that we must re-throw the original object, and not the sliced one.

Re-throw happens in the catch phase and we know if it's a re-throw if the throw does not have any parameters, so we will throw the object that evoke the
original exception.

In the code above when we do the re-throw we do not throw the Base b. The der1 will be copied into the static memory location which is the base class, so
we will throw the copy that is stored in the static memory location. If we would say throw(b) then we would throw the original object and not the copied version
that is located in the static memory. THIS IS AN IMPORTANT PART OF RE-THROWING!

(...) -> this is an universal handler which catches "everything". This is the most general handler. If we have this handler then we must write it as the lowest handler
and of course it should not be at the top of the hierarchy. In this case always think of the "szita efektus".
*/



// EXCEPTION HIERARCHIES - 3 | WHY IS IT USEFUL TO CREATE EXCEPTION HIERARCHIES?

#include <stdexcept>

struct matrixError // perhaps member in class Matrix
{
    matrixError(std::string r) : reason(r) { }
    std::string reason;
    virtual ~matrixError() { } // we know that we will be working with inheritance, so we need a virtual destructor
};

struct indexError : public matrixError, public std::out_of_range
{
    indexError(int i, const char *r = "Bad index") : matrixError(r), out_of_range(r), index(i)
    {
        std::ostringstream os;
        os << index;
        reason += ", index = ";
        reason += os.str();
    }

    const char *what() const noexcept override
    {
        return reason.c_str();
    }

    virtual ~indexError() { }
    int index;
};

struct rowIndexError : public indexError
{
    rowIndexError(int i) : indexError(i, "Bad row index") { }
};

struct colIndexError : public indexError
{
    colIndexError(int i) : indexError(i, "Bad col index") { }
};

/*
The main point of creating exception hierarchies is that we can express the bugs and handle them in the most sophisticated way.
*/



// STD EXCEPTION HIERARCHY

class exception {}; // in <exception>

class bad_exception : public exception {}; // calls unexpected()
class bad_weak_ptr : public exception {}; // C++11 weak_ptr -> shared_ptr
class bad_function_call : public exception {}; // C++11 function::operator()
class bad_typeid : public exception {}; // typeid(0)
class bad_cast : public exception {}; // dynamic_cast
    class bad_any_cast : public bad_cast {}; // C++17
class bad_variant_access : exception {} // C++17
class bad_optional_access : exception {} // C++17
class bad_alloc : public exception {}; // new <new>
    class bad_array_new_length : bad_alloc {} // C++11, new T[-1]

class logic_error : public exception {};
    class domain_error : public logic_error {}; // domain error,std::sqrt(-1)
    class invalid_argument : public logic_error {}; // bitset char != 0 or 1
    class length_error : public logic_error {}; // length str.resize(-1)
    class out_of_range : public logic_error {}; // bad index in container or string
    class future_error : public logic_error {}; // C++11: promise abandons the shared state

class runtime_error : public exception {};
    class range_error : public runtime_error {}; // floating point ovf or unf
    class overflow_error : public runtime_error {}; // int overflow INT_MAX+1
    class underflow_error : public runtime_error {}; // int underflow INT_MIN-1
    class system_error : public runtime_error {}; // e.g. std::thread constr.
        class ios_base::failure : public system_error {}; // C++11
        class filesystem::filesystem_error : public system_error {}; // C++17
        class nonexistent_local_time : public system_error // C++20
        class ambigous_local_time : public system_error // C++20
        class format_error : public system_error // C++20



// EXCEPTION SPECIFICATION BEFORE C++11

class E1;
class E2;

void f() throw(E1) // throws only E1 or subclasses
{
    ...
    throw E1(); // throws exception of type E1
    ...
    throw E2(); // calls unexpected() which calls terminate()
}

// same as:
void f()
try {
    ...
}
catch(E1) { throw; }
catch(...) { std::unexpected(); }

/*
The problem is that if f calls function c that calls another function that does exception handling. If this function throws exception that will go through
function c and then in function f as well. The compiler cannot check it in C.

That is the reason why in C++ the throw(E1) only can throw E1 or its subclasses, but not any other classes avoiding the problem we described above. If E1 comes
we catch it and throw, but if anything else comes then we abort the code instantly.

Since C++17 this got deleted from the standard.

The code below is a more sophisticated way of throwing exception and we do not terminate the code:
*/

class E1;
class E2;

void f() throw(E1, std::bad_exception) // throws only E1 or subclasses
{
    ...
    throw E1(); // throws exception of type E1
    ...
    throw E2(); // calls unexpected() which throws bad_exception()
}

typedef void (*terminate_handler)();
terminate_handler set_terminate(terminate_handler);
terminate_handler get_terminate(); // C++11

// until C++17
typedef void (*unexpected_handler)();
unexpected_handler set_unexpected(unexpected_handler);
unexpected_handler get_unexpected(); // C++11

void f() throw() // does not throw, can be optimized
// People using the f() function could assume that this function does not throw exception since the () after the throw is empty, so
// they could optimize it. The problem is that during compilation time we cannot check it

/*
So here, instead of terminating the code we just include the std::bad_exception and then handle the exception.

This got also removed from the standard since C++20

Because all of the issues with these exception handling methods we got the noexcept in the standard.
*/



// NOEXCEPT SPECIFIER

void f() noexcept(expr) { } // the compiler will evaluate the parameter in compilation time
void f() noexcept(true) { }
void f() noexcept { } // noexcept(true)

/*
Noexcept is important for optimizations and code safety. C++ functions are either non-throwing or potentially throwing (we are not sure if they will throw, but potentially they can).
C++ cannot execute full compile time check on possible exceptions due to possible call of non C++ functions.

IMPORTANT: Destructors, deallocation functions are non-throwing, so it means we don't need to write noexcept for them.

Based on how we declared the base class or function that the following expression call, they can be non-throwing as well: Implicitly declared or defaulted default-, copy- and move constructors,
copy, move oper. So again, based on what base class or operators they are calling (if they call a noexcept base class for example) we do not need to describe them as noexcept.

At move semantic, for example, using noexcept can lead to better performance.
*/



// NOEXCEPT OPERATOR

template <typename T>
//       this is the specifier   and this is the operator!!!
void f() noexcept               (noexcept(T::g())) {
    T::g();
}

/*
In C++ there are noexcept specifier, and operator. In case of noexcept operator the noexcept has a parameter (T::g()). The parameter produces true or false values based on its argument is
non-throwing or potentially throwing.

The key difference between the noexcept operator and specifier: In the f() with a noexcept operator we aren't saying that this function never gives exception, but we rather say that this f()
will throw or not throw exception based on what we know about function g().

This is especially useful with templates when we do not know what type it will be instantiated with. We do not know that the type the template will be instantiated with will throw exception or not.

*/

// Basic example of output
template <typename T>
void f() noexcept(noexcept(T::g(5 + 2))) {
    T::g();
}

// OUTPUT: True



// DESTRUCTORS

/*
In C++ it is a very strong rule that destructors must not throw exceptions. It's not strictly in the standard library, so we can write destructors that throw exception and there are examples for that in real
life as well, but we have to be careful with that. This article inspects this topic in more detail (https://akrzemi1.wordpress.com/2011/09/21/destructors-that-throw/).
*/



// EXCEPTION SAFETY

class T1 {...};
class T2 {...};

template <typename T1, typename T2>
void f(T1*, T2*);

void g() {
    f(new T1(), new T2());
    // ...
}

/*
Imagine that I have constructed T1, but T2 throws exception, because we couldn't allocate the memory in the heap of T2 or the constructor of T2 throws exception then C++ guarantees that the memory reserved by T2 will
free, but the memory space of T1 won't free and then we get memory leak. Before C++17 it was also possible that the expressions got overlapped in the call. Both expression were before the sequence point, so it was possible
before C++17 that the memory was allocated for T1 and then T2, but the constructor got called for T1 and then T2 and if there was an exception in any of the parameters then the memory allocated to the other parameter didn't free.

This is the exception safety problem that's the reason why we have to carefully implement exceptions because there can happen memory leak and we lose the whole point of working with exceptions.
*/



// ENSURE EXCEPTION SAFETY

class T1 {...};

class T2 {...};

template <typename T1, typename T2>
void f(std::unique_ptr<T1>, std::unique_ptr<T2>);

void g() {
    std::unique_ptr<T1> ptr1{ new T1() };
    std::unique_ptr<T2> ptr2{ new T2() };
    f(ptr1, ptr2);
    // ...
}

/*
By rearranging the source code and using sequence points we can make sure that exception safety is guaranteed.
For example, using unique pointers and declare them differently (under each other, so there are sequance point between them) we can ensure exception safety.

*/



// EXCEPTION SAFETY IN STL

/*
In the stl there are there levels of exception safety described:
    Basic guarantee: no memory leak or other resource issue
    Strong guarantee: the operation is atomic:
                    it either succeeds or has no effect
        e.g. push_back() for vector, insert() for assoc. cont.
    Nothrow guarantee: the operation does not throw
        e.g. pop_back() for vector, erase() for assoc. cont., swap()

It is possible that a push_back throws exception, but if it happens it's guaranteed that we keep the vectors old value before the push_back().

In case of pop_back() it's guaranteed that it won't throw exception. That's the difference between nothrow guarantee and strong guarantee.
*/



// STRONG GUARANTEE EXAMPLE

template <class T>
class Vec
{
    // ...
private:
    size_t cap;
    size_t sz;
    T* v;
};

Vec<T>& Vec<T>::operator=(const Vec& rhs)
{
    if (this != &rhs)                          if (this != &rhs)
    {                                          {
        delete[] v;                              Vec tmp(rhs);
        cap = sz = rhs.sz;                       cap = sz = tmp.sz;
        v = new T[cap];                          std::swap(v, tmp.v);
        for (size_t i = 0; i < cap; ++i)       }
    v[i] = rhs.v[i];                           // tmp deleted here with the old buffer
    }
    return *this;                              return *this;
}                                             }

/*
On the right hand we create a temporal object - in this case a class called Vec. Here in the temporal object we do the copying and if this copying cannot allocate the memory
or the elements in the buffer throws exception it's not a problem because the temporal object will be terminated. If the copying is successfull then we do operations that are
are noexcept guaranteed, like swap. The problem with the copying on the right hand side is that for a moment we will allocate double of the memory, so it will result in memory overhead.

That's the reason why not all the expressions are strong guaranteed, but only a few of those.
*/



// EXCEPTION PTR

#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>

void handle_eptr(std::exception_ptr eptr) // passing by value is ok
{
    try
    {
        if (eptr != std::exception_ptr())
        {
            std::rethrow_exception(eptr);
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Caught exception \"" << e.what() << "\"\n";
    }
}

int main()
{
    std::exception_ptr eptr;
    try
    {
        std::string().at(1); // this generates an std::out_of_range
    }
    catch (...)
    {
        eptr = std::current_exception(); // capture
    }

    handle_eptr(eptr);
} // destructor for std::out_of_range called here, when the eptr is destructed
  // output: Caught exception "basic_string::at"



// NESTED EXCEPTIONS

void print_exception(const std::exception& e, int level = 0) // prints the string of an exception.
{   // if nested, recurses
    std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);
    } catch(const std::exception& e) {
        print_exception(e, level + 1);
    } catch(...) {}
}

void open_file(const std::string& s) // catches an exception and wraps it in a nested exception
{
    try {
        std::ifstream file(s);
        file.exceptions(std::ios_base::failbit);
    } catch(...) {
        std::throw_with_nested(std::runtime_error("Couldn't open " + s));
    }
}

void run() // sample function that catches an exception and wraps it in a nested exception
{
    try {
        open_file("nonexistent.file");
    } catch(...) {
        std::throw_with_nested(std::runtime_error("run() failed"));
    }
}

int main() // runs the sample function above and prints the caught exception
try { // exception: run() failed
    run(); // exception: Couldn't open nonexistent file
} catch(const std::exception& e) { // exception: basic_ios::clear
    print_exception(e);
}

/*
It is possible that we find some nested exceptions in real life systems, but it's rare. Even though, it's just good to know about them.
*/
