    SectionGetFlags ${StableDiffusionGUIServer} $0
    IntOp $0 $0 & ${SF_SELECTED}
    ${If} $0 == ${SF_SELECTED}
      ;ExecWait 'sc.exe create "StableDiffusionGUIServer" displayname= "Stable Diffusion GUI Server" binPath= "\"$INSTDIR\stablediffusiongui_server.exe\" \"$INSTDIR\server.json\"" start= auto'
      ExecWait '$INSTDIR/WinSW-x64.exe install $INSTDIR/winsw.xml'
      MessageBox MB_OK "Server service installed!"
    ${EndIf}