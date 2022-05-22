#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QStringListModel>
#include "PA22XClient.h"
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent *) override;

private:
    int _displayTimer;
    PA22XClient _pa22x_client;
    Ui::MainWindow* _ui;
    QButtonGroup* _pe_channel_select_group;
    QStringListModel _pa22x_client_responder_list;
    void UISetup();
    void UIEnable();
    void UIDisable();
    void UpdatePEParametersUI();
    void UpdatePAParametersUI();
    virtual void closeEvent (QCloseEvent *event);
    PASViewParameter _pa_view_parameter[UTShadowPADevice::_max_group];
    bool _ui_setup;

    const static QString _gate_a_label;
    const static QString _gate_b_label;
    const static QString _gate_c_label;
    const static QString _gate_d_label;

private slots:
    void ClientConnectToServer();
    void ClientDisconnectToServer();
    void PresentClientData();
    void SetupPAScanrule();
    void LoadConfigFile();
};

#endif // MAINWINDOW_H
