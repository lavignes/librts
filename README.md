### What is libRTS? ###

From [The Lost Art of C Structure Packing](http://www.catb.org/esr/structure-packing/):
> The first thing to understand is that, on modern processors, the way your C compiler lays out basic C datatypes in memory is constrained in order to make memory accesses faster.

Put simply, the way a struct is defined in your C code is not necessarily the way it will be defined in memory. The compiler may insert extra unused bytes into your struct to make struct member accesses more efficient.

```c
struct s {
    char *p; /* 4 bytes */
    char c;  /* 1 byte */
    int x;   /* 4 bytes */
}

sizeof(s) == 12 ?
```

For example, on an x86 system the size of this struct is not 9 bytes, even though the sum size of each member is 9 bytes. Besides having a size, all types have an intrinsic alignment requirement. In this case, the member `c` has a alignment requirement of only 1 byte, but `x` must be aligned with a 4 byte address. The C compiler will actually generate an internal representation like this:

```c
struct s {
    char *p;
    char c;
    char unused[3];
    int x;
}
```

High level languages may not know at compile time how to manipulate structs passed to and from C functions. This can make foreign function interfaces cumbersome and non-portable. libRTS allows you to define C structs (and unions!) at runtime and gives you the ability to manipulate them as part your foreign fuction interface. In fact, libRTS is designed to complement systems that already use [libFFI](https://github.com/atgreen/libffi)!

### The Basics ###

The first thing you need to do is create an `RtsType` object and set a few of its members. This will represent the "shape" of your struct and allow you to perform introspection on its members. You then complete the initialization of your struct by calling the function `rts_type_init`.

```c
RtsStatus rts_type_init(RtsType *type)
```

This initalizes `type`.
`rts_type_init` returns a status code of type `RtsStatus`. This will either be `RTS_STATUS_OK` if the initialization was successful, or `RTS_STATUS_BAD_TYPEDEF` if the `type` or any of its members is incorrect.

### Simple Example ###

```c
#include <assert.h>
#include <rts/rts.h>

int main(int argc, char **argv) {

    struct s {
        void *p;
        char c;
        int x;
    };
    
    RtsType *elements[] = {&RTS_TYPE_POINTER, &RTS_TYPE_CHAR, &RTS_TYPE_SINT, NULL};
    size_t offsets[3];
    RtsType s_type;
    s_type.tag = RTS_TYPE_TAG_STRUCT;
    s_type.elements = elements;
    s_type.offsets = offsets;

    assert(rts_type_init(&s_type) == RTS_STATUS_OK);

    assert(s_type.size == sizeof(struct s));

    assert(offsets[0] == offsetof(struct s, p));
    assert(offsets[1] == offsetof(struct s, c));
    assert(offsets[2] == offsetof(struct s, x));
    
    return 0;
}
```

### Installing ###

libRTS uses CMake to build and install.

* From the librts directory, create a build folder and navigate inside of it:
```sh
$ mkdir build
$ cd build
```
* Next use CMake to configure the project for building on your system. This causes CMake to read the `CMakeLists.txt` file in the project root and generate a build enviorment in the current directory:
```sh
$ cmake ..
```
* Finally, build and install the library:
```sh
$ make
$ sudo make install
```
* Additionally, you may uninstall using the uninstall target:
```sh
$ sudo make uninstall
```

### Supported Platforms ###

libRTS has been tested on x86/64 and ARM. It *probably* works OOTB with many others.
