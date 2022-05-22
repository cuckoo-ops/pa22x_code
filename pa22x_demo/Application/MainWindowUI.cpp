#include "MainWindow.h"
#include <QLabel>
#include <QDebug>
#include <QTcpSocket>
#include <QEventLoop>
#include <QTimer>
#include <QStackedLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QFileDialog>

#if defined(_MSC_VER)
#pragma execution_character_set("utf-8")
#endif

void MainWindow::SetupConnectUI()
{
    _connectionStatusLabel = new QLabel;
    _ui->statusBar->addPermanentWidget(_connectionStatusLabel);
    connect(_ui->server_connect_button, SIGNAL(released()), this, SLOT(ConnectToServer()));
}

void MainWindow::SetupPADisplayUI()
{
    _fpsLabel = new QLabel;
    _ui->statusBar->addPermanentWidget(_fpsLabel);

    connect(_ui->pa_group1_image_view, &ImageView::toggled, this, [=]() {
        emit PAGroupSelect(PA22XClient::_pa_group1);
    });
    connect(_ui->pa_group2_image_view, &ImageView::toggled, this, [=]() {
        emit PAGroupSelect(PA22XClient::_pa_group2);
    });
    connect(_ui->pa_group3_image_view, &ImageView::toggled, this, [=]() {
        emit PAGroupSelect(PA22XClient::_pa_group3);
    });
    connect(_ui->pa_group4_image_view, &ImageView::toggled, this, [=]() {
        emit PAGroupSelect(PA22XClient::_pa_group4);
    });
}

void MainWindow::SetupPEDisplayUI()
{
    _pe_channel_select_group = new QButtonGroup(this);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_1 ,1);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_2 ,2);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_3 ,3);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_4 ,4);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_5 ,5);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_6 ,6);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_7 ,7);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_8 ,8);

    connect(_ui->pe_channel_select_1, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            emit PEChannelSelect(PA22XClient::_pe_channel1);
    });

    connect(_ui->pe_channel_select_2, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            emit PEChannelSelect(PA22XClient::_pe_channel2);
    });

    connect(_ui->pe_channel_select_3, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            emit PEChannelSelect(PA22XClient::_pe_channel3);
    });

    connect(_ui->pe_channel_select_4, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            emit PEChannelSelect(PA22XClient::_pe_channel4);
    });

    connect(_ui->pe_channel_select_5, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            emit PEChannelSelect(PA22XClient::_pe_channel5);
    });

    connect(_ui->pe_channel_select_6, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            emit PEChannelSelect(PA22XClient::_pe_channel6);
    });

    connect(_ui->pe_channel_select_7, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            emit PEChannelSelect(PA22XClient::_pe_channel7);
    });

    connect(_ui->pe_channel_select_8, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            emit PEChannelSelect(PA22XClient::_pe_channel8);
    });
}

void MainWindow::SetupScanTabSelectUI()
{
    connect(_ui->scan_tabs,
            static_cast<void(QTabWidget::*)(int)>(&QTabWidget::currentChanged), [=](int index) {
                DeviceSelect();
    });
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {
        e->ignore();
        return true;
    }
    return MainWindow::eventFilter(o, e);
}

void MainWindow::SetupPASettingUI()
{
    // PA Setting again input init
    _ui->pa_setting_again_input_step->addItem("0.1",	QVariant(InputStep_0_1));
    _ui->pa_setting_again_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pa_setting_again_input_step->addItem("2",		QVariant(InputStep_2));
    _ui->pa_setting_again_input_step->addItem("6",		QVariant(InputStep_6));
    _ui->pa_setting_again_input_step->setCurrentIndex(2);
    connect(_ui->pa_setting_again_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pa_setting_again_input->setSingleStep(_ui->pa_setting_again_input_step->currentData().toDouble());
    });

    _ui->pa_setting_again_input->setSingleStep(_ui->pa_setting_again_input_step->currentData().toDouble());
    _ui->pa_setting_again_input->setMinimum(0.0f);
    _ui->pa_setting_again_input->setMaximum(50.5f);
    connect(_ui->pa_setting_again_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                _pa22x_client.sendCommand("set again " + QString::number(i));
    });
    _ui->pa_setting_again_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting dgain input init
    _ui->pa_setting_dgain_input_step->addItem("0.1",	QVariant(InputStep_0_1));
    _ui->pa_setting_dgain_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pa_setting_dgain_input_step->addItem("2",		QVariant(InputStep_2));
    _ui->pa_setting_dgain_input_step->addItem("6",		QVariant(InputStep_6));
    _ui->pa_setting_dgain_input_step->setCurrentIndex(2);
    connect(_ui->pa_setting_dgain_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pa_setting_dgain_input->setSingleStep(_ui->pa_setting_dgain_input_step->currentData().toDouble());
    });

    _ui->pa_setting_dgain_input->setSingleStep(_ui->pa_setting_dgain_input_step->currentData().toDouble());
    _ui->pa_setting_dgain_input->setMinimum(0.0f);
    _ui->pa_setting_dgain_input->setMaximum(48.0f);
    connect(_ui->pa_setting_dgain_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                _pa22x_client.sendCommand("set dgain " + QString::number(i));
    });
    _ui->pa_setting_dgain_input->findChild<QLineEdit*>()->setReadOnly(true);



    // PA Setting voltage input init
    _ui->pa_setting_voltage_input->setSingleStep(5);
    _ui->pa_setting_voltage_input->setMinimum(0);
    _ui->pa_setting_voltage_input->setMaximum(95);
    connect(_ui->pa_setting_voltage_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
                _pa22x_client.sendCommand("set high_voltage " + QString::number(i));
    });
    _ui->pa_setting_voltage_input->findChild<QLineEdit*>()->setReadOnly(true);



    // PA Setting pulse_width input init
    _ui->pa_setting_pulse_width_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pa_setting_pulse_width_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pa_setting_pulse_width_input_step->addItem("10",		QVariant(InputStep_10));
    _ui->pa_setting_pulse_width_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_pulse_width_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pa_setting_pulse_width_input->setSingleStep(_ui->pa_setting_pulse_width_input_step->currentData().toInt());
    });

    _ui->pa_setting_pulse_width_input->setSingleStep(_ui->pa_setting_pulse_width_input_step->currentData().toInt());
    _ui->pa_setting_pulse_width_input->setMinimum(25);
    _ui->pa_setting_pulse_width_input->setMaximum(600);
    connect(_ui->pa_setting_pulse_width_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
                _pa22x_client.sendCommand("set pulse_width " + QString::number(i));
    });
    _ui->pa_setting_pulse_width_input->findChild<QLineEdit*>()->setReadOnly(true);



    // PA Setting prf input init
    _ui->pa_setting_prf_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pa_setting_prf_input_step->addItem("50",	QVariant(InputStep_50));
    _ui->pa_setting_prf_input_step->addItem("100",	QVariant(InputStep_100));
    _ui->pa_setting_prf_input_step->addItem("200",	QVariant(InputStep_200));
    _ui->pa_setting_prf_input_step->setCurrentIndex(3);
    connect(_ui->pa_setting_prf_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
                _ui->pa_setting_prf_input->setSingleStep(_ui->pa_setting_prf_input_step->currentData().toInt());
    });

    _ui->pa_setting_prf_input->setSingleStep(_ui->pa_setting_prf_input_step->currentData().toInt());
    _ui->pa_setting_prf_input->setMinimum(25);
    _ui->pa_setting_prf_input->setMaximum(10000);
    connect(_ui->pa_setting_prf_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i){
                _pa22x_client.sendCommand("set prf " + QString::number(i));
    });
    _ui->pa_setting_prf_input->findChild<QLineEdit*>()->setReadOnly(true);


    // PA Setting range input init
    _ui->pa_setting_range_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pa_setting_range_input_step->addItem("10",		QVariant(InputStep_10));
    _ui->pa_setting_range_input_step->addItem("100",	QVariant(InputStep_100));
    _ui->pa_setting_range_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_range_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pa_setting_range_input->setSingleStep(_ui->pa_setting_range_input_step->currentData().toDouble());
    });

    _ui->pa_setting_range_input->setSingleStep(_ui->pa_setting_range_input_step->currentData().toDouble());
    _ui->pa_setting_range_input->setMinimum(0.0f);
    _ui->pa_setting_range_input->setMaximum(10000.0f);
    connect(_ui->pa_setting_range_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                _pa22x_client.sendCommand("set range " + QString::number(i));
    });
    _ui->pa_setting_range_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting filter input init
    _ui->pa_setting_filter_input->addItem("0.5M - 10M",	QVariant(FilterType0));
    _ui->pa_setting_filter_input->addItem("  4M - 12M",	QVariant(FilterType1));
    _ui->pa_setting_filter_input->addItem("  6M - 20M",	QVariant(FilterType2));
    _ui->pa_setting_filter_input->setCurrentIndex(1);
    connect(_ui->pa_setting_filter_input,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _pa22x_client.sendCommand("set filter " + QString::number(_ui->pa_setting_filter_input->currentData().toInt()));
    });


    // PA Setting gate a pos input init
    _ui->pa_setting_gate_a_pos_input_step->addItem("0.1",	QVariant(InputStep_0_1));
    _ui->pa_setting_gate_a_pos_input_step->addItem("1", 	QVariant(InputStep_1));
    _ui->pa_setting_gate_a_pos_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pa_setting_gate_a_pos_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pa_setting_gate_a_pos_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_a_pos_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pa_setting_gate_a_pos_input->setSingleStep(_ui->pa_setting_gate_a_pos_input_step->currentData().toDouble());
    });

    _ui->pa_setting_gate_a_pos_input->setSingleStep(_ui->pa_setting_gate_a_pos_input_step->currentData().toDouble());
    _ui->pa_setting_gate_a_pos_input->setMinimum(0.0f);
    _ui->pa_setting_gate_a_pos_input->setMaximum(10000.0f);
    connect(_ui->pa_setting_gate_a_pos_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                double gate_pos = i;
                double gate_width = _ui->pa_setting_gate_a_width_input->value();
                int gate_height = _ui->pa_setting_gate_a_height_input->value();
                _pa22x_client.sendCommand("set gate_pos a " + QString::number(gate_pos) +
                                          " " + QString::number(gate_width) +
                                          " " + QString::number(gate_height));
    });
    _ui->pa_setting_gate_a_pos_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting gate a width input init
    _ui->pa_setting_gate_a_width_input_step->addItem("0.1",	QVariant(InputStep_0_1));
    _ui->pa_setting_gate_a_width_input_step->addItem("1", 	QVariant(InputStep_1));
    _ui->pa_setting_gate_a_width_input_step->addItem("5",	QVariant(InputStep_5));
    _ui->pa_setting_gate_a_width_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pa_setting_gate_a_width_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_a_width_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pa_setting_gate_a_width_input->setSingleStep(_ui->pa_setting_gate_a_width_input_step->currentData().toDouble());
    });

    _ui->pa_setting_gate_a_width_input->setSingleStep(_ui->pa_setting_gate_a_width_input_step->currentData().toDouble());
    _ui->pa_setting_gate_a_width_input->setMinimum(1.0f);
    _ui->pa_setting_gate_a_width_input->setMaximum(10000.0f);
    connect(_ui->pa_setting_gate_a_width_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                double gate_pos =  _ui->pa_setting_gate_a_pos_input->value();
                double gate_width = i;
                int gate_height = _ui->pa_setting_gate_a_height_input->value();
                _pa22x_client.sendCommand("set gate_pos a " + QString::number(gate_pos) +
                                          " " + QString::number(gate_width) +
                                          " " + QString::number(gate_height));
    });
    _ui->pa_setting_gate_a_width_input->findChild<QLineEdit*>()->setReadOnly(true);


    // PA Setting gate a height input init
    _ui->pa_setting_gate_a_height_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pa_setting_gate_a_height_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pa_setting_gate_a_height_input_step->addItem("10",		QVariant(InputStep_10));
    _ui->pa_setting_gate_a_width_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_a_height_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pa_setting_gate_a_height_input->setSingleStep(_ui->pa_setting_gate_a_height_input_step->currentData().toInt());
    });

    _ui->pa_setting_gate_a_height_input->setSingleStep(_ui->pa_setting_gate_a_height_input_step->currentData().toInt());
    _ui->pa_setting_gate_a_height_input->setMinimum(0);
    _ui->pa_setting_gate_a_height_input->setMaximum(100);
    connect(_ui->pa_setting_gate_a_height_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
                double gate_pos =  _ui->pa_setting_gate_a_pos_input->value();
                double gate_width = _ui->pa_setting_gate_a_width_input->value();
                int gate_height = i;
                _pa22x_client.sendCommand("set gate_pos a " + QString::number(gate_pos) +
                                          " " + QString::number(gate_width) +
                                          " " + QString::number(gate_height));
    });
    _ui->pa_setting_gate_a_height_input->findChild<QLineEdit*>()->setReadOnly(true);



    // PA Setting gate b pos input init
    _ui->pa_setting_gate_b_pos_input_step->addItem("0.1",	QVariant(InputStep_0_1));
    _ui->pa_setting_gate_b_pos_input_step->addItem("1", 	QVariant(InputStep_1));
    _ui->pa_setting_gate_b_pos_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pa_setting_gate_b_pos_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pa_setting_gate_b_pos_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_b_pos_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pa_setting_gate_b_pos_input->setSingleStep(_ui->pa_setting_gate_b_pos_input_step->currentData().toDouble());
    });

    _ui->pa_setting_gate_b_pos_input->setSingleStep(_ui->pa_setting_gate_b_pos_input_step->currentData().toDouble());
    _ui->pa_setting_gate_b_pos_input->setMinimum(0.0f);
    _ui->pa_setting_gate_b_pos_input->setMaximum(10000.0f);
    connect(_ui->pa_setting_gate_b_pos_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                double gate_pos = i;
                double gate_width = _ui->pa_setting_gate_b_width_input->value();
                int gate_height = _ui->pa_setting_gate_b_height_input->value();
                _pa22x_client.sendCommand("set gate_pos b " + QString::number(gate_pos) + " " + QString::number(gate_width) + " " + QString::number(gate_height));

    });
    _ui->pa_setting_gate_b_pos_input->findChild<QLineEdit*>()->setReadOnly(true);



    // PA Setting gate b width input init
    _ui->pa_setting_gate_b_width_input_step->addItem("0.1",	QVariant(InputStep_0_1));
    _ui->pa_setting_gate_b_width_input_step->addItem("1", 	QVariant(InputStep_1));
    _ui->pa_setting_gate_b_width_input_step->addItem("5",	QVariant(InputStep_5));
    _ui->pa_setting_gate_b_width_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pa_setting_gate_b_width_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_b_width_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pa_setting_gate_b_width_input->setSingleStep(_ui->pa_setting_gate_b_width_input_step->currentData().toDouble());
    });

    _ui->pa_setting_gate_b_width_input->setSingleStep(_ui->pa_setting_gate_b_width_input_step->currentData().toDouble());
    _ui->pa_setting_gate_b_width_input->setMinimum(0.0f);
    _ui->pa_setting_gate_b_width_input->setMaximum(10000.0f);
    connect(_ui->pa_setting_gate_b_width_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                double gate_pos =  _ui->pa_setting_gate_b_pos_input->value();
                double gate_width = i;
                int gate_height = _ui->pa_setting_gate_b_height_input->value();
                _pa22x_client.sendCommand("set gate_pos b " + QString::number(gate_pos) + " " + QString::number(gate_width) + " " + QString::number(gate_height));
    });
    _ui->pa_setting_gate_b_width_input->findChild<QLineEdit*>()->setReadOnly(true);



    // PA Setting gate b height input init
    _ui->pa_setting_gate_b_height_input_step->addItem("1", 	QVariant(InputStep_1));
    _ui->pa_setting_gate_b_height_input_step->addItem("5",	QVariant(InputStep_5));
    _ui->pa_setting_gate_b_height_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pa_setting_gate_b_height_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_b_height_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pa_setting_gate_b_height_input->setSingleStep(_ui->pa_setting_gate_b_height_input_step->currentData().toInt());
    });

    _ui->pa_setting_gate_b_height_input->setSingleStep(_ui->pa_setting_gate_b_height_input_step->currentData().toInt());
    _ui->pa_setting_gate_b_height_input->setMinimum(0);
    _ui->pa_setting_gate_b_height_input->setMaximum(100);
    connect(_ui->pa_setting_gate_b_height_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
                double gate_pos =  _ui->pa_setting_gate_b_pos_input->value();
                double gate_width = _ui->pa_setting_gate_b_width_input->value();
                int gate_height = i;
                _pa22x_client.sendCommand("set gate_pos b " + QString::number(gate_pos) + " " + QString::number(gate_width) + " " + QString::number(gate_height));
    });
    _ui->pa_setting_gate_b_height_input->findChild<QLineEdit*>()->setReadOnly(true);



    _ui->pa_setting_color_mode_input->addItem("彩色", QVariant(kImageViewColorMode0));
    _ui->pa_setting_color_mode_input->addItem("单色", QVariant(kImageViewColorMode1));
    connect(_ui->pa_setting_color_mode_input,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
            _color_mode = _ui->pa_setting_color_mode_input->currentData().toInt();
            _ui->pa_group1_image_view->setColorMode((ImageViewColorMode)_color_mode);
            _ui->pa_group2_image_view->setColorMode((ImageViewColorMode)_color_mode);
            _ui->pa_group3_image_view->setColorMode((ImageViewColorMode)_color_mode);
            _ui->pa_group4_image_view->setColorMode((ImageViewColorMode)_color_mode);
            _ui->pa_group_active_image_view->setColorMode((ImageViewColorMode)_color_mode);

    });


    _ui->pa_setting_a_wave_index_input->setMinimum(1);
    _ui->pa_setting_a_wave_index_input->setMaximum(128);
    connect(_ui->pa_setting_a_wave_index_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
            _selected_beam_number = i;
    });
    _ui->pa_setting_a_wave_index_input->findChild<QLineEdit*>()->setReadOnly(true);
}

void MainWindow::SetupPAScanRuleUI()
{
    _ui->pa_scan_rule_type_input->addItem("S",              QVariant(PAScanRuleTypeSScan));
    _ui->pa_scan_rule_type_input->addItem("L",              QVariant(PAScanRuleTypeLScan));
    _ui->pa_scan_rule_wedge_mode_input->addItem("Y",        QVariant(PAScanWedgeModeEnable));
    _ui->pa_scan_rule_wedge_mode_input->addItem("N",        QVariant(PAScanWedgeModeDisable));
    _ui->pa_scan_rule_wedge_type_input->addItem("S",        QVariant(PAScanWedgeTypeS));
    _ui->pa_scan_rule_wedge_type_input->addItem("L",        QVariant(PAScanWedgeTypeL));

    connect(_ui->pa_scan_rule_setup_button, SIGNAL(released()), this, SLOT(SetupScanRule()));
}


void MainWindow::SetupPESettingUI()
{
    _ui->pe_setting_gain_input_step->addItem("0.1",		QVariant(InputStep_0_1));
    _ui->pe_setting_gain_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pe_setting_gain_input_step->addItem("2",		QVariant(InputStep_2));
    _ui->pe_setting_gain_input_step->addItem("6",		QVariant(InputStep_6));
    _ui->pe_setting_gain_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_gain_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_gain_input->setSingleStep(_ui->pe_setting_gain_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gain_input->setSingleStep(_ui->pe_setting_gain_input_step->currentData().toDouble());
    _ui->pe_setting_gain_input->setMinimum(0.0f);
    _ui->pe_setting_gain_input->setMaximum(110.0f);
    connect(_ui->pe_setting_gain_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                _pa22x_client.sendCommand("set gain " + QString::number(i));
    });
    _ui->pe_setting_gain_input->findChild<QLineEdit*>()->setReadOnly(true);


    _ui->pe_setting_voltage_input->setSingleStep(25);
    _ui->pe_setting_voltage_input->setMinimum(-250);
    _ui->pe_setting_voltage_input->setMaximum(-50);
    connect(_ui->pe_setting_voltage_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
                _pa22x_client.sendCommand("set high_voltage " + QString::number(i));

    });
    _ui->pe_setting_voltage_input->findChild<QLineEdit*>()->setReadOnly(true);


    _ui->pe_setting_pulse_width_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pe_setting_pulse_width_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pe_setting_pulse_width_input_step->addItem("10",		QVariant(InputStep_10));
    connect(_ui->pe_setting_pulse_width_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_pulse_width_input->setSingleStep(_ui->pe_setting_pulse_width_input_step->currentData().toInt());
    });
    _ui->pe_setting_pulse_width_input->setSingleStep(_ui->pe_setting_pulse_width_input_step->currentData().toInt());
    _ui->pe_setting_pulse_width_input->setMinimum(25);
    _ui->pe_setting_pulse_width_input->setMaximum(600);
    connect(_ui->pe_setting_pulse_width_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
                _pa22x_client.sendCommand("set pulse_width " + QString::number(i));
    });
    _ui->pe_setting_pulse_width_input->findChild<QLineEdit*>()->setReadOnly(true);

    _ui->pe_setting_prf_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pe_setting_prf_input_step->addItem("50",	QVariant(InputStep_50));
    _ui->pe_setting_prf_input_step->addItem("100",	QVariant(InputStep_100));
    _ui->pe_setting_prf_input_step->addItem("200",	QVariant(InputStep_200));
    connect(_ui->pe_setting_prf_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
                _ui->pe_setting_prf_input->setSingleStep(_ui->pe_setting_prf_input_step->currentData().toInt());
    });
    _ui->pe_setting_prf_input->setSingleStep(_ui->pe_setting_prf_input_step->currentData().toInt());
    _ui->pe_setting_prf_input->setMinimum(25);
    _ui->pe_setting_prf_input->setMaximum(10000);
    connect(_ui->pe_setting_prf_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i){
                _pa22x_client.sendCommand("set prf " + QString::number(i));
    });
    _ui->pe_setting_prf_input->findChild<QLineEdit*>()->setReadOnly(true);

    _ui->pe_setting_range_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pe_setting_range_input_step->addItem("10",		QVariant(InputStep_10));
    _ui->pe_setting_range_input_step->addItem("100",	QVariant(InputStep_100));
    _ui->pe_setting_range_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_range_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_range_input->setSingleStep(_ui->pe_setting_range_input_step->currentData().toDouble());
    });
    _ui->pe_setting_range_input->setSingleStep(_ui->pe_setting_range_input_step->currentData().toDouble());
    _ui->pe_setting_range_input->setMinimum(0.0f);
    _ui->pe_setting_range_input->setMaximum(10000.0f);
    connect(_ui->pe_setting_range_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                _pa22x_client.sendCommand("set range " + QString::number(i));
    });
    _ui->pe_setting_range_input->findChild<QLineEdit*>()->setReadOnly(true);


    _ui->pe_setting_filter_input->addItem("  2M - 3M",	QVariant(FilterType0));
    _ui->pe_setting_filter_input->addItem("0.5M - 15M",	QVariant(FilterType1));
    _ui->pe_setting_filter_input->setCurrentIndex(0);
    connect(_ui->pe_setting_filter_input,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                qDebug("%d", _ui->pe_setting_filter_input->currentData().toInt());
                _pa22x_client.sendCommand("set filter " + QString::number(_ui->pe_setting_filter_input->currentData().toInt()));
    });

    _ui->pe_setting_gate_a_pos_input_step->addItem("0.1",	QVariant(InputStep_0_1));
    _ui->pe_setting_gate_a_pos_input_step->addItem("1", 	QVariant(InputStep_1));
    _ui->pe_setting_gate_a_pos_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pe_setting_gate_a_pos_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pe_setting_gate_a_pos_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_gate_a_pos_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_gate_a_pos_input->setSingleStep(_ui->pe_setting_gate_a_pos_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gate_a_pos_input->setSingleStep(_ui->pe_setting_gate_a_pos_input_step->currentData().toDouble());
    _ui->pe_setting_gate_a_pos_input->setMinimum(0.0f);
    _ui->pe_setting_gate_a_pos_input->setMaximum(10000.0f);
    connect(_ui->pe_setting_gate_a_pos_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                UpdatePEGateAPos(i);
    });
    _ui->pe_setting_gate_a_pos_input->findChild<QLineEdit*>()->setReadOnly(true);

    _ui->pe_setting_gate_a_width_input_step->addItem("0.1",	QVariant(InputStep_0_1));
    _ui->pe_setting_gate_a_width_input_step->addItem("1", 	QVariant(InputStep_1));
    _ui->pe_setting_gate_a_width_input_step->addItem("5",	QVariant(InputStep_5));
    _ui->pe_setting_gate_a_width_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pe_setting_gate_a_width_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_gate_a_width_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_gate_a_width_input->setSingleStep(_ui->pe_setting_gate_a_width_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gate_a_width_input->setSingleStep(_ui->pe_setting_gate_a_width_input_step->currentData().toDouble());
    _ui->pe_setting_gate_a_width_input->setMinimum(0.0f);
    _ui->pe_setting_gate_a_width_input->setMaximum(100.0f);
    connect(_ui->pe_setting_gate_a_width_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                UpdatePEGateAWidth(i);
    });
    _ui->pe_setting_gate_a_width_input->findChild<QLineEdit*>()->setReadOnly(true);

    _ui->pe_setting_gate_a_height_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pe_setting_gate_a_height_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pe_setting_gate_a_height_input_step->addItem("10",		QVariant(InputStep_10));
    _ui->pe_setting_gate_a_height_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_gate_a_height_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_gate_a_height_input->setSingleStep(_ui->pe_setting_gate_a_height_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gate_a_height_input->setSingleStep(_ui->pe_setting_gate_a_height_input_step->currentData().toDouble());
    _ui->pe_setting_gate_a_height_input->setMinimum(0);
    _ui->pe_setting_gate_a_height_input->setMaximum(100);
    connect(_ui->pe_setting_gate_a_height_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
                UpdatePEGateAHeight(i);
    });
    _ui->pe_setting_gate_a_height_input->findChild<QLineEdit*>()->setReadOnly(true);


    _ui->pe_setting_gate_b_pos_input_step->addItem("0.1",	QVariant(InputStep_0_1));
    _ui->pe_setting_gate_b_pos_input_step->addItem("1", 	QVariant(InputStep_1));
    _ui->pe_setting_gate_b_pos_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pe_setting_gate_b_pos_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pe_setting_gate_b_pos_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_gate_b_pos_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_gate_b_pos_input->setSingleStep(_ui->pe_setting_gate_b_pos_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gate_b_pos_input->setSingleStep(_ui->pe_setting_gate_b_pos_input_step->currentData().toDouble());
    _ui->pe_setting_gate_b_pos_input->setMinimum(0.0f);
    _ui->pe_setting_gate_b_pos_input->setMaximum(10000.0f);
    connect(_ui->pe_setting_gate_b_pos_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                UpdatePEGateBPos(i);
    });
    _ui->pe_setting_gate_b_pos_input->findChild<QLineEdit*>()->setReadOnly(true);

    _ui->pe_setting_gate_b_width_input_step->addItem("0.1",	QVariant(InputStep_0_1));
    _ui->pe_setting_gate_b_width_input_step->addItem("1", 	QVariant(InputStep_1));
    _ui->pe_setting_gate_b_width_input_step->addItem("5",	QVariant(InputStep_5));
    _ui->pe_setting_gate_b_width_input_step->addItem("10",	QVariant(InputStep_10));
    _ui->pe_setting_gate_b_width_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_gate_b_width_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_gate_b_width_input->setSingleStep(_ui->pe_setting_gate_b_width_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gate_b_width_input->setSingleStep(_ui->pe_setting_gate_b_width_input_step->currentData().toDouble());
    _ui->pe_setting_gate_b_width_input->setMinimum(0.0f);
    _ui->pe_setting_gate_b_width_input->setMaximum(100.0f);
    connect(_ui->pe_setting_gate_b_width_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                UpdatePEGateBWidth(i);
    });
    _ui->pe_setting_gate_b_width_input->findChild<QLineEdit*>()->setReadOnly(true);

    _ui->pe_setting_gate_b_height_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pe_setting_gate_b_height_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pe_setting_gate_b_height_input_step->addItem("10",		QVariant(InputStep_10));
    _ui->pe_setting_gate_b_height_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_gate_b_height_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_gate_b_height_input->setSingleStep(_ui->pe_setting_gate_b_height_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gate_b_height_input->setSingleStep(_ui->pe_setting_gate_b_height_input_step->currentData().toDouble());
    _ui->pe_setting_gate_b_height_input->setMinimum(0.0f);
    _ui->pe_setting_gate_b_height_input->setMaximum(100.0f);
    connect(_ui->pe_setting_gate_b_height_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
                UpdatePEGateBHeight(i);
    });
    _ui->pe_setting_gate_b_height_input->findChild<QLineEdit*>()->setReadOnly(true);


    _ui->pe_setting_delay_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pe_setting_delay_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pe_setting_delay_input_step->addItem("10",		QVariant(InputStep_10));
    connect(_ui->pe_setting_delay_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_delay_input->setSingleStep(_ui->pe_setting_delay_input_step->currentData().toDouble());
    });
    _ui->pe_setting_delay_input->setSingleStep(_ui->pe_setting_delay_input_step->currentData().toDouble());
    _ui->pe_setting_delay_input->setMinimum(0.0f);
    _ui->pe_setting_delay_input->setMaximum(1000000.0f);
    connect(_ui->pe_setting_delay_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                UpdatePEDelay(i);
    });
    _ui->pe_setting_delay_input->findChild<QLineEdit*>()->setReadOnly(true);

    _ui->pe_setting_origin_input_step->addItem("1", 	QVariant(InputStep_1));
    _ui->pe_setting_origin_input_step->addItem("5",		QVariant(InputStep_5));
    _ui->pe_setting_origin_input_step->addItem("10",	QVariant(InputStep_10));
    connect(_ui->pe_setting_origin_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_origin_input->setSingleStep(_ui->pe_setting_origin_input_step->currentData().toDouble());
    });
    _ui->pe_setting_origin_input->setSingleStep(_ui->pe_setting_origin_input_step->currentData().toDouble());
    _ui->pe_setting_origin_input->setMinimum(0.0f);
    _ui->pe_setting_origin_input->setMaximum(1000000.0f);
    connect(_ui->pe_setting_origin_input,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
                UpdatePEOrigin(i);
    });
    _ui->pe_setting_origin_input->findChild<QLineEdit*>()->setReadOnly(true);

    _ui->pe_setting_dual_mode_input->addItem("dual",        QVariant(PEModeDual));
    _ui->pe_setting_dual_mode_input->addItem("single",      QVariant(PEModeSingle));
    connect(_ui->pe_setting_dual_mode_input,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                if (_ui->pe_setting_dual_mode_input->currentData().toInt() == PEModeDual)
                    _pa22x_client.sendCommand("set dual_mode true");
                else
                    _pa22x_client.sendCommand("set dual_mode false");
    });

    _ui->pe_setting_damping_input->addItem("80",       QVariant(PEDamping80));
    _ui->pe_setting_damping_input->addItem("400",      QVariant(PEDamping400));
    connect(_ui->pe_setting_damping_input,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _pa22x_client.sendCommand("set damping " +
                                          QString::number(_ui->pe_setting_damping_input->currentData().toInt()));

    });

    _ui->pe_setting_velocity_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pe_setting_velocity_input_step->addItem("10",		QVariant(InputStep_10));
    _ui->pe_setting_velocity_input_step->addItem("100",		QVariant(InputStep_100));
    _ui->pe_setting_velocity_input_step->addItem("1000",	QVariant(InputStep_1000));
    connect(_ui->pe_setting_velocity_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_velocity_input->setSingleStep(_ui->pe_setting_velocity_input_step->currentData().toInt());
    });
    _ui->pe_setting_velocity_input->setSingleStep(_ui->pe_setting_velocity_input_step->currentData().toInt());
    _ui->pe_setting_velocity_input->setMinimum(500);
    _ui->pe_setting_velocity_input->setMaximum(15000);
    connect(_ui->pe_setting_velocity_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
                _pa22x_client.sendCommand("set velocity " + QString::number(i));
    });
    _ui->pe_setting_velocity_input->findChild<QLineEdit*>()->setReadOnly(true);


    _ui->pe_setting_average_rate_input_step->addItem("1", 		QVariant(InputStep_1));
    _ui->pe_setting_average_rate_input_step->addItem("10",		QVariant(InputStep_10));
    _ui->pe_setting_average_rate_input_step->addItem("100",		QVariant(InputStep_100));
    connect(_ui->pe_setting_average_rate_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
                _ui->pe_setting_average_rate_input->setSingleStep(_ui->pe_setting_average_rate_input_step->currentData().toInt());
    });
    _ui->pe_setting_average_rate_input->setSingleStep(_ui->pe_setting_average_rate_input_step->currentData().toInt());
    _ui->pe_setting_average_rate_input->setMinimum(1);
    _ui->pe_setting_average_rate_input->setMaximum(200);
    connect(_ui->pe_setting_average_rate_input,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
                _pa22x_client.setAverageLevel(-1, i);
    });
    _ui->pe_setting_average_rate_input->findChild<QLineEdit*>()->setReadOnly(true);
}

void MainWindow::SetupResponderUI()
{
    _ui->pa22x_client_responder_list_view->setModel(&_pa22x_client_responder_list);

    auto responder_list_add_item = [this](QString str) {
        int count = _pa22x_client_responder_list.rowCount();
        _pa22x_client_responder_list.insertRow(count);
        _pa22x_client_responder_list.setData(_pa22x_client_responder_list.index(count), str);
        _ui->pa22x_client_responder_list_view->setCurrentIndex(_pa22x_client_responder_list.index(count));
        ParserResponderString(str);
    };

    connect(&_pa22x_client, &PA22XClient::responseGot, responder_list_add_item);
    connect(&_pa22x_client, &PA22XClient::commandSent, responder_list_add_item);
}

void MainWindow::SetupCommanderUI()
{
    connect(_ui->pa22x_client_command_input, &QLineEdit::editingFinished, [this] {
        auto commandString = _ui->pa22x_client_command_input->text();
        if (!commandString.isEmpty()) {
            _pa22x_client.sendCommand(commandString);
        }
        _ui->pa22x_client_command_input->clear();
    });
}
