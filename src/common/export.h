#ifndef LEGIT_EXPORT_H
#define LEGIT_EXPORT_H

#ifdef LEGIT_STATIC_DEFINE
#  define __LEGIT_EXPORT
#else
#  ifndef __LEGIT_EXPORT
#    if defined(_MSC_VER)
#      ifdef legit_EXPORTS
         /* We are building this library */
#        define __LEGIT_EXPORT __declspec(dllexport)
#      else
         /* We are using this library */
#        define __LEGIT_EXPORT __declspec(dllimport)
#      endif
#    elif defined(__GNUC__)
#      ifdef legit_EXPORTS
         /* We are building this library */
#        define __LEGIT_EXPORT __attribute__((visibility("default")))
#      else
         /* We are using this library */
#        define __LEGIT_EXPORT __attribute__((visibility("default")))
#      endif
#    endif
#  endif
#endif

#endif