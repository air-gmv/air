/**
 * @file
 *
 * @brief Extracts a Node from a Chain
 *
 * @ingroup Score
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

#include <ctype.h>

size_t _Objects_Name_to_string(
  Objects_Name  name,
  bool          is_string,
  char         *buffer,
  size_t        buffer_size
)
{
  char        lname[ 5 ];
  const char *s;
  char       *d;
  size_t      i;

#if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
  if ( is_string ) {
    s = name.name_p;
  } else
#endif
  {
    lname[ 0 ] = (name.name_u32 >> 24) & 0xff;
    lname[ 1 ] = (name.name_u32 >> 16) & 0xff;
    lname[ 2 ] = (name.name_u32 >>  8) & 0xff;
    lname[ 3 ] = (name.name_u32 >>  0) & 0xff;
    lname[ 4 ] = '\0';
    s = lname;
  }

  d = buffer;
  i = 1;

  if ( s != NULL ) {
    while ( *s != '\0' ) {
      if ( i < buffer_size ) {
        *d = isprint((unsigned char) *s) ? *s : '*';
        ++d;
      }

      ++s;
      ++i;
    }
  }

  if ( buffer_size > 0 ) {
    *d = '\0';
  }

  return i - 1;
}

/*
 *  This method objects the name of an object and returns its name
 *  in the form of a C string.  It attempts to be careful about
 *  overflowing the user's string and about returning unprintable characters.
 */

char *_Objects_Get_name_as_string(
  Objects_Id        id,
  size_t            length,
  char             *name
)
{
  Objects_Information   *information;
  Objects_Control       *the_object;
  ISR_lock_Context       lock_context;
  Objects_Id             tmpId;

  if ( length == 0 )
    return NULL;

  if ( name == NULL )
    return NULL;

  tmpId = (id == OBJECTS_ID_OF_SELF) ? _Thread_Get_executing()->Object.id : id;

  information = _Objects_Get_information_id( tmpId );
  if ( !information )
    return NULL;

  the_object = _Objects_Get( tmpId, &lock_context, information );
  if ( the_object == NULL ) {
    return NULL;
  }

  _Objects_Name_to_string(
    the_object->name,
#if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
    information->is_string,
#else
    false,
#endif
    name,
    length
  );

  _ISR_lock_ISR_enable( &lock_context );
  return name;
}
