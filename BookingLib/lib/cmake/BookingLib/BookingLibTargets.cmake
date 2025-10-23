# Автоматически генерируется CMake при установке
include(CMakeFindDependencyMacro)

# Объявляем импортированные цели
add_library(BookingLib::BookingCore STATIC IMPORTED)
add_library(BookingLib::DataAccess STATIC IMPORTED)

# Указываем расположение библиотек
set_target_properties(BookingLib::BookingCore PROPERTIES
    IMPORTED_LOCATION "/usr/local/BookingLib/lib/libBookingCore.a"
    INTERFACE_INCLUDE_DIRECTORIES "/usr/local/BookingLib/include"
)

set_target_properties(BookingLib::DataAccess PROPERTIES
    IMPORTED_LOCATION "/usr/local/BookingLib/lib/libDataAccess.a"
    INTERFACE_INCLUDE_DIRECTORIES "/usr/local/BookingLib/include"
    INTERFACE_LINK_LIBRARIES "BookingLib::BookingCore;libpqxx::libpqxx"
)