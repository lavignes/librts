#include <stddef.h>

#include <chlorine.h>
#include <rts/rts.h>

// Define the same struct at compile and run-time and check and see if it is the same
CL_SPEC(basic) {

    struct s {
        void *p;
        char c;
        int x;
    };

    RtsType *elements[] = {&RTS_TYPE_POINTER, &RTS_TYPE_CHAR, &RTS_TYPE_SINT, NULL};
    size_t offsets[3];
    RtsType test_type;
    test_type.tag = RTS_TYPE_TAG_STRUCT;
    test_type.elements = elements;
    test_type.offsets = offsets;

    cl_assert(rts_type_init(&test_type) == RTS_STATUS_OK);

    cl_assert(test_type.size == sizeof(struct s));

    cl_assert(offsets[0] == offsetof(struct s, p));
    cl_assert(offsets[1] == offsetof(struct s, c));
    cl_assert(offsets[2] == offsetof(struct s, x));
}

CL_BUNDLE(basic);