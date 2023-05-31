#include "mulle-pointerpair.h"


MULLE_C_NEVER_INLINE
static void   mulle_pointerpair_qsort_compare_pointer_key( struct mulle_pointerpair *buf,
                                                           unsigned int n,
                                                           void *userinfo)
{
   _mulle_qsort_r_inline( buf,
                          n,
                          sizeof( struct mulle_pointerpair),
                          (int (*)(void *, void *, void *)) _mulle_pointerpair_compare_pointer_key,
                          userinfo);
}


MULLE_C_NEVER_INLINE
static void   mulle_pointerpair_qsort_compare_string_key( struct mulle_pointerpair *buf,
                                                          unsigned int n,
                                                          void *userinfo)
{
   _mulle_qsort_r_inline( buf,
                          n,
                          sizeof( struct mulle_pointerpair),
                          (int (*)(void *, void *, void *)) _mulle_pointerpair_compare_string_key,
                          userinfo);
}


void   mulle_pointerpair_qsort_r( struct mulle_pointerpair *buf,
                                  unsigned int n,
                                  mulle_pointerpair_compare_t *compare,
                                  void *userinfo)
{
   if( ! buf)
      return;

   if( compare == _mulle_pointerpair_compare_pointer_key)
      mulle_pointerpair_qsort_compare_pointer_key( buf, n, userinfo);
   else
      if( compare == _mulle_pointerpair_compare_string_key)
         mulle_pointerpair_qsort_compare_string_key( buf, n, userinfo);
      else
         _mulle_qsort_r_inline( buf,
                                n,
                                sizeof( struct mulle_pointerpair),
                                (int (*)(void *, void *, void *)) compare,
                                userinfo);
}


uintptr_t
   _mulle_pointerpair_find_in_range_callback( struct mulle_pointerpair *buf,
                                              struct mulle_pointerpair search,
                                              struct mulle_range range,
                                              struct mulle_container_keyvaluecallback *callback)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   // pointerpair can't use it
   if( range.length == (uintptr_t) -1)
      abort();

   assert( search.key != mulle_pointerpair_notakey);

   p        = &buf[ range.location];
   sentinel = &p[ range.length];

   if( ! callback || _mulle_container_keycallback_isbitequals( &callback->keycallback))
   {
      while( p < sentinel)
      {
         if( p->key == search.key)
            return( p - buf);

         p++;
      }
   }
   else
   {
      while( p < sentinel)
      {
         if( (*callback->keycallback.is_equal)( &callback->keycallback, p->key, search.key))
            return( p - buf);

         p++;
      }
   }
   return( mulle_not_found_e);
}



uintptr_t
   _mulle_pointerpair_find_in_range_compare( struct mulle_pointerpair *buf,
                                             struct mulle_pointerpair search,
                                             struct mulle_range range,
                                             mulle_pointerpair_compare_t *compare,
                                             void *userinfo)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   if( ! buf)
      return( mulle_not_found_e);

   // pointerpair can't use it
   if( range.length == (uintptr_t) -1)
      abort();

   assert( search.key != mulle_pointerpair_notakey);

   p        = &buf[ range.location];
   sentinel = &p[ range.length];

   if( ! compare || compare == _mulle_pointerpair_compare_pointer_key)
   {
      while( p < sentinel)
      {
         if( p->key == search.key)
            return( p - buf);

         p++;
      }
   }
   else
   {
      while( p < sentinel)
      {
         if( (*compare)( p, &search, userinfo) == 0)
            return( p - buf);

         p++;
      }
   }
   return( mulle_not_found_e);
}




