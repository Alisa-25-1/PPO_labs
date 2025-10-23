# Файл конфигурации пакета для CMake
@PACKAGE_INIT@

# Проверка совместимости версий
set_and_check(BookingLib_INCLUDE_DIR "@PACKAGE_INCLUDE_INSTALL_DIR@")
set_and_check(BookingLib_LIB_DIR "@PACKAGE_LIB_INSTALL_DIR@")

# Проверка зависимостей
find_dependency(libpqxx REQUIRED)

# Включение файла с целями
include("${CMAKE_CURRENT_LIST_DIR}/BookingLibTargets.cmake")

# Проверка обязательных компонентов
set(BookingLib_FOUND TRUE)

# Предоставление переменных для обратной совместимости
set(BookingLib_INCLUDE_DIRS ${BookingLib_INCLUDE_DIR})
set(BookingLib_LIBRARIES BookingLib::BookingCore BookingLib::DataAccess)

# Сообщение об успешной загрузке (опционально)
message(STATUS "BookingLib ${BookingLib_VERSION} found")