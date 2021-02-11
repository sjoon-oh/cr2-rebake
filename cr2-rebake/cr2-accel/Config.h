#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

typedef uint16_t DENSE_TYPE;
typedef uint32_t SPARSE_TYPE;


//#define COMMID(ID) ((ID & 0xffff0000))
#define COMMID(ID) ((ID & 0xffff0000))

//#define NUM_OF_NODES (512)
#define NUM_OF_NODES (512)

//#define COMM_SIZE ((4294967296))
// #define COMM_SIZE ((65536))
#define COMM_SIZE ((16))
//#define COMM_SIZE (5242880)
//#define COMM_SIZE (1048576)


#endif

