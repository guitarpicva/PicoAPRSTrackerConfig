#include "picoaprstrackerconfig.h"
#include "ui_picoaprstrackerconfig.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSerialPortInfo>
#include <QTimer>

PicoAPRSTrackerConfig::PicoAPRSTrackerConfig(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PicoAPRSTrackerConfig)
{
    ui->setupUi(this);

    QTimer::singleShot(100, this, &PicoAPRSTrackerConfig::connectToDevice);
}

PicoAPRSTrackerConfig::~PicoAPRSTrackerConfig()
{
    delete ui;
}

void PicoAPRSTrackerConfig::connectToDevice()
{
    if(sport) {
        sport->close();
        disconnect(sport, 0, 0, 0);
    }
    else {
        sport = new QSerialPort(this);
    }
    QStringList items;
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo p, ports)
        items<<p.portName();
    s_portName = QInputDialog::getItem(this, "Choose Port", "Choose the serial port of the Tracker device.", items, 3, false);
    if(s_portName.isEmpty()) {
        QMessageBox::warning(this, "Must Chooose Port", "A serial port must be chosen first!");
        return;
    }
    sport->setPortName(s_portName);
    if(sport->open(QSerialPort::ReadWrite)) {
        sport->setBaudRate(QSerialPort::Baud115200);
        connect(sport, &QSerialPort::readyRead, this, &PicoAPRSTrackerConfig::on_readyRead);
        sport->write("READCONFIG|\r");
    }
    else {
        QMessageBox::warning(this, "Unable to Connect", "Unable to connect to the chosen serial port.");
    }
}

void PicoAPRSTrackerConfig::on_readyRead()
{
    inbytes.append(sport->readAll());
    if(inbytes.count('|') == 5) { // six delimited fields
        ui->plainTextEdit->appendPlainText(inbytes);
        const QString tmp = inbytes;
        const QStringList tmpl = tmp.split("|");
        ui->sourceCallLineEdit->setText(tmpl.at(0));
        ui->digi1LineEdit->setText(tmpl.at(1));
        ui->digi2LineEdit->setText(tmpl.at(2));
        ui->commentLineEdit->setText(tmpl.at(3));
        ui->intervalComboBox->setCurrentText(tmpl.at(4));
        ui->aprsSymbolComboBox->setCurrentText(tmpl.at(5));
        inbytes.clear();
    }
}

void PicoAPRSTrackerConfig::on_actionE_xit_triggered()
{
    close();
}

void PicoAPRSTrackerConfig::on_action_Write_to_Device_triggered()
{
    // gather, evaluate and build the output string to send to the Pico to write to flash
    out.clear();
    QString tmp;
    QRegularExpression good = QRegularExpression("[0-9A-Z-]");
    tmp = ui->sourceCallLineEdit->text().trimmed().toUpper();
    if(tmp.length() > 4 && tmp.length() < 10) // must include the "-<ssid>" characters, so min of 3 call sign + 2 "-<ssid".
    {
        if(tmp.count(good) != tmp.length()) {

            QMessageBox::warning(this, "Bad Characters", "Some of the characters are not valid for a call sign.\n\n0-9, A-Z, - and / only.");
            return;
        }
        ui->plainTextEdit->appendPlainText("Valid Call Sign");
        if(!tmp.contains('-')) {
            if(tmp.length() > 6) {
                QMessageBox::warning(this, "Bad Characters", "Some of the characters are not valid for a call sign.\n\n0-9, A-Z, and \"-\" only.");
                return;
            }
            else {
                tmp.append("-12"); // default to a tracker device SSID
            }
        }
        out<<tmp;
    }
    tmp = ui->digi1LineEdit->text().trimmed().toUpper();
    if(tmp.length() > 2 && tmp.length() < 10) // OK!
    {
        if(tmp.count(good) != tmp.length()) {

            QMessageBox::warning(this, "Bad Characters", "Some of the characters are not valid for a call sign.\n\n0-9, A-Z, and \"-\" only.");
            return;
        }
        ui->plainTextEdit->appendPlainText("Valid Call Sign");
        out<<tmp;
    }
    else {
        out << "WIDE1-1";
    }
    tmp = ui->digi2LineEdit->text().trimmed().toUpper();
    if(tmp.length() > 2 && tmp.length() < 10) // OK!
    {
        if(tmp.count(good) != tmp.length()) {

            QMessageBox::warning(this, "Bad Characters", "Some of the characters are not valid for a call sign.\n\n0-9, A-Z, and \"-\" only.");
            return;
        }
        ui->plainTextEdit->appendPlainText("Valid Call Sign");
        out<<tmp;
    }
    else {
        out << "WIDE2-1";
    }
    // comment
    out<<ui->commentLineEdit->text().trimmed();
    // beacon interval
    out<< ui->intervalComboBox->currentText();
    // APRS symbol
    out<< ui->aprsSymbolComboBox->currentText().trimmed();
    ui->plainTextEdit->appendPlainText(out.join("|"));
}

void PicoAPRSTrackerConfig::on_action_Read_from_Device_triggered()
{
    if(sport) {
        sport->write("READCONFIG|\r");
    }
}

void PicoAPRSTrackerConfig::on_action_Serial_Port_triggered()
{
    connectToDevice();
}

void PicoAPRSTrackerConfig::on_sourceCallLineEdit_returnPressed()
{
    on_action_Write_to_Device_triggered();
}

void PicoAPRSTrackerConfig::on_writeConfigButton_clicked()
{
    on_action_Write_to_Device_triggered();
    if(QMessageBox::question(this, "Write Config?", "Write the configuration:\n\n" + out.join("|") + "\n\n to the device?") == QMessageBox::Yes)
        if(sport) {
            sport->write("WRITECONFIG|" + out.join("|").toLatin1() + "\r");
        }
}

void PicoAPRSTrackerConfig::on_readConfigButton_clicked()
{
    on_action_Read_from_Device_triggered();
}

