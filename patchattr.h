/*
  Copyright Ⓒ 2009  Regis Duchesne

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include <sys/attr.h>
#include <sys/mount.h>

#ifndef ATTR_CMN_FILEID
#define ATTR_CMN_FILEID                         0x02000000
#endif
#ifndef ATTR_CMN_PARENTID
#define ATTR_CMN_PARENTID                       0x04000000
#endif
#ifndef ATTR_CMN_FULLPATH
#define ATTR_CMN_FULLPATH                       0x08000000
#endif
#ifndef ATTR_CMN_RETURNED_ATTRS
#define ATTR_CMN_RETURNED_ATTRS                 0x80000000
#endif

#ifdef LIBFAKEROOT_DEBUGGING
extern int fakeroot_debug;

#endif /* LIBFAKEROOT_DEBUGGING */
static void
patchattr(void *attrList, void *attrBuf, uid_t uid, gid_t gid, mode_t mode)
{
  /* Attributes, in the order in which they are returned (which is the same as
     the order they are described in the man page).
  */
  static const struct {
    const char *name;
    u_int32_t value;
    size_t size;
    int isRef;
  } attrs[] = {
#define PATCHATTR(a,b,c) { #a, a, b, c, }
    PATCHATTR(ATTR_CMN_RETURNED_ATTRS, sizeof (attribute_set_t), 0),
    PATCHATTR(ATTR_CMN_NAME, sizeof (attrreference_t), 1),
    PATCHATTR(ATTR_CMN_DEVID, sizeof (dev_t), 0),
    PATCHATTR(ATTR_CMN_FSID, sizeof (fsid_t), 0),
    PATCHATTR(ATTR_CMN_OBJTYPE, sizeof (fsobj_type_t), 0),
    PATCHATTR(ATTR_CMN_OBJTAG, sizeof (fsobj_tag_t), 0),
    PATCHATTR(ATTR_CMN_OBJID, sizeof (fsobj_id_t), 0),
    PATCHATTR(ATTR_CMN_OBJPERMANENTID, sizeof (fsobj_id_t), 0),
    PATCHATTR(ATTR_CMN_PAROBJID, sizeof (fsobj_id_t), 0),
    PATCHATTR(ATTR_CMN_SCRIPT, sizeof (text_encoding_t), 0),
    PATCHATTR(ATTR_CMN_CRTIME, sizeof (struct timespec), 0),
    PATCHATTR(ATTR_CMN_MODTIME, sizeof (struct timespec), 0),
    PATCHATTR(ATTR_CMN_CHGTIME, sizeof (struct timespec), 0),
    PATCHATTR(ATTR_CMN_ACCTIME, sizeof (struct timespec), 0),
    PATCHATTR(ATTR_CMN_BKUPTIME, sizeof (struct timespec), 0),
    PATCHATTR(ATTR_CMN_FNDRINFO, 32, 0),
    PATCHATTR(ATTR_CMN_OWNERID, sizeof (uid_t), 0),
    PATCHATTR(ATTR_CMN_GRPID, sizeof (gid_t), 0),
    PATCHATTR(ATTR_CMN_ACCESSMASK, sizeof (u_int32_t), 0),
    PATCHATTR(ATTR_CMN_NAMEDATTRCOUNT, sizeof (u_int32_t), 0),
    PATCHATTR(ATTR_CMN_NAMEDATTRLIST, sizeof (attrreference_t), 1),
    PATCHATTR(ATTR_CMN_FLAGS, sizeof (u_int32_t), 0),
    PATCHATTR(ATTR_CMN_USERACCESS, sizeof (u_int32_t), 0),
    PATCHATTR(ATTR_CMN_EXTENDED_SECURITY, sizeof (attrreference_t), 1),
    PATCHATTR(ATTR_CMN_UUID, sizeof (guid_t), 0),
    PATCHATTR(ATTR_CMN_GRPUUID, sizeof (guid_t), 0),
    PATCHATTR(ATTR_CMN_FILEID, sizeof (u_int64_t), 0),
    PATCHATTR(ATTR_CMN_PARENTID, sizeof (u_int64_t), 0),
    PATCHATTR(ATTR_CMN_FULLPATH, sizeof (attrreference_t), 1),
#undef PATCHATTR
  };
  struct attrlist *l = attrList;
  unsigned char *b = attrBuf;
  unsigned i;

#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    fprintf(stderr, "patchattr actual attrBuf size %u\n", *(u_int32_t *)b);
  }
#endif /* LIBFAKEROOT_DEBUGGING */
  b += sizeof (u_int32_t);
  for (i = 0; i < sizeof attrs / sizeof attrs[0]; i++) {
    if (l->commonattr & attrs[i].value) {
#ifdef LIBFAKEROOT_DEBUGGING
      if (fakeroot_debug) {
        fprintf(stderr, "patchattr attr %s: yes\n", attrs[i].name);
        if (attrs[i].isRef) {
          size_t here = b - (unsigned char *)attrBuf;
          size_t begin = here + ((attrreference_t *)b)->attr_dataoffset;
          size_t size = ((attrreference_t *)b)->attr_length;
          size_t alignedEnd = (begin + size + 3) & ~3;
          fprintf(stderr, "patchattr reference begin %zu size %zu aligned end %zu\n", begin, size, alignedEnd);
        }
      }
#endif /* LIBFAKEROOT_DEBUGGING */
      if (attrs[i].value == ATTR_CMN_OWNERID) {
#ifdef LIBFAKEROOT_DEBUGGING
        if (fakeroot_debug) {
          fprintf(stderr, "patchattr owner %d\n", *(uid_t *)b);
        }
#endif /* LIBFAKEROOT_DEBUGGING */
        *(uid_t *)b = uid;
      }
      if (attrs[i].value == ATTR_CMN_GRPID) {
#ifdef LIBFAKEROOT_DEBUGGING
        if (fakeroot_debug) {
          fprintf(stderr, "patchattr group %d\n", *(gid_t *)b);
        }
#endif /* LIBFAKEROOT_DEBUGGING */
        *(gid_t *)b = gid;
      }
      if (attrs[i].value == ATTR_CMN_ACCESSMASK) {
#ifdef LIBFAKEROOT_DEBUGGING
        if (fakeroot_debug) {
          fprintf(stderr, "patchattr mode 0%o\n", *(mode_t *)b);
        }
#endif /* LIBFAKEROOT_DEBUGGING */
        *(mode_t *)b = mode;
      }
      b += (attrs[i].size + 3) & ~3;
    }
  }
#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    size_t here = b - (unsigned char *)attrBuf;
    fprintf(stderr, "patchattr attrBuf fixed size %zu\n", here);
  }
#endif /* LIBFAKEROOT_DEBUGGING */
}
