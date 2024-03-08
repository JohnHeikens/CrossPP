[Setup]
AppName=Medieval Survival
AppVersion=1.0
ArchitecturesInstallIn64BitMode=x64
DefaultDirName={pf}\Medieval Survival
DefaultGroupName=Medieval Survival
UninstallDisplayIcon={app}\MedievalSurvival.exe
OutputDir=Output
OutputBaseFilename=Medieval Survival - Installer
WizardResizable=yes
WizardStyle=modern

[Files]
Source: "x64/release/MedievalSurvival.exe"; DestDir: "{app}"
Source: "x64/release/*.dll"; DestDir: "{app}"
Source: "x64/release/*.pdb"; DestDir: "{app}"
Source: "Data\*"; DestDir: "{commonappdata}\JohnHeikens\Medieval Survival\data\"; Flags: recursesubdirs; Excludes: "Data\player\playerdata.dat,saves,*.class,*.zip"; Permissions: users-full

[Icons]
Name: "{group}\Medieval Survival"; Filename: "{app}\MedievalSurvival.exe"

[Run]
Filename: "{app}\MedievalSurvival.exe"; Flags:runasoriginaluser nowait postinstall skipifsilent