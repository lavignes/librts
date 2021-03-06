#ifndef LIBRTS_H
#define LIBRTS_H

#include <stddef.h>

#ifdef __cplusplus
#define RTS_EXTERN extern "C"
#else
#define RTS_EXTERN extern
#endif

typedef enum _RtsTypeTag {
    RTS_TYPE_TAG_UINT,
    RTS_TYPE_TAG_SINT,
    RTS_TYPE_TAG_CHAR,
    RTS_TYPE_TAG_UCHAR,
    RTS_TYPE_TAG_SCHAR,
    RTS_TYPE_TAG_USHORT,
    RTS_TYPE_TAG_SSHORT,
    RTS_TYPE_TAG_ULONG,
    RTS_TYPE_TAG_SLONG,
    RTS_TYPE_TAG_ULONGLONG,
    RTS_TYPE_TAG_SLONGLONG,
    RTS_TYPE_TAG_FLOAT,
    RTS_TYPE_TAG_DOUBLE,
    RTS_TYPE_TAG_LONGDOUBLE,
    RTS_TYPE_TAG_UINT8,
    RTS_TYPE_TAG_SINT8,
    RTS_TYPE_TAG_UINT16,
    RTS_TYPE_TAG_SINT16,
    RTS_TYPE_TAG_UINT32,
    RTS_TYPE_TAG_SINT32,
    RTS_TYPE_TAG_UINT64,
    RTS_TYPE_TAG_SINT64,
    RTS_TYPE_TAG_POINTER,
    RTS_TYPE_TAG_STRUCT,
    RTS_TYPE_TAG_UNION,
} RtsTypeTag;

typedef enum _RtsStatus {
    RTS_STATUS_OK = 0,
    RTS_STATUS_BAD_TYPEDEF
} RtsStatus;

typedef struct _RtsType {
    RtsTypeTag tag;
    size_t alignment;
    size_t size;
    struct _RtsType **elements;
    size_t *offsets;
} RtsType;

RTS_EXTERN RtsType RTS_TYPE_UINT;
RTS_EXTERN RtsType RTS_TYPE_SINT;
RTS_EXTERN RtsType RTS_TYPE_CHAR;
RTS_EXTERN RtsType RTS_TYPE_UCHAR;
RTS_EXTERN RtsType RTS_TYPE_SCHAR;
RTS_EXTERN RtsType RTS_TYPE_USHORT;
RTS_EXTERN RtsType RTS_TYPE_SSHORT;
RTS_EXTERN RtsType RTS_TYPE_ULONG;
RTS_EXTERN RtsType RTS_TYPE_SLONG;
RTS_EXTERN RtsType RTS_TYPE_ULONGLONG;
RTS_EXTERN RtsType RTS_TYPE_SLONGLONG;
RTS_EXTERN RtsType RTS_TYPE_FLOAT;
RTS_EXTERN RtsType RTS_TYPE_DOUBLE;
RTS_EXTERN RtsType RTS_TYPE_LONGDOUBLE;
RTS_EXTERN RtsType RTS_TYPE_UINT8;
RTS_EXTERN RtsType RTS_TYPE_SINT8;
RTS_EXTERN RtsType RTS_TYPE_UINT16;
RTS_EXTERN RtsType RTS_TYPE_SINT16;
RTS_EXTERN RtsType RTS_TYPE_UINT32;
RTS_EXTERN RtsType RTS_TYPE_SINT32;
RTS_EXTERN RtsType RTS_TYPE_UINT64;
RTS_EXTERN RtsType RTS_TYPE_SINT64;
RTS_EXTERN RtsType RTS_TYPE_POINTER;

RTS_EXTERN RtsStatus rts_type_init(RtsType *type);

#endif /* LIBRTS_H */