todo:

neuralnetwork. h and . cpp
coding tips:
argument list missing after assumed function template: template file not even included!! (vectn)
go past all TODO's

serializevalue throwing static assert: fp changed so long double used instead of double (no serialization method for long double

redefinition different basic types: you used auto
redefinition: pragma once missing

linker error unresolved external symbol? make inline
base class undefined: circular dependencies
gzip reinterpret cast error: define ZLIB_CONST before inclusion

linker error already defined in obj: you #included a '.cpp' file instead of a '.h' file
or you initialized an extern in the declaration
compilation slow: turn 'multi procession compilation' to 'yes' in the project settings. not in the soluition settings!
unresolved external: cpp file not added to solution?

unresolved external symbol (linker errors) with sfml? did you check if someone #set(SFML_BUILD_AUDIO FALSE) ?
also, 
target_link_libraries(${PROJECT_NAME} PRIVATE 
sfml-audio
sfml-network
sfml-graphics
sfml-system
sfml-window
)

if you need a library like zlib, you can also install it with vcpkg.
vcpkg install zlib
it will give instructions on what to put in cmakelists.txt!

'uint32_t' is not a member of 'std':
#include <cstdint> not <stdint> (stdint has no std:: namespace in front of it)

no #include errors:
search for this setting: toggle intellisense engine fallback on include errors

Cyclic build dependency detected: IItemComparable.h depends on slabType.h depends on IItemComparable.h
exclude the files from the project, then reinclude them

installer not creating new files: maybe the installer script you're looking at is in another location!

same goes for connection issues: maybe your project isn't updated enough

no copy constructor available or copy constructor is declared 'explicit': don't only check return values. check all passed values! in lambdas they should have an '&' in front

selector issues: a socket can't be in multiple selectors!

delete gets called randomly: do NOT copy, but create a reference! const &

why is serializeValue not const? because when const is used, serializeValue() can not read.
why is it not a template then with read or write? because template functions can't be overloaded or virtual

access violation, but only in release mode? maybe you are keeping a const reference as a class member while in release the original variable gets optimized away

loottables not the same as displayed: other loottable folder. check path!

warning conversion from 'this' to 'that' with constexpr variable: the line of the error is not the correct line. the error was created where it was used

issue with shared ptr destruction throwing: use std::make_shared<t> instead of std::shared_ptr<t>(new t())

to move a file while keeping it included:
in the solution explorer, click on the 'show all files' button. then it will show the folder view without having to switch views.

setup project:
https://github.com/lutrarutra/SFML-Project-Template/blob/master/README.md

problems with mingw not found in path:
edit environment variables for your account (see link above)

Unable to determine what CMake generator to use. Please install or configure a preferred generator, or update settings.json, your Kit configuration or PATH variable. Error: No usable generator found:
https://stackoverflow.com/questions/70197831/unable-to-determine-what-cmake-generator-to-use-vs-code-windows-10

how to deploy / create an installer:

https://learn.microsoft.com/en-us/cpp/ide/walkthrough-deploying-your-program-cpp?view=msvc-170

how to add a folder to the installer:
https://stackoverflow.com/questions/46493023/add-custom-folder-with-microsoft-visual-studio-installer-projects
In the File System view in the setup project, right click that File System node, then Add Special Folder, then Custom Folder. In the properties window of that custom folder in DefaultLocation put [WindowsVolume] (it's not in the drop-down list).

how to set up precompiled headers:

https://stackoverflow.com/questions/12434123/how-to-include-the-stdafx-h-from-the-root-directory

how to disable warnings from external headers, including imageloader.h:
https://developercommunity.visualstudio.com/t/pragma-warningpush-0-no-longer-works/1433518

to include sfml:
use vcpkg (click on an include directive to see)

do NOT put code in your onedrive documents folder! once it's full, it basically plays a ransom game where you either heve to buy more storage or lose your files after 30 days.
to fix, delete the ipch files and the installer EXE and empty the release and debug output folders

debug heap (memory, access violation etc) errors: use application verifier

how to create an installer: use inno setup. it's very simple!

dll not found in visual studio itself, out of nowhere: clean and rebuild solution

how to manage nuget packages: project -> manage nuget packages (doesn't seem to have much for c++)

etw collection is currently in progress:
https://stackoverflow.com/questions/16724041/cannot-start-concurrency-visualizer-in-visual-studio-2012-got-error-unable-to

include file problems: specify include folder both in the c_cpp_properties.json and using include_directories() in CMakeLists.txt

when having an empty build folder:
run CMake: configure

no builtin-baseline:
https://stackoverflow.com/questions/75184879/builtin-baseline-in-vcpkg-for-installing-franka-matlab
run vcpkg x-update-baseline --add-initial-baseline

when installing vcpkg packages, browse on https://vcpkg.io/en/packages to find them and their version!

follow instructions of the packages! look what INCLUDE_DIRS they need on the command line!

fatal error RC1107: invalid usage; use RC /? for Help:
you included resource (.rc) files?

in extensions: set extra arguments to -G "MinGW Makefiles"

for now disable the automatic recompile

also set additional compilers to search for to C:\msys64\urct64\bin

powershell problems and debugger not breaking when std::terminate() is called:
https://github.com/microsoft/vscode-cpptools/issues/9677

old make command:
ninja.exe CXXFLAGS=\"-g\" --version -j12"

https://code.visualstudio.com/docs/cpp/cpp-debug:
 To debug your Cygwin or MinGW application, add the miDebuggerPath property and set its value to the location of the corresponding gdb.exe for your Cygwin or MinGW environment.

For example:

    "miDebuggerPath": "c:\\mingw\\bin\\gdb.exe"

https://code.visualstudio.com/docs/cpp/cpp-debug:
Debugging
Windows:

GDB on Cygwin and MinGW cannot break a running process. To set a breakpoint when the application is running (not stopped under the debugger), or to pause the application being debugged, press Ctrl-C in the application's terminal.
GDB on Cygwin cannot open core dumps.

(changed to clang)

add support for mingw breakpoints:
MSYS2/Cygwin/MinGW/Clang support extension

add ninja:
mingw-w64-x86_64-ninja
to msys2

cmake set to debug:
"command": "cmake .. -G Ninja -DCMAKE_BUILD_TYPE:STRING=Debug"

linker.exe exits with code 5:
remove all other compiler paths from PATH

debug spitting library unload messages:
seems to have to do with gl context creation. create as few as possible and reuse contexts!

facts about sfml:
rendertextures don't have their own contexts, they use the current context and bind to that

setactive() must also be called when you don't use openGL but use multithreading

it's better to have all openGL rendering in one thread! multiple rendering threads using OpenGL won't give any speedup, if they even work!
for now, let's just use the CPU for rendering. later we could try to create a single render thread (which renders for server and client) and send all the data to that thread which executes it.

ERROR: Unable to start debugging. GDB exited unexpectedly.
to debug this, try running the exe by double clicking on it (not with a debugger)

c++ standard:
change in CMakeLists.txt : 
set(CMAKE_CXX_STANDARD ##) with ## being version number

also change in c_cpp_properties.json

type/value mismatch: f.e. use std::remove_const_t instead of std::remove_const::value

error with std::chrono::system_clock::now: use steady_clock everywhere 

vs code intellisense lagging: another extension is causing it. disable all extensions and see what extension causes it

Android studio:
renamed something, but databinding gives errors:
- rebuild
- file-> invalidate caches and restart

libmedievalsurvival.so not found:
so files are for example in app\build\intermediates\cxx\Debug\235d3j5x\obj\arm64-v8a

ANativeActivity_OnCreate unresolved symbol while the basic functions of sfml get loaded: make sure to link sfml-main using -Wl --whole-archive!

error when calling something from JNI: check logcat!!

cannot open a certain folder in the device explorer: make sure to simulate a device without a google system image (just use a normal android image like upsidedowncake)

problem with a base function not behaving the same way as the parent function: the parent function is derived!!

android studio not updating info about c++ function template: delete the function call (ctrl + x) and let it reappear (ctrl + v or ctrl + z)

error running app: build type isn't debuggable: when built in release mode. select build - > build variants - > debug to debug

disable the app being restarted when rotating:
things you add to configchanges will not close the app when they happen.
            android:configChanges="orientation|screenSize|screenLayout|keyboardHidden"

TODO:

maybe add vcpkg-configuration.json for version consistency (see https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd)

add cmake-toolchain for cross-compile

bonemeal particles
creeper ontploft in gmc
knockback in gmc
fix textures: when you summon a blaze in the end, the texture changes
make the world save when you close the game / autosave
gui textures don't work when they are 2x as big or small
ender dragon dying beams uses worldrandom instead of currentrandom
rename some conditions to have 'condition' behind it
place ruined portal function 
stemPlantGrowthStageCount
gameModeID.h name
save enchantment random in player data
middle click in creative: pick block
stop main menu music from playing when in game
dropping items
wind sounds
legs swinging too fast
wind too fast
enderman should become mad when slapped (anger sound)
remove AI



philadelphia
punten doorgestuurd