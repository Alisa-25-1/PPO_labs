#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

class WebApplication;

class ClientDashboard : public Wt::WContainerWidget {
public:
    ClientDashboard(WebApplication* app);
    
private:
    WebApplication* app_;
    
    void setupUI();
};