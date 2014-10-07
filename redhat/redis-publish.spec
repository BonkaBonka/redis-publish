Summary: A simple utility to publish a message to a Redis channel.
Name: redis-publish
Version: 0.1.0
Release: 1
Group: Applications/Communications
License: MIT
Packager: James Cook <bonkabonka@gmail.com>
URL: https://github.com/BonkaBonka/redis-publish
Source: https://github.com/BonkaBonka/redis-publish/archive/%{version}.tar.gz
BuildRequires: git

%description
A simple utility to publish a message to a Redis channel.

%prep
%setup -q

%build
make

%install
PREFIX=$RPM_BUILD_ROOT/usr make install

%clean

%files
%defattr(-,bin,bin)
%{_bindir}/*
%doc %_mandir/man1/*
