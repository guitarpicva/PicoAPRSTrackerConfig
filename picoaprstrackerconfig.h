#ifndef PICOAPRSTRACKERCONFIG_H
#define PICOAPRSTRACKERCONFIG_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class PicoAPRSTrackerConfig; }
QT_END_NAMESPACE

class PicoAPRSTrackerConfig : public QMainWindow
{
    Q_OBJECT

public:
    PicoAPRSTrackerConfig(QWidget *parent = nullptr);
    ~PicoAPRSTrackerConfig();

private slots:
    void on_actionE_xit_triggered();
    void on_action_Write_to_Device_triggered();
    void on_action_Read_from_Device_triggered();
    void on_action_Serial_Port_triggered();
    void connectToDevice();
    void on_readyRead();
    void on_sourceCallLineEdit_returnPressed();
    void on_writeConfigButton_clicked();
    void on_readConfigButton_clicked();

private:
    Ui::PicoAPRSTrackerConfig *ui;
    QSerialPort *sport = nullptr;
    QByteArray inbytes;
    QString s_portName;
    QStringList out; // holds the configuration values
};
#endif // PICOAPRSTRACKERCONFIG_H
