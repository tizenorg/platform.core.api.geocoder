Name:       capi-location-geocoder
Summary:    A Geocoder library in Tizen Native API
Version: 0.1.0
Release:    14
Group:      API/C API
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(location)
BuildRequires:  pkgconfig(capi-base-common)

%description
A Geocoder library in Tizen Native API

%package devel
Summary:  A Geocoder library in Tizen Native API (Development)
Group:    TO_BE/FILLED_IN
Requires: %{name} = %{version}-%{release}

%description devel
A Geocoder library in Tizen Native API. 
Development Files



%prep
%setup -q


%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake .  -DFULLVER=%{version} -DMAJORVER=${MAJORVER}


make %{?jobs:-j%jobs}

%install
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_libdir}/libcapi-location-geocoder.so*

%files devel
%{_includedir}/location/geocoder.h
%{_libdir}/pkgconfig/*.pc
