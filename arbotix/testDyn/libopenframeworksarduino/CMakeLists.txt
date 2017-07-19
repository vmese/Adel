find_package(Boost COMPONENTS system filesystem signals REQUIRED)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} ) 

set(OPENFRAMEWORKSARDUINO_SRCS
  ofArduino.cpp
  #ofSerial.cpp
  ofSerialLinux.cpp
  #ofSerialWin.cpp	
  )

set(OPENFRAMEWORKSARDUINO_H
  ofArduino.h
  ofConstants.h
  #ofSerial.h
  ofSerialLinux.h
  #ofSerialWin.h
  ofTypes.h
  StdAfx.h
)

add_library(openframeworksarduino SHARED ${OPENFRAMEWORKSARDUINO_SRCS} ${OPENFRAMEWORKSARDUINO_H})

target_link_libraries(openframeworksarduino ${Boost_LIBRARIES})
SET_TARGET_PROPERTIES(openframeworksarduino PROPERTIES OUTPUT_NAME openframeworksarduino)



