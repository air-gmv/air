isSubcomponent('interfaceview::IV','gr740_candriver','others','flagsToCompileWith','DATA','DataView::Taste_directive','NIL','NIL','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','others','flagsToCompileWith','Taste::FS_Default_Value','"compiler-option: ""-I/opt/rtems-5.1-2018.03.08/sparc-rtems5/gr740/lib/include -DCONFIGURE_DRIVER_AMBAPP_GAISLER_GRCAN"""','').
isComponentType('interfaceview::FV::gr740_candriver','PUBLIC','PI_update','SUBPROGRAM','NIL','').
isComponentImplementation('interfaceview::FV::gr740_candriver','PUBLIC','PI_update','others','SUBPROGRAM','NIL','others','').
isFeature('ACCESS','interfaceview::IV','gr740_candriver','PI_update','PROVIDES','SUBPROGRAM','interfaceview::FV::gr740_candriver::PI_update.others','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::gr740_candriver','PI_update','NIL','NIL','Taste::Associated_Queue_Size','1','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','PI_update','Taste::coordinates','"106454 72283"','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','PI_update','Taste::RCMoperationKind','cyclic','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','PI_update','Taste::RCMperiod','1000 ms','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','PI_update','Taste::Deadline','0 ms','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','PI_update','Taste::InterfaceName','"update"','').
isProperty('NIL','=>','interfaceview::FV::gr740_candriver','PI_update','others','NIL','Compute_Execution_Time','0 ms .. 0 ms','').
isSubcomponent('interfaceview::IV','gr740_candriver','others','update_impl','SUBPROGRAM','interfaceview::FV::gr740_candriver::PI_update.others','NIL','NIL','').
isConnection('SUBPROGRAM ACCESS','interfaceview::IV','gr740_candriver','others','OpToPICnx_update_impl','update_impl','->','PI_update','NIL','').
isConnection('SUBPROGRAM ACCESS','interfaceview::IV','interfaceview','others','gr740_candriver_PI_commands_candriver_test_RI_commands','gr740_candriver.PI_commands','->','candriver_test.RI_commands','NIL','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','others','gr740_candriver_PI_commands_candriver_test_RI_commands','Taste::coordinates','"75903 56220 91178 56220 91178 56380 106454 56380"','').
isComponentType('interfaceview::FV::gr740_candriver','PUBLIC','PI_commands','SUBPROGRAM','NIL','').
isComponentImplementation('interfaceview::FV::gr740_candriver','PUBLIC','PI_commands','others','SUBPROGRAM','NIL','others','').
isFeature('ACCESS','interfaceview::IV','gr740_candriver','PI_commands','PROVIDES','SUBPROGRAM','interfaceview::FV::gr740_candriver::PI_commands.others','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::gr740_candriver','PI_commands','NIL','NIL','Taste::Associated_Queue_Size','1','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','PI_commands','Taste::coordinates','"106454 56380"','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','PI_commands','Taste::RCMoperationKind','sporadic','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','PI_commands','Taste::RCMperiod','0 ms','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','PI_commands','Taste::Deadline','0 ms','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','PI_commands','Taste::InterfaceName','"commands"','').
isFeature('PARAMETER','interfaceview::FV::gr740_candriver','PI_commands','cmds','IN','NIL','DataView::T_Int32','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::gr740_candriver','PI_commands','NIL','cmds','Taste::encoding','NATIVE','').
isProperty('NIL','=>','interfaceview::FV::gr740_candriver','PI_commands','others','NIL','Compute_Execution_Time','0 ms .. 0 ms','').
isSubcomponent('interfaceview::IV','gr740_candriver','others','commands_impl','SUBPROGRAM','interfaceview::FV::gr740_candriver::PI_commands.others','NIL','NIL','').
isConnection('SUBPROGRAM ACCESS','interfaceview::IV','gr740_candriver','others','OpToPICnx_commands_impl','commands_impl','->','PI_commands','NIL','').
isConnection('SUBPROGRAM ACCESS','interfaceview::IV','interfaceview','others','candriver_test_PI_samples_gr740_candriver_RI_samples','candriver_test.PI_samples','->','gr740_candriver.RI_samples','NIL','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','others','candriver_test_PI_samples_gr740_candriver_RI_samples','Taste::coordinates','"106454 77010 91178 77010 91178 77641 75903 77641"','').
isComponentType('interfaceview::FV::gr740_candriver','PUBLIC','RI_samples','SUBPROGRAM','NIL','').
isComponentImplementation('interfaceview::FV::gr740_candriver','PUBLIC','RI_samples','others','SUBPROGRAM','NIL','others','').
isImportDeclaration('interfaceview::IV','PUBLIC','interfaceview::FV::candriver_test','').
isFeature('ACCESS','interfaceview::IV','gr740_candriver','RI_samples','REQUIRES','SUBPROGRAM','interfaceview::FV::candriver_test::PI_samples.others','NIL','NIL','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','RI_samples','Taste::coordinates','"106454 77010"','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','RI_samples','Taste::RCMoperationKind','any','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','RI_samples','Taste::InterfaceName','"samples"','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','RI_samples','Taste::labelInheritance','"true"','').
isFeature('PARAMETER','interfaceview::FV::gr740_candriver','RI_samples','smpls','IN','NIL','DataView::T_Int32','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::gr740_candriver','RI_samples','NIL','smpls','Taste::encoding','NATIVE','').
isPackage('interfaceview::FV::gr740_candriver','PUBLIC','').
isComponentType('interfaceview::IV','PUBLIC','gr740_candriver','SYSTEM','NIL','').
isComponentImplementation('interfaceview::IV','PUBLIC','gr740_candriver','others','SYSTEM','NIL','others','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','NIL','Source_Language','(C)','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','NIL','Taste::Active_Interfaces','any','').
isProperty('NIL','=>','interfaceview::IV','gr740_candriver','NIL','NIL','Source_Text','("gr740_candriver.zip")','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','others','gr740_candriver','Taste::coordinates','"106454 48660 149445 82517"','').
isSubcomponent('interfaceview::IV','interfaceview','others','gr740_candriver','SYSTEM','interfaceview::IV::gr740_candriver.others','NIL','NIL','').
isImportDeclaration('interfaceview::IV','PUBLIC','interfaceview::FV::gr740_candriver','').
isImportDeclaration('interfaceview::IV','PUBLIC','Taste','').
isImportDeclaration('interfaceview::FV::gr740_candriver','PUBLIC','Taste','').
isImportDeclaration('interfaceview::IV','PUBLIC','DataView','').
isImportDeclaration('interfaceview::FV::gr740_candriver','PUBLIC','DataView','').
isImportDeclaration('interfaceview::FV::gr740_candriver','PUBLIC','TASTE_IV_Properties','').
isImportDeclaration('interfaceview::IV','PUBLIC','TASTE_IV_Properties','').
isComponentType('interfaceview::FV::candriver_test','PUBLIC','PI_samples','SUBPROGRAM','NIL','').
isComponentImplementation('interfaceview::FV::candriver_test','PUBLIC','PI_samples','others','SUBPROGRAM','NIL','others','').
isFeature('ACCESS','interfaceview::IV','candriver_test','PI_samples','PROVIDES','SUBPROGRAM','interfaceview::FV::candriver_test::PI_samples.others','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::candriver_test','PI_samples','NIL','NIL','Taste::Associated_Queue_Size','1','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','PI_samples','Taste::coordinates','"75903 77641"','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','PI_samples','Taste::RCMoperationKind','sporadic','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','PI_samples','Taste::RCMperiod','0 ms','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','PI_samples','Taste::Deadline','0 ms','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','PI_samples','Taste::InterfaceName','"samples"','').
isFeature('PARAMETER','interfaceview::FV::candriver_test','PI_samples','smpls','IN','NIL','DataView::T_Int32','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::candriver_test','PI_samples','NIL','smpls','Taste::encoding','NATIVE','').
isProperty('NIL','=>','interfaceview::FV::candriver_test','PI_samples','others','NIL','Compute_Execution_Time','0 ms .. 0 ms','').
isSubcomponent('interfaceview::IV','candriver_test','others','samples_impl','SUBPROGRAM','interfaceview::FV::candriver_test::PI_samples.others','NIL','NIL','').
isConnection('SUBPROGRAM ACCESS','interfaceview::IV','candriver_test','others','OpToPICnx_samples_impl','samples_impl','->','PI_samples','NIL','').
isComponentType('interfaceview::FV::candriver_test','PUBLIC','RI_commands','SUBPROGRAM','NIL','').
isComponentImplementation('interfaceview::FV::candriver_test','PUBLIC','RI_commands','others','SUBPROGRAM','NIL','others','').
isFeature('ACCESS','interfaceview::IV','candriver_test','RI_commands','REQUIRES','SUBPROGRAM','interfaceview::FV::gr740_candriver::PI_commands.others','NIL','NIL','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','RI_commands','Taste::coordinates','"75903 56220"','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','RI_commands','Taste::RCMoperationKind','any','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','RI_commands','Taste::InterfaceName','"commands"','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','RI_commands','Taste::labelInheritance','"true"','').
isFeature('PARAMETER','interfaceview::FV::candriver_test','RI_commands','cmds','IN','NIL','DataView::T_Int32','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::candriver_test','RI_commands','NIL','cmds','Taste::encoding','NATIVE','').
isPackage('interfaceview::FV::candriver_test','PUBLIC','').
isComponentType('interfaceview::IV','PUBLIC','candriver_test','SYSTEM','NIL','').
isComponentImplementation('interfaceview::IV','PUBLIC','candriver_test','others','SYSTEM','NIL','others','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','NIL','Source_Language','(C)','').
isProperty('NIL','=>','interfaceview::IV','candriver_test','NIL','NIL','Taste::Active_Interfaces','any','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','others','candriver_test','Taste::coordinates','"31495 48346 75903 82675"','').
isSubcomponent('interfaceview::IV','interfaceview','others','candriver_test','SYSTEM','interfaceview::IV::candriver_test.others','NIL','NIL','').
isImportDeclaration('interfaceview::FV::candriver_test','PUBLIC','Taste','').
isImportDeclaration('interfaceview::FV::candriver_test','PUBLIC','DataView','').
isImportDeclaration('interfaceview::FV::candriver_test','PUBLIC','TASTE_IV_Properties','').
isProperty('_','_','_','_','_','_','LMP::Unparser_ID_Case','AsIs','').
isProperty('_','_','_','_','_','_','LMP::Unparser_Insert_Header','Yes','').
isPackage('interfaceview::IV','PUBLIC','').
isProperty('NIL','=>','interfaceview::IV','NIL','NIL','NIL','Taste::dataView','("DataView")','').
isProperty('NIL','=>','interfaceview::IV','NIL','NIL','NIL','Taste::dataViewPath','("gr740-candriver_dv.aadl")','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','NIL','NIL','Taste::dataView','("DataView")','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','NIL','NIL','Taste::dataViewPath','("gr740-candriver_dv.aadl")','').
isVersion('AADL2.1','TASTE type interfaceview','','generated code: do not edit').
isProperty('NIL','=>','interfaceview::IV','NIL','NIL','NIL','Taste::coordinates','"0 0 297000 210000"','').
isProperty('NIL','=>','interfaceview::IV','NIL','NIL','NIL','Taste::version','"1.3"','').
isComponentType('interfaceview::IV','PUBLIC','interfaceview','SYSTEM','NIL','').
isComponentImplementation('interfaceview::IV','PUBLIC','interfaceview','others','SYSTEM','NIL','others','').

