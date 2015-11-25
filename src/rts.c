#include <stdint.h>
#include <stddef.h>

#include <rts/rts.h>

#define RTS_TYPEDEF(name, type)                     \
    struct _struct_align_##name {                   \
        char c;					                    \
        type x;					                    \
    };						                        \
    const RtsType RTS_TYPE_##name = {	            \
        RTS_TYPE_TAG_##name,                        \
        offsetof(struct _struct_align_##name, x),   \
        sizeof(type),					            \
        NULL, NULL    			                    \
    }                                               \

const RtsType RTS_TYPE_VOID = {
    RTS_TYPE_TAG_VOID, 0, 0, NULL, NULL
};

RTS_TYPEDEF(SINT, signed int);
RTS_TYPEDEF(UINT, unsigned int);
RTS_TYPEDEF(CHAR, char);
RTS_TYPEDEF(UCHAR, unsigned char);
RTS_TYPEDEF(SCHAR, signed char);
RTS_TYPEDEF(USHORT, unsigned short);
RTS_TYPEDEF(ULONG, unsigned long);
RTS_TYPEDEF(SLONG, signed long);
RTS_TYPEDEF(ULONGLONG, unsigned long long);
RTS_TYPEDEF(SLONGLONG, signed long long);
RTS_TYPEDEF(FLOAT, float);
RTS_TYPEDEF(DOUBLE, double);
RTS_TYPEDEF(LONGDOUBLE, long double);
RTS_TYPEDEF(UINT8, uint8_t);
RTS_TYPEDEF(SINT8, int8_t);
RTS_TYPEDEF(UINT16, uint16_t);
RTS_TYPEDEF(SINT16, int16_t);
RTS_TYPEDEF(UINT32, uint32_t);
RTS_TYPEDEF(SINT32, int32_t);
RTS_TYPEDEF(UINT64, uint64_t);
RTS_TYPEDEF(SINT64, int64_t);
RTS_TYPEDEF(POINTER, void *);

RtsStatus rts_init(RtsType *type) {
    return RTS_STATUS_OK;
}
