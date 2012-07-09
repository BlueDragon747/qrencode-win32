
[Setup]
AppName=QRCodeGui
AppVersion=1.0
DefaultDirName={pf}\QRCodeGui
DefaultGroupName=QRCodeGui
UninstallDisplayIcon={app}\qrcodegui.exe
Compression=lzma2
SolidCompression=yes
LicenseFile=..\COPYING
OutputDir=.\.build\setup
OutputBaseFilename=qrcodegui_setup-3.3.1

[Files]
Source: "settings.ini"; DestDir: "{userappdata}\QRCodeGui"
Source: ".\.build\release-lib\qrcodegui.exe"; DestDir: "{app}"
Source: ".\.build\release-lib\qrcode.exe"; DestDir: "{app}"
Source: ".\test.bat"; DestDir: "{app}"

[Run]
Filename: "{app}\qrcodegui.exe"; Description: "Open QRCodeGui after finish"; Flags: postinstall

[Icons]
Name: "{group}\QRCodeGui"; Filename: "{app}\qrcodegui.exe"
