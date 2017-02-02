#include "safe_memory_alloc.H"
#include <pthread.h>

pthread_mutex_t Safe_Memory::safe_memory_lock = PTHREAD_MUTEX_INITIALIZER;

void * Safe_Memory::SAFE_MALLOC(size_t size)
{
  void * address;
  {
    CRITICAL_SECTION(&safe_memory_lock);
    address = malloc(size);
  }

  return address;
}

void Safe_Memory::SAFE_FREE(void * free_address)
{
  CRITICAL_SECTION(&safe_memory_lock);
  free(free_address);
}

Safe_Memory::~Safe_Memory()
{
  pthread_mutex_destroy(&safe_memory_lock);
}


