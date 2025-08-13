# platform-cmake

This folder contains a collection of useful CMake modules for working with EVE's C++ codebase outside the
Perforce monolith.

## Usage

Simply use [CMake's include](https://cmake.org/cmake/help/latest/command/include.html) command to include the things
you would like to use. 

## Available CMake modules

- CcpBuildConfigurations.cmake: defines supported build configurations with all relevant compiler switches; also known as [flavors](https://wiki.ccpgames.com/pages/viewpage.action?pageId=167285354#Glossary-Flavor)
- CcpPacIdentifiers.cmake: provides variables containing our platform, architecture and compiler identifiers
- PyTestDiscoverTests.cmake: allows running pytest tests through ctest
