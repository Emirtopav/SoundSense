[Setup]
; Basic Application Information
AppName=SoundSense
AppVersion=1.2.0
AppPublisher=Emir Topav
AppPublisherURL=https://github.com/Emirtopav/SoundSense
AppSupportURL=https://github.com/Emirtopav/SoundSense
AppUpdatesURL=https://github.com/Emirtopav/SoundSense

; Default installation folder
DefaultDirName={autopf}\SoundSense
DefaultGroupName=SoundSense
AllowNoIcons=yes

; Output Installer Settings
OutputDir=.\Output
OutputBaseFilename=SoundSense_Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

; Icon for the installer
SetupIconFile=SoundSenseCpp\logo.png

; Requires Windows 10 or later
MinVersion=10.0.10240

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "autostart"; Description: "Start SoundSense automatically when Windows starts"; GroupDescription: "Windows Startup:"

[Files]
; Main Executable and all dependencies from dist folder
Source: "SoundSenseCpp\dist\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; Documentation
Source: "README.md"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
; Start Menu
Name: "{group}\SoundSense"; Filename: "{app}\SoundSenseCpp.exe"; IconFilename: "{app}\logo.png"
Name: "{group}\{cm:UninstallProgram,SoundSense}"; Filename: "{uninstallexe}"
; Desktop Shortcut
Name: "{autodesktop}\SoundSense"; Filename: "{app}\SoundSenseCpp.exe"; Tasks: desktopicon; IconFilename: "{app}\logo.png"
; Auto Start Shortcut
Name: "{userstartup}\SoundSense"; Filename: "{app}\SoundSenseCpp.exe"; Tasks: autostart; IconFilename: "{app}\logo.png"

[Run]
Filename: "{app}\SoundSenseCpp.exe"; Description: "{cm:LaunchProgram,SoundSense}"; Flags: nowait postinstall skipifsilent
