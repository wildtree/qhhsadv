; ハイハイスクールアドベンチャー インストーラ (Inno Setup)

[Setup]
AppName=High High School Adventure Qt
AppVersion=1.1.0
AppPublisher=WildTreeJP
DefaultDirName={commonpf}\QHHSAdv
DefaultGroupName=ハイハイスクールアドベンチャー
UninstallDisplayIcon={app}\qhhsadv.exe
OutputBaseFilename=qhhsadv_setup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
; Qtアプリ本体と依存DLLをまとめたフォルダを指定
Source: "deploy\qhhsadv.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "deploy\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs ignoreversion

; リソースディレクトリ
Source: "deploy\data\*"; DestDir: "{app}\data"; Flags: recursesubdirs createallsubdirs ignoreversion

[Icons]
Name: "{group}\ハイハイスクールアドベンチャー"; Filename: "{app}\qhhsadv.exe"
Name: "{commondesktop}\ハイハイスクールアドベンチャー"; Filename: "{app}\qhhsadv.exe"; Tasks: desktopicon

[Run]
; インストール完了後に起動するオプション
Filename: "{app}\qhhsadv.exe"; Description: "{cm:LaunchProgram,ハイハイスクールアドベンチャー}"; Flags: nowait postinstall skipifsilent
