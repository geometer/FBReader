# Upstream: Nikolay Pultsin <geometer@fbreader.org>

%define desktop_vendor rpmforge

Summary: E-book reader
Name: fbreader
Version: 0.8.8
Release: 1
License: GPL
Group: Applications/Multimedia
URL: http://www.fbreader.org/

Packager: Nikolay Pultsin <geometer@fbreader.org>

Source: http://www.fbreader.org/fbreader-sources-%{version}.tgz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: zlib-devel, libpng-devel, gtk2-devel
BuildRequires: desktop-file-utils
BuildRequires: bzip2-devel, expat-devel, libjpeg-devel

%description
FBReader is an e-book reader for Linux PDAs and desktop computers.
FBReader supports several e-book formats: plucker, palmdoc, zTXT, 
HTML, fb2, plain text, etc.

%prep
%setup

%build
%{__make} %{?_smp_mflags} INSTALLDIR=%{_prefix} TARGET_ARCH=pepperpad3 UI_TYPE=gtk TARGET_STATUS=release

%install
%{__rm} -rf %{buildroot}
%makeinstall INSTALLDIR=%{_prefix} TARGET_ARCH=pepperpad3 UI_TYPE=gtk TARGET_STATUS=release DESTDIR=%{buildroot}

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%{_bindir}/FBReader
%{_datadir}/FBReader
%{_datadir}/pixmaps/FBReader.png
%{_datadir}/pixmaps/FBReader
%{_datadir}/applications/FBReader.desktop
%{_libdir}/libzlcore.so.%{version}
%{_libdir}/libzltext.so.%{version}
%{_libdir}/zlibrary/ui/zlui-gtk.so
%{_datadir}/zlibrary

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%changelog
* Sun Nov 18 2007 Nikolay Pultsin <geometer@fbreader.org> - 0.8.8-1
- new upstream version
- excluded keynames.patch

* Sun Nov 18 2007 Nikolay Pultsin <geometer@fbreader.org> - 0.8.7b-1
- new upstream version
- excluded rpath.patch
- spec file cleanup

* Sun Jun 3 2007 Nikolay Pultsin <geometer@fbreader.org> - 0.8.4-1
- new upstream version
- removed gcc.patch
- added keynames.patch
- fixed homepage link

* Sat May 19 2007 Mikhail Sobolev <mss@mawhrin.net> - 0.8.3d-1
- new upstream version

* Mon May 14 2007 Mikhail Sobolev <mss@mawhrin.net> - 0.8.3c-1
- new upstream version
- fixed day of week in changelog

* Sun May 12 2007 Mikhail Sobolev <mss@mawhrin.net> - 0.8.3b-1
- new upstream version

* Thu May 10 2007 Mikhail Sobolev <mss@mawhrin.net> - 0.8.3-1
- new upstream version

* Mon Apr 9 2007 Mikhail Sobolev <mss@mawhrin.net> - 0.8.1d-1
- new upstream version

* Wed Mar 26 2007 Mikhail Sobolev <mss@mawhrin.net> - 0.8.1c-1
- new upstream version

* Wed Mar 26 2007 Mikhail Sobolev <mss@mawhrin.net> - 0.8.1b-1
- new upstream version

* Wed Mar 26 2007 Mikhail Sobolev <mss@mawhrin.net> - 0.8.1a-1
- new upstream version
- fixed wrong .so name for zlibrary
- added application pixmap to the list of included files
- added pixmap dir to the list of automatically included stuff
- desktop version now has its own .desktop file: include it, do not generate our own one
- now really include it

* Tue Dec 5 2006 Mikhail Sobolev <mss@mawhrin.net> - 0.7.4q-1
- new upstream version

* Mon Nov 27 2006 Mikhail Sobolev <mss@mawhrin.net> - 0.7.4p-1
- new upstream version

* Tue Nov 21 2006 Mikhail Sobolev <mss@mawhrin.net> - 0.7.4o-1
- new upstream version

* Mon Nov 06 2006 Mikhail Sobolev <mss@mawhrin.net> - 0.7.4n-3
- Introduced pepper_pad_2 variable that should indicate whether we build for Pepper Pad 2
- Cleaned up BuildRequires

* Mon Nov 06 2006 Mikhail Sobolev <mss@mawhrin.net> - 0.7.4n-2
- Merged the changes done by Victor Rehorst <chuma@chuma.org> to support Pepper Pad 2

* Sun Nov 05 2006 Mikhail Sobolev <mss@mawhrin.net> - 0.7.4n-1
- Trying new upstream version

* Sat Nov 04 2006 Mikhail Sobolev <mss@mawhrin.net> - 0.7.4m-3
- Included execution of ldconfig

* Sat Nov 04 2006 Mikhail Sobolev <mss@mawhrin.net> - 0.7.4m-2
- Updated based on Kad's comments

* Sat Nov 04 2006 Mikhail Sobolev <mss@mawhrin.net> - 0.7.4m-1
- Upgraded to version 0.7.4m

* Sat Nov 04 2006 Mikhail Sobolev <mss@mawhrin.net> - 0.7.4-2
- Modified spec to support gtk+ instead of qt

* Fri May 26 2006 Dries Verachtert <dries@ulyssis.org> - 0.7.4-1 - 4419+/dries
- Updated to release 0.7.4.

* Sun Mar 26 2006 Dries Verachtert <dries@ulyssis.org> - 0.7.3-1
- Updated to release 0.7.3.

* Fri Feb 17 2006 Dries Verachtert <dries@ulyssis.org> - 0.7.2-1
- Updated to release 0.7.2.

* Wed Dec 07 2005 Dries Verachtert <dries@ulyssis.org> - 0.7.1-0.b
- Initial package.
