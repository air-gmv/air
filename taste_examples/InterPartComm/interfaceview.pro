isConnection('SUBPROGRAM ACCESS','interfaceview::IV','interfaceview','others','hello_PI_s_read_RI_s','hello.PI_s','->','read.RI_s','NIL','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','others','hello_PI_s_read_RI_s','Taste::coordinates','"154957 53700 142043 53700 142043 47716 129130 47716"','').
isComponentType('interfaceview::FV::hello','PUBLIC','PI_s','SUBPROGRAM','NIL','').
isComponentImplementation('interfaceview::FV::hello','PUBLIC','PI_s','others','SUBPROGRAM','NIL','others','').
isFeature('ACCESS','interfaceview::IV','hello','PI_s','PROVIDES','SUBPROGRAM','interfaceview::FV::hello::PI_s.others','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::hello','PI_s','NIL','NIL','Taste::Associated_Queue_Size','5','').
isProperty('NIL','=>','interfaceview::IV','hello','NIL','PI_s','Taste::coordinates','"129130 47716"','').
isProperty('NIL','=>','interfaceview::IV','hello','NIL','PI_s','Taste::RCMoperationKind','sporadic','').
isProperty('NIL','=>','interfaceview::IV','hello','NIL','PI_s','Taste::RCMperiod','10 ms','').
isProperty('NIL','=>','interfaceview::IV','hello','NIL','PI_s','Taste::Deadline','5 ms','').
isProperty('NIL','=>','interfaceview::IV','hello','NIL','PI_s','Taste::InterfaceName','"s"','').
isFeature('PARAMETER','interfaceview::FV::hello','PI_s','t','IN','NIL','DataView::TASTE_Boolean','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::hello','PI_s','NIL','t','Taste::encoding','NATIVE','').
isProperty('NIL','=>','interfaceview::FV::hello','PI_s','others','NIL','Compute_Execution_Time','0 ms .. 2 ms','').
isSubcomponent('interfaceview::IV','hello','others','s_impl','SUBPROGRAM','interfaceview::FV::hello::PI_s.others','NIL','NIL','').
isConnection('SUBPROGRAM ACCESS','interfaceview::IV','hello','others','OpToPICnx_s_impl','s_impl','->','PI_s','NIL','').
isPackage('interfaceview::FV::hello','PUBLIC','').
isComponentType('interfaceview::IV','PUBLIC','hello','SYSTEM','NIL','').
isComponentImplementation('interfaceview::IV','PUBLIC','hello','others','SYSTEM','NIL','others','').
isProperty('NIL','=>','interfaceview::IV','hello','NIL','NIL','Source_Language','(C)','').
isProperty('NIL','=>','interfaceview::IV','hello','NIL','NIL','Taste::Active_Interfaces','any','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','others','hello','Taste::coordinates','"94486 44566 129130 75746"','').
isSubcomponent('interfaceview::IV','interfaceview','others','hello','SYSTEM','interfaceview::IV::hello.others','NIL','NIL','').
isImportDeclaration('interfaceview::IV','PUBLIC','interfaceview::FV::hello','').
isImportDeclaration('interfaceview::IV','PUBLIC','Taste','').
isImportDeclaration('interfaceview::FV::hello','PUBLIC','Taste','').
isImportDeclaration('interfaceview::IV','PUBLIC','DataView','').
isImportDeclaration('interfaceview::FV::hello','PUBLIC','DataView','').
isImportDeclaration('interfaceview::FV::hello','PUBLIC','TASTE_IV_Properties','').
isImportDeclaration('interfaceview::IV','PUBLIC','TASTE_IV_Properties','').
isComponentType('interfaceview::FV::read','PUBLIC','PI_h','SUBPROGRAM','NIL','').
isComponentImplementation('interfaceview::FV::read','PUBLIC','PI_h','others','SUBPROGRAM','NIL','others','').
isFeature('ACCESS','interfaceview::IV','read','PI_h','PROVIDES','SUBPROGRAM','interfaceview::FV::read::PI_h.others','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::read','PI_h','NIL','NIL','Taste::Associated_Queue_Size','1','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','PI_h','Taste::coordinates','"189601 53700"','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','PI_h','Taste::RCMoperationKind','cyclic','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','PI_h','Taste::RCMperiod','100 ms','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','PI_h','Taste::Deadline','10 ms','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','PI_h','Taste::InterfaceName','"h"','').
isProperty('NIL','=>','interfaceview::FV::read','PI_h','others','NIL','Compute_Execution_Time','0 ms .. 3 ms','').
isSubcomponent('interfaceview::IV','read','others','h_impl','SUBPROGRAM','interfaceview::FV::read::PI_h.others','NIL','NIL','').
isConnection('SUBPROGRAM ACCESS','interfaceview::IV','read','others','OpToPICnx_h_impl','h_impl','->','PI_h','NIL','').
isComponentType('interfaceview::FV::read','PUBLIC','RI_s','SUBPROGRAM','NIL','').
isComponentImplementation('interfaceview::FV::read','PUBLIC','RI_s','others','SUBPROGRAM','NIL','others','').
isFeature('ACCESS','interfaceview::IV','read','RI_s','REQUIRES','SUBPROGRAM','interfaceview::FV::hello::PI_s.others','NIL','NIL','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','RI_s','Taste::coordinates','"154957 53700"','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','RI_s','Taste::RCMoperationKind','any','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','RI_s','Taste::InterfaceName','"s"','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','RI_s','Taste::labelInheritance','"true"','').
isFeature('PARAMETER','interfaceview::FV::read','RI_s','t','IN','NIL','DataView::TASTE_Boolean','NIL','NIL','').
isProperty('NIL','=>','interfaceview::FV::read','RI_s','NIL','t','Taste::encoding','NATIVE','').
isPackage('interfaceview::FV::read','PUBLIC','').
isComponentType('interfaceview::IV','PUBLIC','read','SYSTEM','NIL','').
isComponentImplementation('interfaceview::IV','PUBLIC','read','others','SYSTEM','NIL','others','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','NIL','Source_Language','(C)','').
isProperty('NIL','=>','interfaceview::IV','read','NIL','NIL','Taste::Active_Interfaces','any','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','others','read','Taste::coordinates','"154957 48975 189601 77006"','').
isSubcomponent('interfaceview::IV','interfaceview','others','read','SYSTEM','interfaceview::IV::read.others','NIL','NIL','').
isImportDeclaration('interfaceview::IV','PUBLIC','interfaceview::FV::read','').
isImportDeclaration('interfaceview::FV::read','PUBLIC','Taste','').
isImportDeclaration('interfaceview::FV::read','PUBLIC','DataView','').
isImportDeclaration('interfaceview::FV::read','PUBLIC','TASTE_IV_Properties','').
isProperty('_','_','_','_','_','_','LMP::Unparser_ID_Case','AsIs','').
isProperty('_','_','_','_','_','_','LMP::Unparser_Insert_Header','Yes','').
isPackage('interfaceview::IV','PUBLIC','').
isProperty('NIL','=>','interfaceview::IV','NIL','NIL','NIL','Taste::dataView','("DataView")','').
isProperty('NIL','=>','interfaceview::IV','NIL','NIL','NIL','Taste::dataViewPath','("DataView.aadl")','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','NIL','NIL','Taste::dataView','("DataView")','').
isProperty('NIL','=>','interfaceview::IV','interfaceview','NIL','NIL','Taste::dataViewPath','("DataView.aadl")','').
isVersion('AADL2.1','TASTE type interfaceview','','generated code: do not edit').
isProperty('NIL','=>','interfaceview::IV','NIL','NIL','NIL','Taste::coordinates','"0 0 297000 210000"','').
isProperty('NIL','=>','interfaceview::IV','NIL','NIL','NIL','Taste::version','"1.3"','').
isComponentType('interfaceview::IV','PUBLIC','interfaceview','SYSTEM','NIL','').
isComponentImplementation('interfaceview::IV','PUBLIC','interfaceview','others','SYSTEM','NIL','others','').

