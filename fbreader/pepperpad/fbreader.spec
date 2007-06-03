# Upstream: Nikolay Pultsin <geometer$mawhrin,net>

%if "%{_vendor}" == "montavista"
%define release_suffix .chuma.pepper
%define pepper_pad_2 1
%else
%define release_suffix %{?dist}
%define pepper_pad_2 0
%endif

%define desktop_vendor rpmforge

Summary: E-book reader
Name: fbreader
Version: 0.8.4
Release: 1%{release_suffix}
License: GPL
Group: Applications/Multimedia
URL: http://www.fbreader.org/

Packager: Nikolay Pultsin <geometer@mawhrin.net>

Source: http://www.fbreader.org/fbreader-sources-%{version}.tgz
Patch0: rpath.patch
Patch1: keynames.patch
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: enca-devel, zlib-devel, libpng-devel, gtk2-devel
%if %{pepper_pad_2}
BuildRequires: bzip2-dev, expat, libjpeg-dev
%else
BuildRequires: desktop-file-utils
BuildRequires: bzip2-devel, expat-devel, libjpeg-devel
%endif

%description
FBReader is an e-book reader for Linux PDAs and desktop computers.
FBReader supports several e-book formats: plucker, palmdoc, zTXT, 
HTML, fb2, plain text, etc.

%prep
%setup
%patch0 -p1
%patch1 -p1

#%if %{!pepper_pad_2}
#%{__cat} <<EOF >fbreader.desktop
#[Desktop Entry]
#Name=FBReader
#Comment=Read various ebook formats
#Exec=FBReader
#Terminal=false
#Type=Application
#StartupNotify=true
#Categories=Application;AudioVideo;
#EOF
#%endif

%build
%{__make} %{?_smp_mflags} INSTALLDIR=%{_prefix} TARGET_ARCH=desktop UI_TYPE=gtk TARGET_STATUS=release

%install
%{__rm} -rf %{buildroot}
%makeinstall INSTALLDIR=%{_prefix} TARGET_ARCH=desktop UI_TYPE=gtk TARGET_STATUS=release DESTDIR=%{buildroot}

#%{__install} -d -m0755 %{buildroot}%{_datadir}/applications/
#%if %{!pepper_pad_2}
#desktop-file-install --vendor %{desktop_vendor}    \
#	--add-category X-Red-Hat-Base              \
#	--dir %{buildroot}%{_datadir}/applications \
#	fbreader.desktop
#%endif

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%{_bindir}/FBReader
%{_datadir}/FBReader
%{_datadir}/pixmaps/FBReader.png
%{_datadir}/pixmaps/FBReader
%{_datadir}/applications/FBReader.desktop
#%if %{!pepper_pad_2}
#%{_datadir}/applications/%{desktop_vendor}-fbreader.desktop
#%endif
%{_libdir}/libzlibrary-gtk.so.%{version}
%{_datadir}/zlibrary

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%changelog
* Sun Jun 3 2007 Nikolay Pultsin <geometer@mawhrin.net> - 0.8.4-1
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
