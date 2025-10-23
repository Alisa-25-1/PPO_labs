#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "BookingLib::BookingCore" for configuration ""
set_property(TARGET BookingLib::BookingCore APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(BookingLib::BookingCore PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libBookingCore.a"
  )

list(APPEND _cmake_import_check_targets BookingLib::BookingCore )
list(APPEND _cmake_import_check_files_for_BookingLib::BookingCore "${_IMPORT_PREFIX}/lib/libBookingCore.a" )

# Import target "BookingLib::DataAccess" for configuration ""
set_property(TARGET BookingLib::DataAccess APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(BookingLib::DataAccess PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libDataAccess.a"
  )

list(APPEND _cmake_import_check_targets BookingLib::DataAccess )
list(APPEND _cmake_import_check_files_for_BookingLib::DataAccess "${_IMPORT_PREFIX}/lib/libDataAccess.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
