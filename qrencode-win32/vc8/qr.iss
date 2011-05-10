
[Setup]
AppName=QRCodeGui
AppVersion=1.0
DefaultDirName={pf}\QRCodeGui
DefaultGroupName=QRCodeGui
UninstallDisplayIcon={app}\qrcodegui.exe
Compression=lzma2
SolidCompression=yes
LicenseFile=..\COPYING
OutputDir=release lib
OutputBaseFilename=qrcodegui_setup-1.0

[Files]
Source: "settings.ini"; DestDir: "{userappdata}\QRCodeGui"
Source: "release lib\qrcodegui.exe"; DestDir: "{app}"
Source: "release lib\qrcode.exe"; DestDir: "{app}"

[Run]
Filename: "{app}\qrcodegui.exe"; Description: "Open QRCodeGui after finish"; Flags: postinstall

[Icons]
Name: "{group}\QRCodeGui"; Filename: "{app}\qrcodegui.exe"
