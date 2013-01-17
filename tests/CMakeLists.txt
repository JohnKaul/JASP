configure_file( 
    ${CMAKE_CURRENT_SOURCE_DIR}/testReadME.txt 
    ${EXECUTABLE_OUTPUT_PATH}/testReadME.txt @ONLY 
    )

configure_file( 
    ${CMAKE_CURRENT_SOURCE_DIR}/MakeReadME.cmd 
    ${EXECUTABLE_OUTPUT_PATH}/MakeReadME.cmd @ONLY 
    )

install(FILES 
    ${EXECUTABLE_OUTPUT_PATH}/testReadME.txt 
    DESTINATION tests
    )

install(FILES 
    ${EXECUTABLE_OUTPUT_PATH}/MakeReadME.cmd 
    DESTINATION tests
    )

