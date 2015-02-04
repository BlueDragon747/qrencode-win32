
[Setup]
AppName=QRCodeGui
AppVersion=3.4.4.0
DefaultDirName={pf}\QRCodeGui
DefaultGroupName=QRCodeGui
UninstallDisplayIcon={app}\qrcodegui.exe
Compression=lzma2
SolidCompression=yes
LicenseFile=..\COPYING
OutputDir=.\.build\setup
OutputBaseFilename=qrcodegui_setup-3.4.4

[Files]
Source: "settings.ini"; DestDir: "{userappdata}\QRCodeGui"
Source: ".\.build\release-lib\qrcodegui.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\.build\release-lib\qrcodecon.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\.build\release-lib\qrcode.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\test.bat"; DestDir: "{app}"; Flags: ignoreversion

[Run]
Filename: "{app}\qrcodegui.exe"; Description: "Open QRCodeGui after finish"; Flags: postinstall

[Icons]
Name: "{group}\QRCodeGui"; Filename: "{app}\qrcodegui.exe"
