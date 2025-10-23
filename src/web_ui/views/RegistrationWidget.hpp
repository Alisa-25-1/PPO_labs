#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPasswordEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WComboBox.h>

class WebApplication;

class RegistrationWidget : public Wt::WContainerWidget {
public:
    RegistrationWidget(WebApplication* app);
    
private:
    WebApplication* app_;
    
    Wt::WLineEdit* nameEdit_;
    Wt::WLineEdit* emailEdit_;
    Wt::WLineEdit* phoneEdit_;
    Wt::WPasswordEdit* passwordEdit_;
    Wt::WPasswordEdit* confirmPasswordEdit_;
    Wt::WPushButton* registerButton_;
    Wt::WPushButton* backButton_;
    Wt::WText* statusText_;

    void setupUI();
    void handleRegister();
    void handleBack();
    void updateStatus(const std::string& message, bool isError = false);
    bool validateForm();
};