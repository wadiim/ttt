if(UNIX)
	set(OS unix)
elseif(WIN32)
	set(OS windows)
endif()

set(SOURCE_FILES
	${OS}/term.c
	game.c
	main.c
	utils.c
	)

add_executable(ttt ${SOURCE_FILES})
