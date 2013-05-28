Name:       capi-location-geocoder
Summary:    A Geocoder library in Tizen Native API
Version:    0.1.2
Release:    1
Group:      System/Libraries
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(location)
BuildRequires:  pkgconfig(capi-base-common)

%description
A Geocoder library in Tizen Native API.


%package devel
Summary:  A Geocoder library in Tizen Native API (Development)
Group:    Development/Libraries
Requires: %{name} = %{version}

%description devel
A Geocoder library in Tizen Native API (Development).


%prep
%setup -q


%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER}
make %{?jobs:-j%jobs}

%install
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%license LICENSE
%{_libdir}/libcapi-location-geocoder.so*
%manifest capi-location-geocoder.manifest

%files devel
%{_includedir}/location/geocoder.h
%{_libdir}/pkgconfig/*.pc
