#ifndef _GC_CONFIG_H_
#define _GC_CONFIG_H_


/* 1024 objects in a segment */
#define SEGMENT_SIZE    1024

/* default heap contains 512 segments */
#define DEFAULT_HEAP_SIZE   512

/* extend free_list if availale objects is less than this */
#define EXTEND_THRESHOLD    512

/* triger gc if available objects is less than this */
#define GC_THRESHOLD    256

#endif

