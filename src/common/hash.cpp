#include "common/hash.h"

namespace utils {

uint32_t Hash32(void *ptr, size_t length) {
  const uint32_t m = 0x5bd1e995;
        const int r = 24;
        const int seed = 97;
        uint32_t h = seed ^ length;
        // Mix 4 bytes at a time into the hash
        const unsigned char *data = (const unsigned char *)ptr;
        while(length >= 4)
        {
                uint32_t k = *(uint32_t *)data;
                k *= m;
                k ^= k >> r;
                k *= m;
                h *= m;
                h ^= k;
                data += 4;
                length -= 4;
        }

	switch(length)
        {
            case 3: h ^= data[2] << 16;
            case 2: h ^= data[1] << 8;
            case 1: h ^= data[0];
            h *= m;
        };
        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.
        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;
        return h;

}

uint64_t Hash64(void *ptr, size_t length) {
 
	const uint32_t m = 0x5bd1e995;
        const int r = 24;
        const int seed=97;
        uint32_t h1 = uint32_t(seed) ^ length;
        uint32_t h2 = uint32_t(seed >> 32);
        const uint32_t * data = (const uint32_t *)ptr;
        while(length >= 8){
                 uint32_t k1 = *data++;
                 k1 *= m; k1 ^= k1 >> r; k1 *= m;
                 h1 *= m; h1 ^= k1;
                 length -= 4;
                 uint32_t k2 = *data++;
                 k2 *= m; k2 ^= k2 >> r; k2 *= m;
                 h2 *= m; h2 ^= k2;
                 length -= 4;
        }

	if(length >= 4){

                 uint32_t k1 = *data++;

                 k1 *= m; k1 ^= k1 >> r; k1 *= m;

                 h1 *= m; h1 ^= k1;

                 length -= 4;

         }

	switch(length){

         case 3: h2 ^= ((unsigned char*)data)[2] << 16;

         case 2: h2 ^= ((unsigned char*)data)[1] << 8;

         case 1: h2 ^= ((unsigned char*)data)[0];

         h2 *= m;

        };



        h1 ^= h2 >> 18; h1 *= m;

        h2 ^= h1 >> 22; h2 *= m;

        h1 ^= h2 >> 17; h1 *= m;

	h2 ^= h1 >> 19; h2 *= m;



        uint64_t h = h1;



        h = (h << 32) | h2;



        return h;


}

}  // end namespace utils
