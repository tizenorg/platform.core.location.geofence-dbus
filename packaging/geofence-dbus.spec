Name: geofence-dbus
Summary: Dbus interface for Geofence service
Version: 0.3.2
Release:    1
Group:      Location/Libraries
License:    Apache-2.0
Source0:    geofence-dbus-%{version}.tar.gz

%if "%{?profile}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gobject-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(gio-2.0)
BuildRequires:  pkgconfig(gio-unix-2.0)
BuildRequires:  pkgconfig(capi-base-common)

%description
Geofence dbus interface

%package -n libgeofence-dbus
Summary:    Geofence dbus library
Group:      Location/Libraries
Requires(post): sys-assert

%description -n libgeofence-dbus
Geofence client API library

%package -n libgeofence-dbus-devel
Summary:    Telephony client API (devel)
Group:      Development/Libraries
Requires:   libgeofence-dbus = %{version}-%{release}

%description -n libgeofence-dbus-devel
Geofence client API library (devel)


%prep
%setup -q


%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
export CFLAGS+=" -Wno-unused-local-typedefs "
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DLIBDIR=%{_libdir}\

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post -n libgeofence-dbus -p /sbin/ldconfig

%postun -n libgeofence-dbus -p /sbin/ldconfig


%files -n libgeofence-dbus
%manifest libgeofence-dbus.manifest
%defattr(-,root,root,-)
#%doc COPYING
%{_libdir}/*.so.*
%{_prefix}/etc/dbus-1/system.d/*

%files -n libgeofence-dbus-devel
%defattr(-,root,root,-)
%{_includedir}/geofence-dbus/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/*.so
