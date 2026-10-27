#define MyAppName "Offor Stem Splitter"
#define MyAppVersion "1.1.0"
#define MyAppPublisher "ONNTECH"
#define MyAppExeName "Offor Stem Splitter.exe"

[Setup]
AppId={{8D8C7D91-5F45-4E42-9B8C-OFFORSTEMSPLIT}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\Offor Stem Splitter
DefaultGroupName=Offor Stem Splitter
DisableProgramGroupPage=yes

SetupIconFile=..\Resources\OfforStemSplitter.ico

OutputDir=.
OutputBaseFilename=Offor-Stem-Splitter-Setup-v{#MyAppVersion}

Compression=lzma2
SolidCompression=yes

ArchitecturesInstallIn64BitMode=x64compatible
ArchitecturesAllowed=x64compatible

WizardStyle=modern
LicenseFile=EULA.rtf

UninstallDisplayIcon={app}\{#MyAppExeName}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"

[Files]

; ==========================================================
; MAIN APPLICATION
; ==========================================================

Source: "Offor Stem Splitter\Offor Stem Splitter.exe"; \
    DestDir: "{app}"; \
    Flags: ignoreversion

; ==========================================================
; MANUAL
; ==========================================================

Source: "Offor Stem Splitter\Manual.pdf"; \
    DestDir: "{app}"; \
    Flags: ignoreversion

; ==========================================================
; END USER LICENSE AGREEMENT
; ==========================================================
Source: "EULA.rtf"; DestDir: "{app}"; Flags: ignoreversion

; ==========================================================
; OFFOR AI PYTHON ENVIRONMENT
; ==========================================================

Source: "Offor Stem Splitter\demucs-env\*";  \
  DestDir: "{app}\demucs-env"; \
  Flags: ignoreversion recursesubdirs createallsubdirs; \
  Excludes: "Lib\site-packages\*.dist-info\licenses\*"

; ==========================================================
; PORTABLE PYTHON RUNTIME
; ==========================================================
Source: "Offor Stem Splitter\demucs-runtime\*"; DestDir: "{app}\demucs-runtime"; Flags: ignoreversion recursesubdirs createallsubdirs

; ==========================================================
; DEMUCS ENGINE
; ==========================================================

Source: "Offor Stem Splitter\demucs-engine\*"; \
    DestDir: "{app}\demucs-engine"; \
    Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]

; ==========================================================
; START MENU
; ==========================================================

Name: "{group}\Offor Stem Splitter"; \
    Filename: "{app}\{#MyAppExeName}"

Name: "{group}\Offor Stem Splitter Manual"; \
    Filename: "{app}\Manual.pdf"

; ==========================================================
; DESKTOP
; ==========================================================

Name: "{autodesktop}\Offor Stem Splitter"; \
    Filename: "{app}\{#MyAppExeName}"; \
    Tasks: desktopicon

[Run]

Filename: "{app}\{#MyAppExeName}"; \
    Description: "Launch Offor Stem Splitter"; \
    Flags: nowait postinstall skipifsilent