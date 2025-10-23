#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WPasswordEdit.h>

class WebApplication;

class LoginWidget : public Wt::WContainerWidget {
public:
    LoginWidget(WebApplication* app);
    
private:
    WebApplication* app_;
    Wt::WLineEdit* emailEdit_;
    Wt::WPasswordEdit* passwordEdit_; 
    Wt::WPushButton* loginButton_;
    Wt::WText* statusText_;
    
    void handleLogin();
};