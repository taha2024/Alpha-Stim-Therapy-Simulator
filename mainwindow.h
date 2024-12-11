#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cesdevice.h"
#include <string.h>


/*
Class: MainWindow

Purpose: This class serves as the frontend representation of the CES device.

Usage: Provides functionality for and helps display the user interface.
       Handles buttons/tabs/spinboxes on device or in admin area:

*/


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CESDevice* device;
    QTimer* batteryTimer;
    QTimer* inactivityTimer;
    QTimer* skinOffTimer;
    int inactiveSeconds;

private slots:
    void powerClick();
    void recordClick();
    void downClick();
    void upClick();
    void selectClick();
    void returnClick();
    void powerLevelAdminChange(int);
    void skinContactAdminChange(int);
    void deviceEnabledChange(int);
    void batteryUpdate();
    void adminBatteryUpdate(int);
    void inactivityUpdate();
    void displayInactiveTime();
    void resetInactivity();
    void turnOnDevice();
    void turnOffDevice();
    void skinContactUpdate();
    void resetUI();

};
#endif // MAINWINDOW_H
