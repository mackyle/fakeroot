--------------------
Building on Mac OS X
--------------------
It's necessary to have a fat binary library for fakeroot to work
properly for all executables on Mac OS X.  (It's also pointless to
build the static library version for Mac OS X because of the way
fakeroot works on Mac OS X.)

-------------
Mac OS X 10.4
-------------
You'll need the GNU autotools to run bootstrap.  They can be installed
via macports with "sudo port install libtool automake autoconf".
Be sure to add the appropriate directory (/opt/local/bin for macports)
to your path before running bootstrap.

If building on 10.4 that means something like this:

  ./bootstrap # GNU autotools required
  SDKROOT='-isysroot /Developer/SDKs/MacOSX10.4u.sdk' \
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
installed via macports with "sudo port install libtool automake
autoconf".  Be sure to add the appropriate directory (/opt/local/bin
for macports) to the FRONT of your path before running bootstrap.

If building on 10.5 that means something like this:

  ./bootstrap # Mac OS X 10.5 autotools are too old
  CFLAGS='-pipe -O2 -arch x86_64 -arch ppc64 -arch i386 -arch ppc' \
  LDFLAGS='-Wl,-force_cpusubtype_ALL' \
  ./configure --disable-dependency-tracking --disable-static
  make && make check && sudo make install

-----------------------
Mac OS X 10.6 and later
-----------------------
If building on 10.6 or later that means something like this:

  ./bootstrap # Mac OS X 10.6 autotools are new enough
  CFLAGS='-pipe -O2 -arch x86_64 -arch i386 -arch ppc -Wno-deprecated-declarations' \
  LDFLAGS='-Wl,-force_cpusubtype_ALL' \
  ./configure --disable-dependency-tracking --disable-static
  make && make check && sudo make install

The stat64 and friends functions may be deprecated as of 10.6, but
they still need to be interecepted and we don't need to see the
warnings about them hence the use of -Wno-deprecated-declarations.

------------------------------
Cross OS Version Compatibility
------------------------------
Building the 10.4 version and running it on 10.5 or 10.6 is not
recommended because the new system calls added in 10.5 and 10.6
will NOT be intercepted by the version built for 10.4.

Similarly building the 10.5 version and running it on 10.6 is not
recommended either because 10.6 added yet more calls that will NOT
be intercepted by the version build for 10.5.
