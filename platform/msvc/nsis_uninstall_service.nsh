  SectionGetFlags ${StableDiffusionGUIServer} $0
  IntOp $0 $0 & ${SF_SELECTED}
  ${If} $0 == ${SF_SELECTED}
    ;ExecWait 'sc.exe delete "StableDiffusionServer"'
    ExecWait '$INSTDIR/WinSW-x64.exe uninstall $INSTDIR/winsw.xml'
    MessageBox MB_OK "Server service uninstalled!"
  ${EndIf}