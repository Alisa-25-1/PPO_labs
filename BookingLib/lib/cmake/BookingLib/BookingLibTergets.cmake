# Файл с импортированными целями (генерируется автоматически CMake)
# Это упрощенная версия того, что генерирует CMake

# Цель BookingCore
add_library(BookingLib::BookingCore STATIC IMPORTED)
set_target_properties(BookingLib::BookingCore PROPERTIES
  IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../../libBookingCore.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/../../include"
  INTERFACE_COMPILE_FEATURES "cxx_std_17"
  INTERFACE_LINK_LIBRARIES "BookingLib::DataAccess;libpqxx::pqxx;pthread"
)

# Цель DataAccess
add_library(BookingLib::DataAccess STATIC IMPORTED)
set_target_properties(BookingLib::DataAccess PROPERTIES
  IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../../libDataAccess.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/../../include"
  INTERFACE_COMPILE_FEATURES "cxx_std_17"
  INTERFACE_LINK_LIBRARIES "libpqxx::pqxx;pthread"
)

# Проверка существования файлов библиотек
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../libBookingCore.a")
  message(FATAL_ERROR "Импортированная цель 'BookingLib::BookingCore' ссылается на несуществующий файл: ${CMAKE_CURRENT_LIST_DIR}/../../libBookingCore.a")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../libDataAccess.a")
  message(FATAL_ERROR "Импортированная цель 'BookingLib::DataAccess' ссылается на несуществующий файл: ${CMAKE_CURRENT_LIST_DIR}/../../libDataAccess.a")
endif()