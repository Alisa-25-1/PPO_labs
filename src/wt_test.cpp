#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WBreak.h>

class HelloApplication : public Wt::WApplication {
public:
    HelloApplication(const Wt::WEnvironment& env) : Wt::WApplication(env) {
        setTitle("Wt Test Application");
        
        auto container = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
        
        // Заголовок
        container->addWidget(std::make_unique<Wt::WText>("<h1>Wt Test - Работает!</h1>"));
        container->addWidget(std::make_unique<Wt::WBreak>());
        
        // Поле ввода
        auto nameEdit = container->addWidget(std::make_unique<Wt::WLineEdit>());
        nameEdit->setPlaceholderText("Введите ваше имя");
        container->addWidget(std::make_unique<Wt::WBreak>());
        
        // Кнопка
        auto button = container->addWidget(std::make_unique<Wt::WPushButton>("Приветствовать"));
        container->addWidget(std::make_unique<Wt::WBreak>());
        
        // Текст для вывода
        auto greeting = container->addWidget(std::make_unique<Wt::WText>());
        
        // Обработчик кнопки
        button->clicked().connect([=] {
            if (!nameEdit->text().empty()) {
                greeting->setText("Привет, " + nameEdit->text() + "! Wt работает корректно.");
            } else {
                greeting->setText("Пожалуйста, введите имя!");
            }
        });
    }
};

int main(int argc, char** argv) {
    return Wt::WRun(argc, argv, [](const Wt::WEnvironment& env) {
        return std::make_unique<HelloApplication>(env);
    });
}