#ifndef SAFE_MEMORY_ALLOC_H
#define SAFE_MEMORY_ALLOC_H 

#include <aleph.H>
#include <pthread.h>
#include <useMutex.H>

class Safe_Memory 
{  
  static pthread_mutex_t safe_memory_lock;

public:

  static void * SAFE_MALLOC(size_t size);
  
  static void SAFE_FREE(void * free_address);
  
  ~Safe_Memory();
};
#endif //SAFE_MEMORY_ALLOC_H 

