/*
  Copyright: GPL. 
  Author: regis duchesne  (hpreg@vmware.com)
*/

#include <sys/attr.h>
#include <sys/mount.h>

static void
patchattr(void *attrList, void *attrBuf, uid_t uid, gid_t gid)
{
  /* Attributes, in the order in which they are returned (which is the same as
     the order they are described in the man page).
  */
  static const struct {
    u_int32_t value;
    size_t size;
  } attrs[] = {
#define PATCHATTR(a,b) { a, b, }
    PATCHATTR(ATTR_CMN_RETURNED_ATTRS, sizeof (attribute_set_t)),
    PATCHATTR(ATTR_CMN_NAME, sizeof (attrreference_t)),
    PATCHATTR(ATTR_CMN_DEVID, sizeof (dev_t)),
    PATCHATTR(ATTR_CMN_FSID, sizeof (fsid_t)),
    PATCHATTR(ATTR_CMN_OBJTYPE, sizeof (fsobj_type_t)),
    PATCHATTR(ATTR_CMN_OBJTAG, sizeof (fsobj_tag_t)),
    PATCHATTR(ATTR_CMN_OBJID, sizeof (fsobj_id_t)),
    PATCHATTR(ATTR_CMN_OBJPERMANENTID, sizeof (fsobj_id_t)),
    PATCHATTR(ATTR_CMN_PAROBJID, sizeof (fsobj_id_t)),
    PATCHATTR(ATTR_CMN_SCRIPT, sizeof (text_encoding_t)),
    PATCHATTR(ATTR_CMN_CRTIME, sizeof (struct timespec)),
    PATCHATTR(ATTR_CMN_MODTIME, sizeof (struct timespec)),
    PATCHATTR(ATTR_CMN_CHGTIME, sizeof (struct timespec)),
    PATCHATTR(ATTR_CMN_ACCTIME, sizeof (struct timespec)),
    PATCHATTR(ATTR_CMN_BKUPTIME, sizeof (struct timespec)),
    PATCHATTR(ATTR_CMN_FNDRINFO, 32),
    PATCHATTR(ATTR_CMN_OWNERID, sizeof (uid_t)),
    PATCHATTR(ATTR_CMN_GRPID, sizeof (gid_t)),
    PATCHATTR(ATTR_CMN_ACCESSMASK, sizeof (u_int32_t)),
    PATCHATTR(ATTR_CMN_NAMEDATTRCOUNT, sizeof (u_int32_t)),
    PATCHATTR(ATTR_CMN_NAMEDATTRLIST, sizeof (attrreference_t)),
    PATCHATTR(ATTR_CMN_FLAGS, sizeof (u_int32_t)),
    PATCHATTR(ATTR_CMN_USERACCESS, sizeof (u_int32_t)),
    PATCHATTR(ATTR_CMN_EXTENDED_SECURITY, sizeof (attrreference_t)),
    PATCHATTR(ATTR_CMN_UUID, sizeof (guid_t)),
    PATCHATTR(ATTR_CMN_GRPUUID, sizeof (guid_t)),
    PATCHATTR(ATTR_CMN_FILEID, sizeof (u_int64_t)),
    PATCHATTR(ATTR_CMN_PARENTID, sizeof (u_int64_t)),
    PATCHATTR(ATTR_CMN_FULLPATH, sizeof (attrreference_t)),
#undef PATCHATTR
  };
  struct attrlist *l = attrList;
  unsigned char *b = attrBuf;
  unsigned i;

  b += sizeof (u_int32_t);
  for (i = 0; i < sizeof attrs / sizeof attrs[0]; i++) {
    if (l->commonattr & attrs[i].value) {
      if (attrs[i].value == ATTR_CMN_OWNERID) {
        *(uid_t *)b = uid;
      }
      if (attrs[i].value == ATTR_CMN_GRPID) {
        *(gid_t *)b = gid;
      }
      b += (attrs[i].size + 3) & ~3;
    }
  }
}
