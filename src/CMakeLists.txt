find_package(Threads)
find_package(Boost REQUIRED)

include(FetchContent)
FetchContent_Declare(sqlite3 URL "https://www.sqlite.org/snapshot/sqlite-snapshot-202205041843.tar.gz")
FetchContent_MakeAvailable(sqlite3)
if (NOT sqlite3_POPULATED)
    FetchContent_Populate(sqlite3)
endif()


add_executable(${PROJECT_NAME}  "${sqlite3_SOURCE_DIR}/sqlite3.c"  main.cpp)

target_include_directories( ${PROJECT_NAME} PUBLIC
                            ${Boost_INCLUDE_DIR}
                            ${sqlite3_SOURCE_DIR}
                           )

if(MINGW)
target_link_libraries(${PROJECT_NAME}
                        ws2_32
                        wsock32
                      )
endif()

#BEGIN release
install(TARGETS ${PROJECT_NAME})
#install(FILES ${DB_FILE} DESTINATION var/lib)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
#END



