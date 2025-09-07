# Esterv.Network.Manager

[TOC]
This repo produce ready to use network components for QML.
Right now it uses [connman-dbus API](https://git.kernel.org/pub/scm/network/connman/connman.git/tree/doc) to list Wi-Fi networks, connect and disconnect from them.
In the future maybe with your help we will add network status and properties components.

## Dependencies

- Connman should be running and its DBus API working.
- Qt/QML libraries
- Glib

## Configure, build, test, package ...

The project uses [CMake presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) as a way to share CMake configurations.
Refer to [cmake](https://cmake.org/cmake/help/latest/manual/cmake.1.html), [ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html) and [cpack](https://cmake.org/cmake/help/latest/manual/cpack.1.html) documentation for more information on the use of presets.


## Adding the libraries to your CMake project

```CMake
include(FetchContent)
FetchContent_Declare(
	EstervNetworkManager
	GIT_REPOSITORY https://github.com/EddyTheCo/Esterv.Network.Manager.git
	GIT_TAG vMAJOR.MINOR.PATCH
	FIND_PACKAGE_ARGS MAJOR.MINOR CONFIG
	)
FetchContent_MakeAvailable(EstervNetworkManager)

target_link_libraries(<target> <PRIVATE|PUBLIC|INTERFACE> Esterv::NetworkManager)
target_link_libraries(<target> <PRIVATE|PUBLIC|INTERFACE> $<$<STREQUAL:$<TARGET_PROPERTY:Esterv::NetworkManager,TYPE>,STATIC_LIBRARY>:Esterv::NetworkManagerplugin>)


```

## API reference

You can read the [API reference](https://eddytheco.github.io/Esterv.Network.Manager), or generate it yourself like
```
cmake --workflow --preset default-documentation
```

## Contributing

We appreciate any contribution!

You can open an issue or request a feature.
You can open a PR to the `develop` branch and the CI/CD will take care of the rest.
Make sure to acknowledge your work, and ideas when contributing.

