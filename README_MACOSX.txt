--------------------
Building on Mac OS X
--------------------
It's necessary to have a fat binary library for fakeroot to work
properly for all executables on Mac OS X.  (It's also pointless to
build the static library version for Mac OS X because of the way
fakeroot works on Mac OS X.)

See the Mac OS X version specific headings below for build instructions.

---------------------------
DYLD_FORCE_FLAT_NAMESPACE=1
---------------------------
Previous versions of fakeroot support for Mac OS X required setting
DYLD_FORCE_FLAT_NAMESPACE=1 in the environment (which was automatically
handled by fakeroot).  This prevents fakeroot from being compatible with
some programs and may cause other programs to behave unexpectedly.

This version of Mac OS X support in fakeroot no longer requires that
DYLD_FORCE_FLAT_NAMESPACE be set and therefore fakeroot should now
be fully compatible with any Mac OS X program that relies solely on
Linux/Unix/POSIX APIs (limited only by the normal fakeroot caveats).

------------------------------
What Is Supported On Mac OS X?
------------------------------
Mac OS X has a Mach foundation and numerous system operations rely on
using Mach port communications to establish credentials.  The Mac OS X
fakeroot support DOES NOT intercept any Mach port communications!  This
means that fakeroot support on Mac OS X works ONLY with programs that
rely solely on Linux/Unix/POSIX APIs.

In particular, special effort is made to avoid crashes caused in
CarbonCore starting on Mac OS X 10.6 (and also CoreServicesInternal
starting on Mac OS X 10.7) when the result of getuid() running under
fakeroot differs from the actual real uid as seen through various
Mach APIs that are not intercepted by fakeroot.

The packagemaker command line interface is fully supported (ultimately
it uses pax-like functionality to build the installer archives which
relies solely on APIs supported by fakeroot).  So Mac OS X installer
packages built using packagemaker under fakeroot will have all the
desired users, groups and permissions as set up under fakeroot.

In addition, as expected, all other command line archiving tools work
under fakeroot just as they would under fakeroot on Linux.

The hdiutil command line interface to create disk images can be run
under fakeroot without crashing, but since it makes heavy use of various
Mac OS X Mach APIs, owner, group and permission changes made under fakeroot
ARE NOT picked up by hdiutil.  Also the -uid and -gid options to hdiutil
end up being ignored.  This should not matter if hdiutil is being used
solely to create a compressed disk image containing an installer package
(and possibly some supporting documentation) as such a disk image will
always be mounted read-only and will always have the "Ignore ownership on
this volume" checkbox enabled when it's mounted.

fakeroot support for Mac OS X does NOT currently support creating disk
images via hdiutil run under fakeroot where fakeroot has been used to
alter owners, groups and/or permissions -- hdiutil will simply ignore
all of the fakeroot-made changes and create the disk image the same way
as it would if it was not run under fakeroot (although running under
fakeroot WILL suppress prompting for an sudo password, files that cannot
actually be read as the current user will then fail to be placed on the
disk image).

-------------
Mac OS X 10.4
-------------
You'll need the GNU autotools to run bootstrap.  They can be installed
via macports (macports.org) with "sudo port install libtool automake autoconf".
Be sure to add the appropriate directory (/opt/local/bin for macports)
to your path before running bootstrap.

If building on 10.4 that means something like this:

  ./bootstrap # GNU autotools required
  SDKROOT='-isysroot /Developer/SDKs/MacOSX10.4u.sdk -mmacosx-version-min=10.4'
  CFLAGS="-pipe -O2 -arch x86_64 -arch ppc64 -arch i386 -arch ppc $SDKROOT" \
  ./configure --disable-dependency-tracking --disable-static
  make && make check && sudo make install

Note the use of -isysroot.  The -isysroot option is only required
when building on Mac OS X 10.4 because the system libraries are not
guaranteed to be fat whereas they are on Mac OS X 10.5 and later.

-------------
Mac OS X 10.5
-------------
You'll need newer GNU autotools than come with 10.5.  They can be
installed via macports (macports.org) with "sudo port install libtool automake
autoconf".  Be sure to add the appropriate directory (/opt/local/bin
for macports) to the FRONT of your PATH (so they supersede the
outdated, pre-installed versions) before running bootstrap.

If building on 10.5 that means something like this:

  ./bootstrap # Mac OS X 10.5 autotools are too old
  CFLAGS='-pipe -O2 -arch x86_64 -arch ppc64 -arch i386 -arch ppc' \
  LDFLAGS='-Wl,-force_cpusubtype_ALL' \
  ./configure --disable-dependency-tracking --disable-static
  make && make check && sudo make install

-------------
Mac OS X 10.6
-------------
If building on 10.6 that means something like this:

  ./bootstrap # Mac OS X 10.6 autotools are new enough
  CFLAGS='-pipe -O2 -arch x86_64 -arch i386 -arch ppc -Wno-deprecated-declarations' \
  LDFLAGS='-Wl,-force_cpusubtype_ALL' \
  ./configure --disable-dependency-tracking --disable-static
  make && make check && sudo make install

The stat64 and friends functions may be deprecated as of 10.6, but
they still need to be interecepted and we don't need to see the
warnings about them hence the use of -Wno-deprecated-declarations.

-----------------------
Mac OS X 10.7 and later
-----------------------
If building on 10.7 or later that means something like this:

  ./bootstrap # Mac OS X 10.7 autotools are new enough
  CFLAGS='-pipe -O2 -arch x86_64 -arch i386 -Wno-deprecated-declarations' \
  ./configure --disable-dependency-tracking --disable-static
  make && make check && sudo make install

The stat64 and friends functions may be deprecated as of 10.6, but
they still need to be interecepted and we don't need to see the
warnings about them hence the use of -Wno-deprecated-declarations.
(Since 10.7 dropped ppc support, it's removed from the -arch list.)

------------------------------------
Cross Mac OS X Version Compatibility
------------------------------------
Building the 10.4 version and running it on 10.5 or 10.6 is not
recommended because the new system calls added in 10.5 and 10.6
will NOT be intercepted by the version built for 10.4.

Similarly building the 10.5 version and running it on 10.6 is not
recommended either because 10.6 added yet more calls that will NOT
be intercepted by the version built for 10.5.

And so on for 10.7 etc.
