/*
*   This file will be regenerated by `mulle-sde reflect` and any edits will be
*   lost. Suppress generation of this file with:
*      mulle-sde environment --global \
*         set MULLE_SOURCETREE_TO_C_INCLUDE_FILE DISABLE
*
*   To not generate any header files:
*      mulle-sde environment --global \
*         set MULLE_SOURCETREE_TO_C_RUN DISABLE
*/

#ifndef _mulle_container_include_h__
#define _mulle_container_include_h__

// How to tweak the following mulle-allocator #include
//    remove:          `mulle-sourcetree mark mulle-allocator no-header`
//    rename:          `mulle-sde dependency|library set mulle-allocator include whatever.h`
//    toggle #import:  `mulle-sourcetree mark mulle-allocator [no-]import`
//    toggle public:   `mulle-sourcetree mark mulle-allocator [no-]public`
//    toggle optional: `mulle-sourcetree mark mulle-allocator [no-]require`
//    remove for os:   `mulle-sourcetree mark mulle-allocator no-os-<osname>`
# include <mulle-allocator/mulle-allocator.h>   // mulle-allocator

// How to tweak the following mulle-data #include
//    remove:          `mulle-sourcetree mark mulle-data no-header`
//    rename:          `mulle-sde dependency|library set mulle-data include whatever.h`
//    toggle #import:  `mulle-sourcetree mark mulle-data [no-]import`
//    toggle public:   `mulle-sourcetree mark mulle-data [no-]public`
//    toggle optional: `mulle-sourcetree mark mulle-data [no-]require`
//    remove for os:   `mulle-sourcetree mark mulle-data no-os-<osname>`
# include <mulle-data/mulle-data.h>   // mulle-data

#endif
