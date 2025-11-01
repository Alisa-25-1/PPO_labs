include(CMakeFindDependencyMacro)

find_dependency(libpqxx REQUIRED)

if(NOT TARGET BookingLib::BookingCore)
    include("${CMAKE_CURRENT_LIST_DIR}/BookingLibTargets.cmake")
endif()

set(BookingLib_FOUND TRUE)
set(BOOKINGLIB_INCLUDE_DIRS "/usr/local/BookingLib/include")
set(BOOKINGLIB_LIBRARIES BookingLib::BookingCore BookingLib::DataAccess)