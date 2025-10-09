#include <gtest/gtest.h>
#include <memory>
#include "../../data/DatabaseConnection.hpp"
#include "../../repositories/impl/PostgreSQLClientRepository.hpp"
#include "../../repositories/impl/PostgreSQLHallRepository.hpp"
#include "../../repositories/impl/PostgreSQLBookingRepository.hpp"

class RepositoryIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::string connectionString = "postgresql://dance_user:dance_password@localhost/test_dance_studio";
        dbConnection = std::make_shared<DatabaseConnection>(connectionString);
        
        clientRepository = std::make_unique<PostgreSQLClientRepository>(dbConnection);
        hallRepository = std::make_unique<PostgreSQLHallRepository>(dbConnection);
        bookingRepository = std::make_unique<PostgreSQLBookingRepository>(dbConnection);
        
        // Очищаем тестовые данные перед каждым тестом
        clearTestData();
        
        // Создаем тестовые данные, которые будут использоваться в нескольких тестах
        setupTestData();
    }

    void TearDown() override {
        clearTestData();
    }

    void clearTestData() {
        try {
            auto work = dbConnection->beginTransaction();
            work.exec("DELETE FROM bookings");
            work.exec("DELETE FROM clients");
            work.exec("DELETE FROM halls");
            dbConnection->commitTransaction(work);
        } catch (...) {
            // Игнорируем ошибки очистки
        }
    }

    void setupTestData() {
        // Создаем тестовый зал, который будет использоваться в тестах
        testHallId = UUID::generate();
        Hall hall(testHallId, "Test Hall", 50, UUID::generate()); // branchId тоже генерируем
        hall.setDescription("Test hall for integration tests");
        hall.setFloorType("wooden");
        hall.setEquipment("mirrors, sound system");
        
        hallRepository->save(hall);
    }

    std::shared_ptr<DatabaseConnection> dbConnection;
    std::unique_ptr<IClientRepository> clientRepository;
    std::unique_ptr<IHallRepository> hallRepository;
    std::unique_ptr<IBookingRepository> bookingRepository;
    
    UUID testHallId; // Будет использоваться в нескольких тестах
};

// Тест 1: Сохранение и извлечение клиента
TEST_F(RepositoryIntegrationTest, ClientRepository_SaveAndFind) {
    // Arrange
    UUID clientId = UUID::generate();
    Client client(clientId, "Integration Test User", "integration@test.com", "+123456789");
    
    // Act
    bool saveResult = clientRepository->save(client);
    auto foundClient = clientRepository->findById(clientId);
    
    // Assert
    EXPECT_TRUE(saveResult);
    ASSERT_TRUE(foundClient.has_value());
    EXPECT_EQ(foundClient->getId(), clientId);
    EXPECT_EQ(foundClient->getName(), "Integration Test User");
    EXPECT_EQ(foundClient->getEmail(), "integration@test.com");
}

// Тест 2: Сохранение и обновление бронирования
TEST_F(RepositoryIntegrationTest, BookingRepository_SaveAndUpdate) {
    // Arrange
    UUID clientId = UUID::generate();
    UUID bookingId = UUID::generate();
    
    // Создаем и сохраняем клиента
    Client client(clientId, "Test Client", "client@test.com", "+123456789");
    clientRepository->save(client);
    
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(1);
    TimeSlot timeSlot(startTime, 120);
    Booking booking(bookingId, clientId, testHallId, timeSlot, "Integration Test");
    
    // Act - Сохраняем бронирование
    bool saveResult = bookingRepository->save(booking);
    auto foundBooking = bookingRepository->findById(bookingId);
    
    // Assert - Проверяем сохранение
    EXPECT_TRUE(saveResult);
    ASSERT_TRUE(foundBooking.has_value());
    EXPECT_EQ(foundBooking->getPurpose(), "Integration Test");
    EXPECT_EQ(foundBooking->getStatus(), BookingStatus::PENDING);
    
    // Act - Обновляем бронирование
    foundBooking->confirm();
    bool updateResult = bookingRepository->update(*foundBooking);
    auto updatedBooking = bookingRepository->findById(bookingId);
    
    // Assert - Проверяем обновление
    EXPECT_TRUE(updateResult);
    ASSERT_TRUE(updatedBooking.has_value());
    EXPECT_EQ(updatedBooking->getStatus(), BookingStatus::CONFIRMED);
}

// Тест 3: Поиск конфликтующих бронирований
TEST_F(RepositoryIntegrationTest, BookingRepository_FindConflictingBookings) {
    // Arrange
    UUID clientId = UUID::generate();
    
    // Создаем и сохраняем клиента
    Client client(clientId, "Conflict Test User", "conflict@test.com", "+123456789");
    clientRepository->save(client);
    
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(2);
    TimeSlot timeSlot1(startTime, 60); // 14:00 - 15:00
    TimeSlot timeSlot2(startTime + std::chrono::minutes(30), 60); // 14:30 - 15:30 (конфликт)
    TimeSlot timeSlot3(startTime + std::chrono::hours(2), 60); // 16:00 - 17:00 (нет конфликта)
    
    Booking booking1(UUID::generate(), clientId, testHallId, timeSlot1, "First Booking");
    booking1.confirm();
    bookingRepository->save(booking1);
    
    // Act - Ищем конфликтующие бронирования
    auto conflicts = bookingRepository->findConflictingBookings(testHallId, timeSlot2);
    auto noConflicts = bookingRepository->findConflictingBookings(testHallId, timeSlot3);
    
    // Assert
    EXPECT_EQ(conflicts.size(), 1);
    EXPECT_EQ(conflicts[0].getId(), booking1.getId());
    EXPECT_TRUE(noConflicts.empty());
}

// Тест 4: Проверка работы с залами
TEST_F(RepositoryIntegrationTest, HallRepository_BasicOperations) {
    // Arrange
    UUID hallId = UUID::generate();
    UUID branchId = UUID::generate();
    Hall hall(hallId, "New Test Hall", 30, branchId);
    hall.setDescription("Another test hall");
    hall.setFloorType("marley");
    hall.setEquipment("mirrors, barre");
    
    // Act
    bool saveResult = hallRepository->save(hall);
    auto foundHall = hallRepository->findById(hallId);
    bool existsResult = hallRepository->exists(hallId);
    
    // Assert
    EXPECT_TRUE(saveResult);
    EXPECT_TRUE(existsResult);
    ASSERT_TRUE(foundHall.has_value());
    EXPECT_EQ(foundHall->getName(), "New Test Hall");
    EXPECT_EQ(foundHall->getCapacity(), 30);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "Running integration tests. Ensure test database is running." << std::endl;
    std::cout << "Connection string: postgresql://dance_user:dance_password@localhost/test_dance_studio" << std::endl;
    
    return RUN_ALL_TESTS();
}