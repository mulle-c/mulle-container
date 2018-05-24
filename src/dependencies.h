#ifndef _mulle_container_dependencies_h__
#define _mulle_container_dependencies_h__

/* This is a central include file to keep dependencies out of the actual .c
   files.

   The advantage is that now .c files become motile. They and their header can
   be moved to other projects and don't need to be edited. Also less typing...

   Therefore it is important that this file is called "dependencies.h" and
   not "mulle-container-dependencies.h" to keep the #include uniform.

   The C-compiler will pick up the nearest one.
*/

/* include header file automatically generated by c-sourcetree-update
   here the prefix is harmless and serves disambiguation
 */
# include "_mulle-container-dependencies.h"

/* You can add some more dependencies here */

#endif
