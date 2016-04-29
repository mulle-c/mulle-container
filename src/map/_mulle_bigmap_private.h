//
//  _mullebigmap_private.h
//  mulle-container
//
//  Created by Nat! on 17.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#ifndef _mullebigmap_private_h__
#define _mullebigmap_private_h__

//
// some somewhat obscure facilities for map
//
static inline int  __mulle_keyvaluepair_is_empty( struct _mulle_keyvaluepair *pair, void *not_a_key_marker) mulle_nonnull_first;
static inline int  __mulle_keyvaluepair_is_empty( struct _mulle_keyvaluepair *pair, void *not_a_key_marker)
{
   return( pair->_value == 0 && pair->_key == not_a_key_marker);
}


static inline int  _mulle_keyvaluepair_is_empty( struct _mulle_keyvaluepair *pair,
                                                 struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;
static inline int  _mulle_keyvaluepair_is_empty( struct _mulle_keyvaluepair *pair,
                                                 struct mulle_container_keyvaluecallback *callback)
{
   return( __mulle_keyvaluepair_is_empty( pair, callback->keycallback.not_a_key_marker));
}


static inline int  __mulle_keyvaluepair_is_queue( struct _mulle_keyvaluepair *pair,
                                                  void *not_a_key_marker) mulle_nonnull_first;
static inline int  __mulle_keyvaluepair_is_queue( struct _mulle_keyvaluepair *pair,
                                                  void *not_a_key_marker)
{
   return( pair->_value != 0 && pair->_key == not_a_key_marker);
}


static inline int  _mulle_keyvaluepair_is_queue( struct _mulle_keyvaluepair *pair,
                                                 struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;
static inline int  _mulle_keyvaluepair_is_queue( struct _mulle_keyvaluepair *pair,
                                                 struct mulle_container_keyvaluecallback *callback)
{
   return( __mulle_keyvaluepair_is_queue( pair, callback->keycallback.not_a_key_marker));
}


static inline int  __mulle_keyvaluepair_is_storage( struct _mulle_keyvaluepair *pair,
                                                    void *not_a_key_marker) mulle_nonnull_first;
static inline int  __mulle_keyvaluepair_is_storage( struct _mulle_keyvaluepair *pair,
                                                    void *not_a_key_marker)
{
   return( pair->_key != not_a_key_marker);
}


static inline int  _mulle_keyvaluepair_is_storage( struct _mulle_keyvaluepair *pair,
                                                   struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;
static inline int  _mulle_keyvaluepair_is_storage( struct _mulle_keyvaluepair *pair,
                                                   struct mulle_container_keyvaluecallback *callback)
{
   return( __mulle_keyvaluepair_is_storage( pair, callback->keycallback.not_a_key_marker));
}

#endif /* _mullebigmap_private_h */
