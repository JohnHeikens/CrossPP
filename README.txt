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

TODO:

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