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
