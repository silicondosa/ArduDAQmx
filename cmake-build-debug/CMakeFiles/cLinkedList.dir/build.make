# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2018.2.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2018.2.2\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\tyros\codebase\DAQwrapper

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles\cLinkedList.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\cLinkedList.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\cLinkedList.dir\flags.make

CMakeFiles\cLinkedList.dir\sources\DAQwrapper.c.obj: CMakeFiles\cLinkedList.dir\flags.make
CMakeFiles\cLinkedList.dir\sources\DAQwrapper.c.obj: ..\sources\DAQwrapper.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/cLinkedList.dir/sources/DAQwrapper.c.obj"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\cLinkedList.dir\sources\DAQwrapper.c.obj /FdCMakeFiles\cLinkedList.dir\ /FS -c C:\Users\tyros\codebase\DAQwrapper\sources\DAQwrapper.c
<<

CMakeFiles\cLinkedList.dir\sources\DAQwrapper.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cLinkedList.dir/sources/DAQwrapper.c.i"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe > CMakeFiles\cLinkedList.dir\sources\DAQwrapper.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\tyros\codebase\DAQwrapper\sources\DAQwrapper.c
<<

CMakeFiles\cLinkedList.dir\sources\DAQwrapper.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cLinkedList.dir/sources/DAQwrapper.c.s"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\cLinkedList.dir\sources\DAQwrapper.c.s /c C:\Users\tyros\codebase\DAQwrapper\sources\DAQwrapper.c
<<

CMakeFiles\cLinkedList.dir\sources\cLinkedList.c.obj: CMakeFiles\cLinkedList.dir\flags.make
CMakeFiles\cLinkedList.dir\sources\cLinkedList.c.obj: ..\sources\cLinkedList.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/cLinkedList.dir/sources/cLinkedList.c.obj"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\cLinkedList.dir\sources\cLinkedList.c.obj /FdCMakeFiles\cLinkedList.dir\ /FS -c C:\Users\tyros\codebase\DAQwrapper\sources\cLinkedList.c
<<

CMakeFiles\cLinkedList.dir\sources\cLinkedList.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cLinkedList.dir/sources/cLinkedList.c.i"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe > CMakeFiles\cLinkedList.dir\sources\cLinkedList.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\tyros\codebase\DAQwrapper\sources\cLinkedList.c
<<

CMakeFiles\cLinkedList.dir\sources\cLinkedList.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cLinkedList.dir/sources/cLinkedList.c.s"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\cLinkedList.dir\sources\cLinkedList.c.s /c C:\Users\tyros\codebase\DAQwrapper\sources\cLinkedList.c
<<

CMakeFiles\cLinkedList.dir\sources\garbageCollector.c.obj: CMakeFiles\cLinkedList.dir\flags.make
CMakeFiles\cLinkedList.dir\sources\garbageCollector.c.obj: ..\sources\garbageCollector.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/cLinkedList.dir/sources/garbageCollector.c.obj"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\cLinkedList.dir\sources\garbageCollector.c.obj /FdCMakeFiles\cLinkedList.dir\ /FS -c C:\Users\tyros\codebase\DAQwrapper\sources\garbageCollector.c
<<

CMakeFiles\cLinkedList.dir\sources\garbageCollector.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cLinkedList.dir/sources/garbageCollector.c.i"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe > CMakeFiles\cLinkedList.dir\sources\garbageCollector.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\tyros\codebase\DAQwrapper\sources\garbageCollector.c
<<

CMakeFiles\cLinkedList.dir\sources\garbageCollector.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cLinkedList.dir/sources/garbageCollector.c.s"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\cLinkedList.dir\sources\garbageCollector.c.s /c C:\Users\tyros\codebase\DAQwrapper\sources\garbageCollector.c
<<

CMakeFiles\cLinkedList.dir\sources\listTestUtils.c.obj: CMakeFiles\cLinkedList.dir\flags.make
CMakeFiles\cLinkedList.dir\sources\listTestUtils.c.obj: ..\sources\listTestUtils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/cLinkedList.dir/sources/listTestUtils.c.obj"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\cLinkedList.dir\sources\listTestUtils.c.obj /FdCMakeFiles\cLinkedList.dir\ /FS -c C:\Users\tyros\codebase\DAQwrapper\sources\listTestUtils.c
<<

CMakeFiles\cLinkedList.dir\sources\listTestUtils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cLinkedList.dir/sources/listTestUtils.c.i"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe > CMakeFiles\cLinkedList.dir\sources\listTestUtils.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\tyros\codebase\DAQwrapper\sources\listTestUtils.c
<<

CMakeFiles\cLinkedList.dir\sources\listTestUtils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cLinkedList.dir/sources/listTestUtils.c.s"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\cLinkedList.dir\sources\listTestUtils.c.s /c C:\Users\tyros\codebase\DAQwrapper\sources\listTestUtils.c
<<

CMakeFiles\cLinkedList.dir\sources\main.c.obj: CMakeFiles\cLinkedList.dir\flags.make
CMakeFiles\cLinkedList.dir\sources\main.c.obj: ..\sources\main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/cLinkedList.dir/sources/main.c.obj"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\cLinkedList.dir\sources\main.c.obj /FdCMakeFiles\cLinkedList.dir\ /FS -c C:\Users\tyros\codebase\DAQwrapper\sources\main.c
<<

CMakeFiles\cLinkedList.dir\sources\main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cLinkedList.dir/sources/main.c.i"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe > CMakeFiles\cLinkedList.dir\sources\main.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\tyros\codebase\DAQwrapper\sources\main.c
<<

CMakeFiles\cLinkedList.dir\sources\main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cLinkedList.dir/sources/main.c.s"
	C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\cLinkedList.dir\sources\main.c.s /c C:\Users\tyros\codebase\DAQwrapper\sources\main.c
<<

# Object files for target cLinkedList
cLinkedList_OBJECTS = \
"CMakeFiles\cLinkedList.dir\sources\DAQwrapper.c.obj" \
"CMakeFiles\cLinkedList.dir\sources\cLinkedList.c.obj" \
"CMakeFiles\cLinkedList.dir\sources\garbageCollector.c.obj" \
"CMakeFiles\cLinkedList.dir\sources\listTestUtils.c.obj" \
"CMakeFiles\cLinkedList.dir\sources\main.c.obj"

# External object files for target cLinkedList
cLinkedList_EXTERNAL_OBJECTS =

cLinkedList.exe: CMakeFiles\cLinkedList.dir\sources\DAQwrapper.c.obj
cLinkedList.exe: CMakeFiles\cLinkedList.dir\sources\cLinkedList.c.obj
cLinkedList.exe: CMakeFiles\cLinkedList.dir\sources\garbageCollector.c.obj
cLinkedList.exe: CMakeFiles\cLinkedList.dir\sources\listTestUtils.c.obj
cLinkedList.exe: CMakeFiles\cLinkedList.dir\sources\main.c.obj
cLinkedList.exe: CMakeFiles\cLinkedList.dir\build.make
cLinkedList.exe: CMakeFiles\cLinkedList.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking C executable cLinkedList.exe"
	"C:\Program Files\JetBrains\CLion 2018.2.2\bin\cmake\win\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\cLinkedList.dir --manifests  -- C:\PROGRA~2\MICROS~4\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\link.exe /nologo @CMakeFiles\cLinkedList.dir\objects1.rsp @<<
 /out:cLinkedList.exe /implib:cLinkedList.lib /pdb:C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug\cLinkedList.pdb /version:0.0  /machine:X86 /debug /INCREMENTAL /subsystem:console kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<

# Rule to build all files generated by this target.
CMakeFiles\cLinkedList.dir\build: cLinkedList.exe

.PHONY : CMakeFiles\cLinkedList.dir\build

CMakeFiles\cLinkedList.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\cLinkedList.dir\cmake_clean.cmake
.PHONY : CMakeFiles\cLinkedList.dir\clean

CMakeFiles\cLinkedList.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" C:\Users\tyros\codebase\DAQwrapper C:\Users\tyros\codebase\DAQwrapper C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug C:\Users\tyros\codebase\DAQwrapper\cmake-build-debug\CMakeFiles\cLinkedList.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\cLinkedList.dir\depend

