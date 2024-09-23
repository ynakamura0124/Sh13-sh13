#ROOT_GENERATE_DICTIONARY(BRIKENDict
#	${BRIKEN_TOOLS_MERGER_DIR}/data2Tree.hpp
#	LINKDEF ../include/BRIKENDataLinkDef.h
#)

add_library(BRIKENLib SHARED
	#${BRIKEN_TOOLS_MERGER_DIR}/data2Tree.cxx
	BRIKENGammaSourceGeneratorAction.cpp	
	#BRIKENDict.cxx
)

#target_link_libraries(BRIKENLib ${ROOT_LIBRARIES} ${Gean4_LIBRARIES})
target_link_libraries(BRIKENLib ${ROOT_LIBRARIES} ${Gean4_LIBRARIES} data2TreeLib)

install(TARGETS BRIKENLib
	LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include
)

#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/BRIKENDict_rdict.pcm DESTINATION lib)