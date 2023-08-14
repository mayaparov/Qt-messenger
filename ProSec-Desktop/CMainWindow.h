#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>

// PAINT
#include <QPaintEvent>
#include <QPainter>
#include <QGuiApplication>
#include <QPainterPath>

// ERROR
#include <QMessageBox>

// UI
#include <QString>
#include <QFontDatabase>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QTextBrowser>
#include <QCheckBox>
#include <QListWidget>

#include <QTcpServer>
#include <QTcpSocket>

// ETC
#include <QPropertyAnimation>
#include <QtSvg/QSvgRenderer>
#include <QApplication>

#include "CColorSet/CColorSet.h"
#include "CNetworkWrapper/CNetworkWrapper.h"
#include "CClientData.h"

#define OSX_TARGET true

class CMainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QRect nrect READ GetMessagesRect WRITE SetMessagesRect)

public:
    CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

    virtual void    paintEvent              (QPaintEvent *event) override;

    #if defined(_WIN32) || defined(__MACH__)
    virtual void    keyPressEvent           (QKeyEvent* event) override;
    virtual void    mousePressEvent         (QMouseEvent* event) override;
    virtual void    mouseReleaseEvent       (QMouseEvent* event) override;
    virtual void    mouseMoveEvent          (QMouseEvent* event) override;
    #endif

    // ~ Functions ~
    void            GetGlobalScreenSize     ();
    void            LoginPage               (QPainter main_painter_ptr);
    void            SetupVars               ();

    // ~ Layouts [NA]~
    QGridLayout     m_MainLayout;
    QGridLayout     m_MessagerLayout;

    // ~ Input [Get] ~
    QLineEdit*      m_InputBoxUserName;
    QLineEdit*      m_InputBoxPassword;

    // ~ Buttons [Actions] ~
    QPushButton*    m_SubmitLoginButton;
    QPushButton*    m_RegisterLoginButton;
    QToolButton*    m_MessengerButton;
    QToolButton*    m_ProfileButton;
    QToolButton*    m_SettingsButton;

    // {  CHATS TAB }
    //QPushButton*    m_ConnectButton; // AUTO USERNAME!
    QListWidget*    m_Dialogs;
    QPushButton*    m_SendButton;
    QLineEdit*      m_MessageLineEdit;
    QTextBrowser*   m_MessagesTextBrowser;

    // {  SETTINGS TAB }
    QPushButton*    m_SecuritySettingsButton;
    QPushButton*    m_NotificationSettingsButton;
    QPushButton*    m_StylesSettingsButton;
    QPushButton*    m_HelpSettingsButton;

    QPushButton*    m_SearchButtonUpside;
    QPushButton*    m_SwitchLightmodeButtonUpside;
    QPushButton*    m_EtcButtonUpside;

    // ~ Painter Paths [UNKNWN]
    QPainterPath*   m_RoundedWindowPath;
    QPainterPath*   m_RoundedAvatarPath;

    // ~~ Images ~~
    QImage*          m_UserAvatar;

    // ~~ CheckBox [BOOL] ~~
    QCheckBox*      m_LightModeSettingsCheckBox;

    // ~ Fonts [Data] ~
    QFont           m_AzonixFont;
    QFont           m_DinFont;
    QFont           m_NoirFont;
    QFont           m_SfProDisplayRegularFont;
    QFont           m_SfProDisplayBoldFont;

    QFont           m_fProDisplaySettingsFont;
    QFont           m_UsernameSettingFont;
    QFont           m_UserStatusSettingFont;
    QFont           m_OptionSettingsFont;

    // ~ Server Chat [Data] ~
    void SendToServer();

    QString         m_MessageTextStr;
    QTcpSocket*     m_TcpSocket;
    QByteArray      m_DataByteArray;
    quint16         m_MessageBlockSize;

    // ~ Point [Data] ~
    QPoint          m_MessengerWindowRect;
    QPoint          crutch; //shitcode

    #if defined(_WIN32) || defined(__MACH__)
    QPoint          last_mouse_pos;
    #endif

    // ~ Labels [Text] ~
    QLabel* m_CurrentPage;

    QLabel* m_HeaderIntoLabel;
    QLabel* m_UsernameIntoLabel;
    QLabel* m_UserPasswordIntoLabel;
    QLabel* m_CopyrightLabel;

    QLabel* m_ErrorLabel;

    QLabel* m_ChatName;

    QLabel* m_UsernameLabelSettings;
    QLabel* m_UserStatusLabelSettings;
    QLabel* m_LightModeLabelSettings;



    // ~ Animations [DYNAMIC] ~
    QPropertyAnimation* m_LoginInputAnimation;
    QPropertyAnimation* m_PasswordInputAnimation;
    QPropertyAnimation* m_SubmitButtonAnimation;
    QPropertyAnimation* m_RegistrationAnimation;

    QPropertyAnimation* m_MessegesRectAnimation;
    QRect               GetMessagesRect() const;

    // ~~ Rects [VEC4] ~~
    QRect           m_GlobalScreenRect;
    QRect           m_BackgroundMainRect;

    // ~ Nums [Data] ~
    int             m_PageType;
    int             m_MobileScreenWidth;
    int             m_MobileScreenHeight;

    // ~ String [Data] ~
    QString         m_LoginText;
    QString         m_PasswordText;
    QString         m_CurrentUserName;

    // ~ Boolean [STATE] ~
    #if defined(_WIN32) || defined(__MACH__)
    bool            left_button_pressed;
    #endif

private:

    Client          *p_client;

    void            Authentication();

    void            DrawChatWindow();
    void            DrawSettingsWindow();
public slots:

    void            SubmitButtonEventPressed();
    void            ChatsButtonEventPressed();
    void            ProfileButtonEventPressed();
    void            SettingsButtonEventPressed();

    void            m_DialogsItemPressed    (QListWidgetItem* item);
    void            LightModeState          (int state);
    void            GetLoginText            (const QString& inputed_text);
    void            GetPasswordText         (const QString& inputed_text);
    void            SetMessagesRect         (const QRect &rect);

    // ~ Server Chat [NET] ~
    void GetMessage(const QString& inputed_text);
    void OnRLSocketConnect(); // onPushButtonClicked
    void OnReadyRead(); //slotReadyRead

};
#endif // CMAINWINDOW_H
