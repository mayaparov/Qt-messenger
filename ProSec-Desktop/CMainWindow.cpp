#include "CMainWindow.h"

CColorSet g_CColorSet;
CNetworkWrapper g_CNetworkWrapper;

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent)
{
    SetupVars();

    connect(m_SubmitLoginButton, SIGNAL(clicked()), this, SLOT(SubmitButtonEventPressed()));
    connect(m_MessengerButton, SIGNAL(clicked()), this, SLOT(ChatsButtonEventPressed()) );
    connect(m_ProfileButton, SIGNAL(clicked()), this, SLOT(ProfileButtonEventPressed())  );
    connect(m_SettingsButton, SIGNAL(clicked()), this, SLOT(SettingsButtonEventPressed())  );

    connect(m_InputBoxUserName, SIGNAL(textChanged(const QString &)), this, SLOT(GetLoginText(const QString&)));
    connect(m_InputBoxPassword, SIGNAL(textChanged(const QString &)), this, SLOT(GetPasswordText(const QString &)));

    connect(m_MessegesRectAnimation, &QPropertyAnimation::valueChanged, [=] () { repaint(); } );

    connect(m_Dialogs, &QListWidget::itemClicked, this, &CMainWindow::m_DialogsItemPressed);

    connect(m_TcpSocket, &QTcpSocket::readyRead, this, &CMainWindow::OnReadyRead);
    connect(m_TcpSocket, &QTcpSocket::disconnected, m_TcpSocket, &QTcpSocket::deleteLater);
    connect(m_MessageLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(GetMessage(const QString &)));
    connect(m_SendButton, &QPushButton::clicked,  this,  &CMainWindow::SendToServer);

    connect(m_LightModeSettingsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(LightModeState(int)));

    //qDebug() << QCoreApplication::applicationDirPath() << "\n";
}

void CMainWindow::SetupVars()
{
    // ~ Nums & Strings [Data] ~
    {
        m_PageType              = 0;
        m_MessageBlockSize      = 0;
        m_MessengerWindowRect   = { -10, -10};

        m_LoginText             = "???";
        m_PasswordText          = "???";
        m_CurrentUserName       = "Beta User";
        m_BackgroundMainRect    = { 0, m_MessengerWindowRect.y(), m_MessengerWindowRect.x(), m_MessengerWindowRect.y() };
    }

    GetGlobalScreenSize();

    m_MessengerWindowRect = { m_GlobalScreenRect.width() , m_GlobalScreenRect.height() };

    // ~ Allocations ~
    {

        m_RoundedAvatarPath             = new QPainterPath();

        #if defined(_WIN32)
        m_RoundedWindowPath             = new QPainterPath();
        #endif

        m_TcpSocket                     = new QTcpSocket(this);

        m_InputBoxUserName              = new QLineEdit(this);
        m_InputBoxPassword              = new QLineEdit(this);

        m_HeaderIntoLabel               = new QLabel(this);
        m_UsernameIntoLabel             = new QLabel(this);
        m_UserPasswordIntoLabel         = new QLabel(this);
        m_CopyrightLabel                = new QLabel(this);
        m_ErrorLabel                    = new QLabel(this);

        m_CurrentPage                   = new QLabel(this);

        m_ChatName                      = new QLabel(this);

        m_UsernameLabelSettings         = new QLabel(this);
        m_UserStatusLabelSettings       = new QLabel(this);
        m_LightModeLabelSettings        = new QLabel(this);

        m_LightModeSettingsCheckBox     = new QCheckBox(this);

        m_SubmitLoginButton             = new QPushButton("S", this);
        m_RegisterLoginButton           = new QPushButton("R", this);
        m_MessengerButton               = new QToolButton(this);
        m_ProfileButton                 = new QToolButton(this);
        m_SettingsButton                  = new QToolButton(this);

        m_Dialogs                       = new QListWidget(this);
        m_SendButton                    = new QPushButton(this);
        m_MessageLineEdit               = new QLineEdit(this);
        m_MessagesTextBrowser           = new QTextBrowser(this);

        m_SecuritySettingsButton        = new QPushButton(this);
        m_NotificationSettingsButton    = new QPushButton(this);
        m_StylesSettingsButton          = new QPushButton(this);
        //m_HelpSettingsButton            = new QPushButton(this);

        m_SearchButtonUpside            = new QPushButton("", this);
        m_SwitchLightmodeButtonUpside   = new QPushButton("", this);
        m_EtcButtonUpside               = new QPushButton("", this);

        m_LoginInputAnimation           = new QPropertyAnimation(m_InputBoxUserName, "geometry");
        m_PasswordInputAnimation        = new QPropertyAnimation(m_InputBoxPassword, "geometry");
        m_SubmitButtonAnimation         = new QPropertyAnimation(m_SubmitLoginButton, "geometry");
        m_RegistrationAnimation         = new QPropertyAnimation(m_RegisterLoginButton, "geometry");
        m_MessegesRectAnimation         = new QPropertyAnimation(this, "nrect");

        m_UserAvatar                    = new QImage(":/Tests/Tests/AvatarTest.jpeg");
    }

    //~ WIN 32 or OSX ~
    #if (defined(_WIN32) || defined(__MACH__)) && OSX_TARGET
        m_MessengerWindowRect = { (int)(m_GlobalScreenRect.width() * 0.7) , (int)(m_GlobalScreenRect.height() * 0.8) };

    // ~ WINDOWS AREA ~
    #if defined(_WIN32)
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        m_RoundedWindowPath->addRoundedRect(this->rect().x(), this->rect().y(), m_MessengerWindowRect.x(), m_MessengerWindowRect.y(), 20, 20);
    #endif

    // ~ OSX AREA ~
    #if defined(__MACH__)
        setWindowFlags(Qt::Window);
    #endif

    // ~ DESKTOP AREA ~
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setFixedSize(m_MessengerWindowRect.x(), m_MessengerWindowRect.y());

    #endif
    /*
    // ~ Resize Helpers~
    if (m_MessengerWindowRect.x() / 4 > 280)
    { //check for minimum dialog window size
        crutch.setX(m_MessengerWindowRect.x() / 4);
    }
    else
    {
        crutch.setX(280);
    }
    crutch.setY(60);
*/
    m_MainLayout.setGeometry(QRect(0, 0, m_MessengerWindowRect.x(), m_MessengerWindowRect.y()));
    m_MessagerLayout.setGeometry(QRect(0, m_MessengerWindowRect.y(), m_MessengerWindowRect.x(), m_MessengerWindowRect.y() * 0.3));

    // ~ Fonts ~
    {
        int id = QFontDatabase::addApplicationFont(":/Fonts/Fonts/AzonixRegular/AzonixRegular.ttf");
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        m_AzonixFont.setFamily(family);
        m_AzonixFont.setPointSize(30);

        id = QFontDatabase::addApplicationFont(":/Fonts/Fonts/DIN/DIN.ttf");
        //family = QFontDatabase::applicationFontFamilies(id).at(0);
        m_DinFont.setFamily(family);
        m_DinFont.setPointSize(15);

        id = QFontDatabase::addApplicationFont(":/Fonts/Fonts/Noir_medium.otf");
       // family = QFontDatabase::applicationFontFamilies(id).at(0);
        m_NoirFont.setFamily(family);
        m_NoirFont.setPointSize(21);

        id = QFontDatabase::addApplicationFont(":/Fonts/Fonts/Sf-ProDisplayFont-BOLD.OTF");
        //family = QFontDatabase::applicationFontFamilies(id).at(0);
        m_SfProDisplayBoldFont.setFamily(family);
        m_SfProDisplayBoldFont.setPointSize(30);

        id = QFontDatabase::addApplicationFont(":/Fonts/Fonts/Sf-ProDisplayFont-REGULAR.OTF");
        //family = QFontDatabase::applicationFontFamilies(id).at(0);
        m_SfProDisplayRegularFont.setFamily(family);
        m_SfProDisplayRegularFont.setPointSize(15);

        id = QFontDatabase::addApplicationFont(":/Fonts/Fonts/Sf-ProDisplayFont-REGULAR.OTF");
        //family = QFontDatabase::applicationFontFamilies(id).at(0);
        m_fProDisplaySettingsFont.setFamily(family);
        m_fProDisplaySettingsFont.setPointSize(14);

        id = QFontDatabase::addApplicationFont(":/Fonts/Fonts/Sf-ProDisplayFont-REGULAR.OTF");
        //family = QFontDatabase::applicationFontFamilies(id).at(0);
        m_UsernameSettingFont.setFamily(family);
        m_UsernameSettingFont.setPointSize(20);

        id = QFontDatabase::addApplicationFont(":/Fonts/Fonts/Sf-ProDisplayFont-REGULAR.OTF");
        //family = QFontDatabase::applicationFontFamilies(id).at(0);
        m_UserStatusSettingFont.setFamily(family);
        m_UserStatusSettingFont.setPointSize(16);

        id = QFontDatabase::addApplicationFont(":/Fonts/Fonts/Sf-ProDisplayFont-REGULAR.OTF");
        //family = QFontDatabase::applicationFontFamilies(id).at(0);
        m_OptionSettingsFont.setFamily(family);
        m_OptionSettingsFont.setPointSize(17);
    }

    // ~ Widgets ~
    {
        // ~ ~
        // m_CurrentPage
        m_MainLayout.addWidget(m_CurrentPage);
        m_CurrentPage->move(QPoint(m_MessengerWindowRect.x() * 0.05, m_MessengerWindowRect.y() * 0.09));
        m_CurrentPage->resize(m_MessengerWindowRect.x() * 0.3, m_MessengerWindowRect.y() * 0.04);
        m_CurrentPage->setFont(m_SfProDisplayBoldFont);
        m_CurrentPage->setText("???");
        m_CurrentPage->setStyleSheet("QLabel { color: rgba(255, 255, 255, 255); opacity: 255; }");
        m_CurrentPage->hide();

        // ~ Login Page ~
        {
            m_MainLayout.addWidget(m_HeaderIntoLabel);
            m_HeaderIntoLabel->move(QPoint((m_MessengerWindowRect.x() / 2) - 105, m_MessengerWindowRect.y() * 0.28));
            m_HeaderIntoLabel->resize(m_MessengerWindowRect.x() * 0.5, m_MessengerWindowRect.y() * 0.04);
            m_HeaderIntoLabel->setFont(m_AzonixFont);
            m_HeaderIntoLabel->setText("PROSEC APP");
            m_HeaderIntoLabel->setStyleSheet("QLabel { color: rgba(255, 255, 255, 200); opacity: 255; }");
            m_HeaderIntoLabel->show();

            m_MainLayout.addWidget(m_UsernameIntoLabel);
            m_UsernameIntoLabel->move(QPoint((m_MessengerWindowRect.x() / 2) - 50, m_MessengerWindowRect.y() * 0.35));
            m_UsernameIntoLabel->resize(m_MessengerWindowRect.x() * 0.3, m_MessengerWindowRect.y() * 0.06);
            m_UsernameIntoLabel->setFont(m_NoirFont);
            m_UsernameIntoLabel->setText("Username");
            m_UsernameIntoLabel->setStyleSheet("QLabel { color: rgba(255, 255, 255, 200); opacity: 255; }");
            m_UsernameIntoLabel->show();

            m_MainLayout.addWidget(m_UserPasswordIntoLabel);
            m_UserPasswordIntoLabel->move(QPoint((m_MessengerWindowRect.x() / 2) - 50, m_MessengerWindowRect.y() * 0.45));
            m_UserPasswordIntoLabel->resize(m_MessengerWindowRect.x() * 0.25, m_MessengerWindowRect.y() * 0.06);
            m_UserPasswordIntoLabel->setFont(m_NoirFont);
            m_UserPasswordIntoLabel->setText("Password");
            m_UserPasswordIntoLabel->setStyleSheet("QLabel { color: rgba(255, 255, 255, 200); opacity: 255;}");
            m_UserPasswordIntoLabel->show();

            m_MainLayout.addWidget(m_CopyrightLabel);
            m_CopyrightLabel->move(QPoint((m_MessengerWindowRect.x() / 2) - 80, m_MessengerWindowRect.y() * 0.9));
            m_CopyrightLabel->resize(m_MessengerWindowRect.x() * 0.4, m_MessengerWindowRect.y() * 0.03);
            m_CopyrightLabel->setFont(m_DinFont);
            m_CopyrightLabel->setText("© 2023 RedLine Software");
            m_CopyrightLabel->setStyleSheet("QLabel { color: rgba(232, 7, 89, 200); opacity: 255;}");
            m_CopyrightLabel->show();

            m_MainLayout.addWidget(m_ErrorLabel);
            m_ErrorLabel->move(QPoint((m_MessengerWindowRect.x() / 2) - 80, m_MessengerWindowRect.y() * 0.8));
            m_ErrorLabel->resize(m_MessengerWindowRect.x() * 0.4, m_MessengerWindowRect.y() * 0.03);
            m_ErrorLabel->setFont(m_DinFont);
            m_ErrorLabel->setStyleSheet("QLabel { color: rgba(232, 7, 89, 200); opacity: 255;}");
            m_ErrorLabel->hide();

            m_MainLayout.addWidget(m_InputBoxUserName);
            m_InputBoxUserName->move(QPoint(m_MessengerWindowRect.x() * 0.2, m_MessengerWindowRect.y() * 0.4));
            m_InputBoxUserName->resize(QSize(m_MessengerWindowRect.x() * 0.6, 30));
            m_InputBoxUserName->setStyleSheet("QLineEdit {  color: white; opacity: 255; border-radius: 8px; background-color: rgba(46, 44, 70, 200); } QLineEdit:hover:focus { background-color: rgba(73, 69, 110, 200); }");
            m_InputBoxUserName->show();

            m_MainLayout.addWidget(m_InputBoxPassword);
            m_InputBoxPassword->move(QPoint(m_MessengerWindowRect.x() * 0.2, m_MessengerWindowRect.y() * 0.5));
            m_InputBoxPassword->resize(QSize(m_MessengerWindowRect.x() * 0.6, 30));
            m_InputBoxPassword->setStyleSheet("QLineEdit {  color: white; opacity: 255; border-radius: 8px; background-color: rgba(46, 44, 70, 200); } QLineEdit:hover:focus { background-color: rgba(73, 69, 110, 200); } ");
            m_InputBoxPassword->show();

            m_MainLayout.addWidget(m_SubmitLoginButton);
            m_SubmitLoginButton->move(QPoint(m_MessengerWindowRect.x() * 0.2, m_MessengerWindowRect.y() * 0.55));
            m_SubmitLoginButton->setStyleSheet("QPushButton { color: white; opacity: 255; border-radius: 8px; background-color: rgba(46, 44, 70, 200);}  QPushButton:hover:pressed { background-color: rgba(73, 69, 110, 200); }");
            m_SubmitLoginButton->resize(QSize(m_MessengerWindowRect.x() * 0.29, 30));
            m_SubmitLoginButton->setText("Submit");
            m_SubmitLoginButton->setFont(QFont(":/Fonts/Fonts/DIN/DIN.ttf", 15));
            m_SubmitLoginButton->show();

            m_MainLayout.addWidget(m_RegisterLoginButton);
            m_RegisterLoginButton->move(QPoint(m_MessengerWindowRect.x() * 0.51, m_MessengerWindowRect.y() * 0.55));
            m_RegisterLoginButton->setStyleSheet("QPushButton { color: white; opacity: 255; border-radius: 8px; background-color: rgba(46, 44, 70, 200);}  QPushButton:hover:pressed { background-color: rgba(73, 69, 110, 200); }");
            m_RegisterLoginButton->resize(QSize(m_MessengerWindowRect.x() * 0.29, 30));
            m_RegisterLoginButton->setText("Register");
            m_RegisterLoginButton->setFont(QFont(":/Fonts/Fonts/DIN/DIN.ttf", 15));
            m_RegisterLoginButton->show();
        }

        // ~ Chats Page ~
        {
            m_MainLayout.addWidget(m_Dialogs);
            m_Dialogs->setGeometry(0, m_MessengerWindowRect.y() * 0.25, 0, 0);
            m_Dialogs->resize(QSize(m_MessengerWindowRect.x() / 3, m_MessengerWindowRect.y()));
            m_Dialogs->setStyleSheet("QListWidget { color: rgba(255, 255, 255, 200); opacity: 255; background-color: rgba(46, 44, 70, 200); border-top-left-radius: 10px; border-top-right-radius: 10px; }");
            QListWidgetItem* item = new QListWidgetItem("Saved messages");
            item->setFont(m_DinFont);
            m_Dialogs->addItem(item);
            m_Dialogs->hide();

            m_MainLayout.addWidget(m_ChatName);
            m_ChatName->move(QPoint((m_MessengerWindowRect.x() * 0.05), m_MessengerWindowRect.y() * 0.17));
            m_ChatName->resize(m_MessengerWindowRect.x() * 0.4, m_MessengerWindowRect.y() * 0.08);
            m_ChatName->setFont(m_DinFont);
            m_ChatName->setText("test root");
            m_ChatName->setStyleSheet("QLabel { color: rgba(255, 255, 255, 200); font: 20pt; opacity: 255;}");
            m_ChatName->hide();

            m_MainLayout.addWidget(m_MessagesTextBrowser);
            m_MessagesTextBrowser->move(QPoint(0, m_MessengerWindowRect.y() * 0.25));
            m_MessagesTextBrowser->resize(m_MessengerWindowRect.x() * 0.9, m_MessengerWindowRect.y() * 0.65);
            m_MessagesTextBrowser->setFont(m_fProDisplaySettingsFont);
            //m_MessagesTextBrowser->setStyleSheet("QTextBrowser { color: rgba(255, 255, 255, 200); opacity: 255; background-color: rgba(13, 11, 33, 200); border-top-left-radius: 10px; border-top-right-radius: 10px; }");
            m_MessagesTextBrowser->setStyleSheet("QTextBrowser { color: rgba(255, 255, 255, 200); opacity: 255; background-color: rgba(46, 44, 70, 200); border-top-left-radius: 10px; border-top-right-radius: 10px; }");
            m_MessagesTextBrowser->hide();

            m_MainLayout.addWidget(m_MessageLineEdit);
            m_MessageLineEdit->move(QPoint(0, m_MessengerWindowRect.y() * 0.9));
            m_MessageLineEdit->resize(m_MessengerWindowRect.x() * 0.9, m_MessengerWindowRect.y() * 0.05);
            m_MessageLineEdit->setStyleSheet("QLineEdit { color: rgba(255, 255, 255, 200); opacity: 255; background-color: rgba(46, 44, 70, 200); }");
            m_MessageLineEdit->hide();

            m_MainLayout.addWidget(m_SendButton);
            m_SendButton->move(QPoint(m_MessengerWindowRect.x() * 0.9, m_MessengerWindowRect.y() * 0.90));
            m_SendButton->setStyleSheet("QPushButton { color: white; opacity: 255; background-color: rgba(46, 44, 70, 200);}");
            m_SendButton->resize(QSize(m_MessengerWindowRect.x() * 0.1, m_MessengerWindowRect.y() * 0.05));
            m_SendButton->setFont(m_UserStatusSettingFont);
            m_SendButton->setText("✉");
            m_SendButton->hide();
        }

        // ~ Profile Page ~

        // ~ Settings Page ~
        {
            m_MainLayout.addWidget(m_UsernameLabelSettings);
            m_UsernameLabelSettings->move(QPoint(m_MessengerWindowRect.x() * 0.3, m_MessengerWindowRect.y() * 0.20));
            m_UsernameLabelSettings->resize(m_MessengerWindowRect.x() * 0.5, m_MessengerWindowRect.y() * 0.04);
            m_UsernameLabelSettings->setFont(m_UsernameSettingFont);
            m_UsernameLabelSettings->setText("???");
            m_UsernameLabelSettings->setStyleSheet("QLabel { color: rgba(255, 255, 255, 255); opacity: 255; }");
            m_UsernameLabelSettings->hide();

            m_MainLayout.addWidget(m_UserStatusLabelSettings);
            m_UserStatusLabelSettings->move(QPoint(m_MessengerWindowRect.x() * 0.3, m_MessengerWindowRect.y() * 0.23));
            m_UserStatusLabelSettings->resize(m_MessengerWindowRect.x() * 0.6, m_MessengerWindowRect.y() * 0.04);
            m_UserStatusLabelSettings->setFont(m_UserStatusSettingFont);
            m_UserStatusLabelSettings->setText("CEO RedLine Software");
            m_UserStatusLabelSettings->setStyleSheet("QLabel { color: rgba(160, 160, 160, 255); opacity: 255;}");
            m_UserStatusLabelSettings->hide();

            // Buttons
            {
                m_MainLayout.addWidget(m_LightModeLabelSettings);
                m_LightModeLabelSettings->move(QPoint(m_MessengerWindowRect.x() * 0.2, m_MessengerWindowRect.y() * 0.33));
                m_LightModeLabelSettings->setStyleSheet("QLabel { color: white; opacity: 255; text-align: left; }");
                m_LightModeLabelSettings->resize(QSize(m_MessengerWindowRect.x() * 0.3, m_MessengerWindowRect.y() * 0.05));
                m_LightModeLabelSettings->setText("Light Mode");
                m_LightModeLabelSettings->setFont(m_OptionSettingsFont);
                m_LightModeLabelSettings->hide();

                m_MainLayout.addWidget(m_LightModeSettingsCheckBox);
                m_LightModeSettingsCheckBox->move(QPoint(m_MessengerWindowRect.x() * 0.85, m_MessengerWindowRect.y() * 0.33));
                m_LightModeSettingsCheckBox->setStyleSheet("QCheckBox { color: white; opacity: 255; text-align: left; }");
                m_LightModeSettingsCheckBox->resize(QSize(m_MessengerWindowRect.x() * 0.1, m_MessengerWindowRect.y() * 0.05));
                m_LightModeSettingsCheckBox->hide();

                m_MainLayout.addWidget(m_SecuritySettingsButton);
                m_SecuritySettingsButton->move(QPoint(m_MessengerWindowRect.x() * 0.2, m_MessengerWindowRect.y() * 0.42));
                m_SecuritySettingsButton->setStyleSheet("QPushButton { color: white; opacity: 255; border-top-right-radius: 20px; border-bottom-right-radius: 20px; text-align: left; } QPushButton:hover:pressed { background-color: rgba(73, 69, 110, 200); }");
                m_SecuritySettingsButton->resize(QSize(m_MessengerWindowRect.x() * 0.7, m_MessengerWindowRect.y() * 0.05));
                m_SecuritySettingsButton->setText("Security Language");
                m_SecuritySettingsButton->setFont(m_OptionSettingsFont);
                //m_SecuritySettingsButton->setIcon(QIcon(":/Icons/Icons/icon-security.svg"));
                //m_SecuritySettingsButton->setIconSize(QSize(23, 23));
                m_SecuritySettingsButton->hide();

                m_MainLayout.addWidget(m_NotificationSettingsButton);
                m_NotificationSettingsButton->move(QPoint(m_MessengerWindowRect.x() * 0.2, m_MessengerWindowRect.y() * 0.48));
                m_NotificationSettingsButton->setStyleSheet("QPushButton { color: white; opacity: 255; border-top-right-radius: 20px; border-bottom-right-radius: 20px; text-align: left; } QPushButton:hover:pressed { background-color: rgba(73, 69, 110, 200); }");
                m_NotificationSettingsButton->resize(QSize(m_MessengerWindowRect.x() * 0.7, m_MessengerWindowRect.y() * 0.05));
                m_NotificationSettingsButton->setText("Notifications");
                m_NotificationSettingsButton->setFont(m_OptionSettingsFont);
                //m_NotificationSettingsButton->setIcon(QIcon(":/Icons/Icons/icon-notification.svg"));
                //m_NotificationSettingsButton->setIconSize(QSize(20, 20));
                m_NotificationSettingsButton->hide();

                m_MainLayout.addWidget(m_StylesSettingsButton);
                m_StylesSettingsButton->move(QPoint(m_MessengerWindowRect.x() * 0.2, m_MessengerWindowRect.y() * 0.54));
                m_StylesSettingsButton->setStyleSheet("QPushButton { color: white; opacity: 255; border-top-right-radius: 20px; border-bottom-right-radius: 20px; text-align: left; } QPushButton:hover:pressed { background-color: rgba(73, 69, 110, 200); }");
                m_StylesSettingsButton->resize(QSize(m_MessengerWindowRect.x() * 0.7, m_MessengerWindowRect.y() * 0.05));
                m_StylesSettingsButton->setText("Help");
                m_StylesSettingsButton->setFont(m_OptionSettingsFont);
                //m_StylesSettingsButton->setIcon(QIcon(":/Icons/Icons/icon-notification.svg"));
                //m_StylesSettingsButton->setIconSize(QSize(20, 20));
                m_StylesSettingsButton->hide();
            }
        }

        m_MainLayout.addWidget(m_SearchButtonUpside);
        m_SearchButtonUpside->move(QPoint(m_MessengerWindowRect.x() * 0.8, m_MessengerWindowRect.y() * 0.07));
        m_SearchButtonUpside->setStyleSheet("QPushButton { color: white; opacity: 255; border-top-right-radius: 20px;}");
        m_SearchButtonUpside->resize(QSize(m_MessengerWindowRect.x() * 0.1, m_MessengerWindowRect.y() * 0.1));
        m_SearchButtonUpside->setFont(m_UserStatusSettingFont);
        m_SearchButtonUpside->setIcon(QIcon(":/Icons/Icons/icon-search.svg"));
        m_SearchButtonUpside->setIconSize(QSize(20, 20));
        m_SearchButtonUpside->hide();

        m_MainLayout.addWidget(m_SwitchLightmodeButtonUpside);
        m_SwitchLightmodeButtonUpside->move(QPoint(m_MessengerWindowRect.x() * 0.7, m_MessengerWindowRect.y() * 0.07));
        m_SwitchLightmodeButtonUpside->setStyleSheet("QPushButton { color: white; opacity: 255; border-top-right-radius: 20px;}");
        m_SwitchLightmodeButtonUpside->resize(QSize(m_MessengerWindowRect.x() * 0.1, m_MessengerWindowRect.y() * 0.1));
        m_SwitchLightmodeButtonUpside->setIcon(QIcon(":/Icons/Icons/icon-nightmodewhite.svg"));
        m_SwitchLightmodeButtonUpside->setIconSize(QSize(20, 20));
        m_SwitchLightmodeButtonUpside->hide();

        m_MainLayout.addWidget(m_EtcButtonUpside);
        m_EtcButtonUpside->move(QPoint(m_MessengerWindowRect.x() * 0.9, m_MessengerWindowRect.y() * 0.07));
        m_EtcButtonUpside->setStyleSheet("QPushButton { color: white; opacity: 255; border-top-right-radius: 20px;}");
        m_EtcButtonUpside->resize(QSize(m_MessengerWindowRect.x() * 0.1, m_MessengerWindowRect.y() * 0.1));
        m_EtcButtonUpside->setIcon(QIcon(":/Icons/Icons/icon-more.svg"));
        m_EtcButtonUpside->setIconSize(QSize(20, 20));
        m_EtcButtonUpside->hide();

        // ~ Widgets Bar ~
        {
            m_MainLayout.addWidget(m_MessengerButton);
            m_MessengerButton->move(QPoint(QPoint(m_MessengerWindowRect.x() * 0.79, m_MessengerWindowRect.y() * 0.75)));
            m_MessengerButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            m_MessengerButton->setStyleSheet("QToolButton { color: white; opacity: 255; border-top-left-radius: 20px; }");
            m_MessengerButton->resize(QSize(m_MessengerWindowRect.x() / 3, m_MessengerWindowRect.y() * 0.07));
            m_MessengerButton->setText("Chats");
            m_MessengerButton->setIcon(QIcon(":/Icons/Icons/icon-chatdesign.svg"));
            m_MessengerButton->setIconSize(QSize(20, 20));
            m_MessengerButton->setFont(m_DinFont);
            m_MessengerButton->hide();

            m_MainLayout.addWidget(m_ProfileButton);
            m_ProfileButton->move(QPoint(m_MessengerWindowRect.x() * 0.79, m_MessengerWindowRect.y() * 0.5));
            m_ProfileButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            m_ProfileButton->setStyleSheet("QToolButton { color: white; opacity: 255; border-radius: 0px; }");
            m_ProfileButton->resize(QSize(m_MessengerWindowRect.x() / 3, m_MessengerWindowRect.y() * 0.07));
            m_ProfileButton->setText("Profile");
            m_ProfileButton->setIcon(QIcon(":/Icons/Icons/icon-account.svg"));
            m_ProfileButton->setIconSize(QSize(20, 20));
            m_ProfileButton->setFont(m_DinFont);
            m_ProfileButton->hide();

            m_MainLayout.addWidget(m_SettingsButton);
            m_SettingsButton->move(QPoint(m_MessengerWindowRect.x() * 0.79, m_MessengerWindowRect.y() * 0.25));
            m_SettingsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            m_SettingsButton->setStyleSheet("QToolButton { color: white; opacity: 255; border-top-right-radius: 20px; }");
            m_SettingsButton->resize(QSize(m_MessengerWindowRect.x() / 3, m_MessengerWindowRect.y() * 0.07));
            m_SettingsButton->setText("Settings");
            m_SettingsButton->setIcon(QIcon(":/Icons/Icons/icon-devices.svg"));
            m_SettingsButton->setIconSize(QSize(20, 20));
            m_SettingsButton->setFont(m_DinFont);
            m_SettingsButton->hide();
        }

    }

    // ~ Animation ~
    {
        m_LoginInputAnimation->setStartValue(m_InputBoxUserName->geometry());
        m_LoginInputAnimation->setEndValue(QRect(m_MessengerWindowRect.x() * 0.2, m_MessengerWindowRect.y() * 0.4, 0, 30));
        m_LoginInputAnimation->setDuration(500);

        m_PasswordInputAnimation->setStartValue(m_InputBoxPassword->geometry());
        m_PasswordInputAnimation->setEndValue(QRect(m_MessengerWindowRect.x() * 0.2, m_MessengerWindowRect.y() * 0.5, 0, 30));
        m_PasswordInputAnimation->setDuration(500);

        m_SubmitButtonAnimation->setStartValue(m_SubmitLoginButton->geometry());
        m_SubmitButtonAnimation->setEndValue(QRect(m_MessengerWindowRect.x() * 0.2, m_MessengerWindowRect.y() * 0.55, 0, 30));
        m_SubmitButtonAnimation->setDuration(500);

        m_RegistrationAnimation->setStartValue(m_RegisterLoginButton->geometry());
        m_RegistrationAnimation->setEndValue(QRect(m_MessengerWindowRect.x() * 0.51, m_MessengerWindowRect.y() * 0.55, 0, 30));
        m_RegistrationAnimation->setDuration(500);

        m_MessegesRectAnimation->setStartValue(QRect(0, m_MessengerWindowRect.y() + 150, m_MessengerWindowRect.x(), 2));
        m_MessegesRectAnimation->setEndValue(QRect(0, m_MessengerWindowRect.y() * 0.15, m_MessengerWindowRect.x(), m_MessengerWindowRect.y()));
        m_MessegesRectAnimation->setDuration(500);
    }

    // ~ Etc. ~
    {
        m_UserAvatar->scaled(QSize(10, 5), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        m_RoundedAvatarPath->setFillRule(Qt::WindingFill);
        m_RoundedAvatarPath->addRoundedRect(m_MessengerWindowRect.x() * 0.1, m_MessengerWindowRect.y() * 0.2, 70, 70, 70, 70);

        OnRLSocketConnect(); // Connect Messages Server
    }

    return;
}

void CMainWindow::GetGlobalScreenSize()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    m_GlobalScreenRect = screen->availableGeometry();

    qDebug() << "Returned global: " << m_GlobalScreenRect.width() << "" << m_GlobalScreenRect.height();

    if (m_GlobalScreenRect.width() == 0 || m_GlobalScreenRect.height() == 0)
    {
        QMessageBox::critical(this, "RL ERROR", "Ошибка значений окна!", QMessageBox::Button::Ok);
        return;
    }

    return;
}

void CMainWindow::paintEvent(QPaintEvent *event)
{
    QPainter main_painter(this);

    #if defined(_WIN32) // Do not use shit round on OSX
       main_painter.setClipPath(*m_RoundedWindowPath);
       main_painter.drawRect(QRect(0, 0, m_MessengerWindowRect.x(), m_MessengerWindowRect.y()));
    #endif

    main_painter.setPen(g_CColorSet.m_ColorZeroPen);

    switch(m_PageType)
    {
        g_CColorSet.m_ColorZeroPen.setWidth(0); // no ShitWhiteSpaces!

        // LOGIN PAGE
        case 0:
        {
            main_painter.setBrush(g_CColorSet.m_ColorBlacky);
            main_painter.drawRect(QRect(0, 0, m_MessengerWindowRect.x(), m_MessengerWindowRect.y() + 100));

            main_painter.drawPixmap(QRect(m_MessengerWindowRect.x() * 0.55, 0, 200, 200), QPixmap(":/UxObjects/UxObjects/Vectorcloud-righttop.svg"));
            main_painter.drawPixmap(QRect(0, m_MessengerWindowRect.y() * 0.6, 100, 200), QPixmap(":/UxObjects/UxObjects/cloud-righttopsmall.svg"));
            main_painter.drawPixmap(QRect(m_MessengerWindowRect.x() * 0.3, m_MessengerWindowRect.y(), 600, 250), QPixmap(":/UxObjects/UxObjects/cloud-rightdown.svg"));
            main_painter.drawImage(QRect(m_MessengerWindowRect.x() * 0.1, m_MessengerWindowRect.y() * 0.15, 50, 50), QImage(":/UxObjects/UxObjects/star.svg"));

        }
        break;

        // MESSAGES PAGE
        case 1:
        {
            main_painter.setBrush(g_CColorSet.m_PurplebackgroundColorBrush);
            main_painter.drawRect(QRect(0, 0, m_MessengerWindowRect.x(), m_MessengerWindowRect.y() + 100));

            // Gradient for Menu!
            {
                QLinearGradient gradient(0, m_MessengerWindowRect.y() * 0.3 , m_MessengerWindowRect.x(), m_MessengerWindowRect.y() * 0.3);

                gradient.setColorAt(0,      QColor(132, 164, 251));
                gradient.setColorAt(0.5,    QColor(142, 78, 222));
                gradient.setColorAt(1,      QColor( 72,47,180));
                main_painter.setBrush(gradient);
                main_painter.drawRect(rect());

                main_painter.setBrush(g_CColorSet.m_BlackyBackgroundColorBrush);
                main_painter.drawRoundedRect(m_BackgroundMainRect, 30, 30);
            }
        }
        break;

        case 2:
        {
            main_painter.setBrush(g_CColorSet.m_PurplebackgroundColorBrush);
            main_painter.drawRect(QRect(0, 0, m_MessengerWindowRect.x(), m_MessengerWindowRect.y() + 100));

            // Gradient for Menu!
            {
                QLinearGradient gradient(0, m_MessengerWindowRect.y() * 0.3 , m_MessengerWindowRect.x(), m_MessengerWindowRect.y() * 0.3);

                gradient.setColorAt(0,      QColor(132, 164, 251));
                gradient.setColorAt(0.5,    QColor(142, 78, 222));
                gradient.setColorAt(1,      QColor( 72,47,180));
                main_painter.setBrush(gradient);
                main_painter.drawRect(rect());

                main_painter.setBrush(g_CColorSet.m_BlackyBackgroundColorBrush);
                main_painter.drawRoundedRect(m_BackgroundMainRect, 30, 30);
            }
        }
        break;

        // SETTINGS PAGE
        case 3:
        {
            main_painter.setBrush(g_CColorSet.m_PurplebackgroundColorBrush);
            main_painter.drawRect(QRect(0, 0, m_MessengerWindowRect.x(), m_MessengerWindowRect.y() + 100));

            // Gradient for Menu!
            {
                QLinearGradient gradient(0, m_MessengerWindowRect.y() * 0.3 , m_MessengerWindowRect.x(), m_MessengerWindowRect.y() * 0.3);

                gradient.setColorAt(0,      QColor(132, 164, 251));
                gradient.setColorAt(0.5,    QColor(142, 78, 222));
                gradient.setColorAt(1,      QColor( 72,47,180));
                main_painter.setBrush(gradient);
                main_painter.drawRect(rect());

                main_painter.setBrush(g_CColorSet.m_BlackyBackgroundColorBrush);
                main_painter.drawRoundedRect(m_BackgroundMainRect, 30, 30);
            }

            // Center block
            main_painter.setBrush(g_CColorSet.m_PurplebackgroundLightColorBrush);
            main_painter.drawRoundedRect(QRect(m_MessengerWindowRect.x() * 0.04, m_MessengerWindowRect.y() * 0.3, m_MessengerWindowRect.x() * 0.87, m_MessengerWindowRect.y() * 0.65), 20, 20);

            main_painter.drawPixmap(QRect(m_MessengerWindowRect.x() * 0.12, m_MessengerWindowRect.y() * 0.34, m_MessengerWindowRect.x() * ((0.34+0.05)/1.63 - 0.12) / (2 * 1.627907), m_MessengerWindowRect.y() * 0.05), QPixmap(":/Icons/Icons/icon-nightmode.svg"));
            main_painter.drawPixmap(QRect(m_MessengerWindowRect.x() * 0.11, m_MessengerWindowRect.y() * 0.43, m_MessengerWindowRect.x() * ((0.43+0.05)/1.63 - 0.11) / (2 * 1.627907), m_MessengerWindowRect.y() * 0.05), QPixmap(":/Icons/Icons/icon-security.svg"));
            main_painter.drawPixmap(QRect(m_MessengerWindowRect.x() * 0.11, m_MessengerWindowRect.y() * 0.49, m_MessengerWindowRect.x() * ((0.43+0.05)/1.63 - 0.11) / (2 * 1.627907), m_MessengerWindowRect.y() * 0.05), QPixmap(":/Icons/Icons/icon-notification.svg"));
            main_painter.drawPixmap(QRect(m_MessengerWindowRect.x() * 0.11, m_MessengerWindowRect.y() * 0.55, m_MessengerWindowRect.x() * ((0.43+0.05)/1.63 - 0.11) / (2 * 1.627907), m_MessengerWindowRect.y() * 0.05), QPixmap(":/Icons/Icons/icon-help.svg"));

            // Line
            main_painter.setPen(g_CColorSet.m_SemiWhiteColorPen);
            main_painter.drawLine(QPoint(m_MessengerWindowRect.x() * 0.1, m_MessengerWindowRect.y() * 0.40), QPoint(m_MessengerWindowRect.x() * 0.90, m_MessengerWindowRect.y() * 0.40));
            main_painter.setPen(g_CColorSet.m_ColorZeroPen);

            //avatar_rect
            main_painter.drawEllipse(m_MessengerWindowRect.x() * 0.1, m_MessengerWindowRect.y() * 0.2, 70, 70);
            main_painter.setClipPath(*m_RoundedAvatarPath);
            main_painter.drawImage(QRect(m_MessengerWindowRect.x() * 0.1, m_MessengerWindowRect.y() * 0.2, 70, 70), *m_UserAvatar);
        }
        break;
    }
}

void CMainWindow::SubmitButtonEventPressed()
{
  if (!(g_CNetworkWrapper.CheckUserLogin(m_InputBoxUserName -> text().toStdString() + " " + m_InputBoxPassword -> text().toStdString())))
  {
    m_ErrorLabel -> setText(QString::fromStdString(g_CNetworkWrapper.GetErrorMessage()));
    m_ErrorLabel -> show();
  }
  else
  {
    this -> p_client = & g_CNetworkWrapper.client;
    this->Authentication();
    {
      m_LoginInputAnimation -> start(QAbstractAnimation::DeleteWhenStopped);
      m_PasswordInputAnimation -> start(QAbstractAnimation::DeleteWhenStopped);
      m_SubmitButtonAnimation -> start(QAbstractAnimation::DeleteWhenStopped);
      m_RegistrationAnimation -> start(QAbstractAnimation::DeleteWhenStopped);
      m_MessegesRectAnimation -> start();

      m_MessengerButton -> show();
      m_ProfileButton -> show();
      m_SettingsButton -> show();
      m_SearchButtonUpside -> show();
      m_SwitchLightmodeButtonUpside -> show();
      m_EtcButtonUpside -> show();

      m_HeaderIntoLabel -> hide();
      m_UsernameIntoLabel -> hide();
      m_UserPasswordIntoLabel -> hide();
      m_CopyrightLabel -> hide();
      m_ErrorLabel -> hide();
      ChatsButtonEventPressed();
      // Auto-Click for first page after submit
    }
  }
  return;
}

void CMainWindow::ChatsButtonEventPressed()
{
    m_PageType = 1;
    m_CurrentPage->setText("Chats");
    m_CurrentPage->resize(m_MessengerWindowRect.x() * 0.3, m_MessengerWindowRect.y() * 0.06);

    m_UsernameLabelSettings->hide();
    m_UserStatusLabelSettings->hide();
    m_LightModeLabelSettings->hide();
    m_SecuritySettingsButton->hide();
    m_NotificationSettingsButton->hide();
    m_StylesSettingsButton->hide();
    m_LightModeSettingsCheckBox->hide();

    m_MessagesTextBrowser->hide();
    m_MessageLineEdit->hide();
    m_SendButton->hide();
    m_ChatName->hide();

    m_SearchButtonUpside->show();
    m_SwitchLightmodeButtonUpside->show();

    m_Dialogs->show();
    /*m_MessagesTextBrowser->show();
    m_MessageLineEdit->show();
    m_SendButton->show();
    */
    this->repaint();
    return;
}

void CMainWindow::m_DialogsItemPressed(QListWidgetItem* item)
{
    m_Dialogs->hide();
    m_MessagesTextBrowser->show();
    m_MessageLineEdit->show();
    m_SendButton->show();

    m_ChatName->setText(item->text());
    m_ChatName->show();

    //this may be useful after server update
    /*
    if(m_Dialogs->item(0) == item)
    {

    }
    */
    this->repaint();
}

void CMainWindow::ProfileButtonEventPressed()
{
    m_PageType = 2;
    m_CurrentPage->setText("Profile");

    m_UsernameLabelSettings->hide();
    m_UserStatusLabelSettings->hide();
    m_LightModeLabelSettings->hide();
    m_SecuritySettingsButton->hide();
    m_NotificationSettingsButton->hide();
    m_StylesSettingsButton->hide();
    m_LightModeSettingsCheckBox->hide();

    m_Dialogs->hide();
    m_MessagesTextBrowser->hide();
    m_MessageLineEdit->hide();
    m_SendButton->hide();

    m_SearchButtonUpside->show();
    m_SwitchLightmodeButtonUpside->show();

    this->repaint();
    return;
}

void CMainWindow::SettingsButtonEventPressed()
{
    m_PageType = 3;
    m_CurrentPage->setText("Settings");

    m_SwitchLightmodeButtonUpside->hide();
    m_SearchButtonUpside->hide();

    m_Dialogs->hide();
    m_MessagesTextBrowser->hide();
    m_MessageLineEdit->hide();
    m_SendButton->hide();

    m_UsernameLabelSettings->show();
    m_UserStatusLabelSettings->show();

    m_LightModeLabelSettings->show();
    m_LightModeSettingsCheckBox->show();

    m_SecuritySettingsButton->show();
    m_NotificationSettingsButton->show();
    m_StylesSettingsButton->show();

    this->repaint();
    return;
}

void CMainWindow::LightModeState(int state)
{
    if (state == Qt::Checked)
    {
        std::cout << "LightMode: Night" << std::endl;
        g_CColorSet.m_PurpleBackgroundColor = { 46, 44, 70 };
        g_CColorSet.m_PurplebackgroundColorBrush = { g_CColorSet.m_PurpleBackgroundColor, Qt::SolidPattern };
        this->repaint();
    }
    else
    {
        g_CColorSet.m_PurpleBackgroundColor = { 255, 255, 255 };
        g_CColorSet.m_PurplebackgroundColorBrush = { g_CColorSet.m_PurpleBackgroundColor, Qt::SolidPattern };
        this->repaint();
        std::cout << "LightMode: Day" << std::endl;
    }
}

QRect CMainWindow::GetMessagesRect() const
{
    return m_BackgroundMainRect;
}

void CMainWindow::SetMessagesRect(const QRect &rect)
{
    m_BackgroundMainRect = rect;
    return;
}

// ~ Login Input Boxes ~
void CMainWindow::GetPasswordText(const QString& inputed_text)
{
    m_PasswordText = inputed_text;
    return;
}

void CMainWindow::GetLoginText(const QString& inputed_text)
{
    m_LoginText = inputed_text;
    return;
}

// ~ Server Chat ~
void CMainWindow::GetMessage(const QString& inputed_text)
{
    m_MessageTextStr = inputed_text;
    std::cout << "[RL CChat Info]: Input text: " << m_MessageTextStr.toStdString() << std::endl;
    return;
}

void CMainWindow::OnRLSocketConnect()
{
    std::cout << "[RL CChat Info]: Connect button" << std::endl;
    //m_TcpSocket->connectToHost("193.105.114.182", 40334);
}

void CMainWindow::OnReadyRead()
{
    QDataStream l_ReadTcpSocket(m_TcpSocket);
    l_ReadTcpSocket.setVersion(QDataStream::Version::Qt_6_4);
    if (l_ReadTcpSocket.status() == QDataStream::Ok)
    {
        for (;;)
        {
            if (m_MessageBlockSize == 0)
            {
                if (m_TcpSocket->bytesAvailable() < 2)
                {
                    break;
                }
                l_ReadTcpSocket >> m_MessageBlockSize;
            }

            if (m_TcpSocket->bytesAvailable() < m_MessageBlockSize)
            {
                break;
            }

            QString l_ReceivedMessage;
            l_ReadTcpSocket >> l_ReceivedMessage;
            m_MessageBlockSize = 0;
            m_MessagesTextBrowser->append(l_ReceivedMessage);
        }
    }
    else
    {
        m_MessagesTextBrowser->append("[RL CCLient Error]: read error");
    }
}

void CMainWindow::SendToServer()
{
    m_DataByteArray.clear();
    QDataStream l_DataSendSize(&m_DataByteArray, QIODevice::WriteOnly);
    l_DataSendSize.setVersion(QDataStream::Version::Qt_6_4);

    l_DataSendSize << quint16(0) << m_MessageTextStr;
    l_DataSendSize.device()->seek(0);
    l_DataSendSize << quint16(m_DataByteArray.size() - sizeof(quint16));

    std::cout << "[RL CChat Info]: Server Send: " << m_MessageTextStr.toStdString() << std::endl;

    m_TcpSocket->write(m_DataByteArray);

    m_MessageLineEdit->clear();
}

// ~ DESKTOP AREA | HANDLES ~
#if defined(_WIN32) || defined(__MACH__)

// HANDLES
void CMainWindow::keyPressEvent(QKeyEvent* event)
{
    if( event->key() == Qt::Key_Escape )
    {
        #ifdef SET_MESSENGER_MODE_DEBUG
            qDebug() << "[RL INFO]: Escape button - Exiting...";
        #endif
        QApplication::exit();
    }
}

// HANDLE: Press Event
void CMainWindow::mousePressEvent(QMouseEvent* event)
{
    last_mouse_pos = event->pos();
    switch (event->button())
    {
        case Qt::LeftButton:
        {
            left_button_pressed = true;
        }
        break;

        default:
        {
            left_button_pressed = false;
        }
        break;
    }
    QMainWindow::mousePressEvent(event);
}

// HANDLE: Release mouse Event
void CMainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    left_button_pressed = false;
    QMainWindow::mouseReleaseEvent(event);
}

// HANDLE: Mouse move Event
void CMainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (left_button_pressed)
    {
        QPoint shift = event->pos() - last_mouse_pos;
        this->move(this->pos() + shift);
    }
    QMainWindow::mouseMoveEvent(event);
}

void CMainWindow::Authentication()
{
    m_UsernameLabelSettings -> setText(QString::fromStdString(p_client -> name));

    for(std::string client_name: this->p_client->dialogs)
    {
        QListWidgetItem* new_item = new QListWidgetItem(QString::fromStdString(client_name));
        new_item->setFont(m_DinFont);
        m_Dialogs->addItem(new_item);
    }
}

#endif

// ~ Destructor ~
CMainWindow::~CMainWindow()
{
    m_MessengerWindowRect = { -10, 10 };
}

