#include "MainWindow.h"
#include "NDTMath.h"
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
#include <QCloseEvent>

#if defined(_MSC_VER)
#pragma execution_character_set("utf-8")
#endif

const QString MainWindow::_gate_a_label = "A";
const QString MainWindow::_gate_b_label = "B";
const QString MainWindow::_gate_c_label = "C";
const QString MainWindow::_gate_d_label = "D";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _ui(new Ui::MainWindow),
      _ui_setup(false)
{
    _ui->setupUi(this);
    setFixedSize(size());
    connect(_ui->server_connect_button, SIGNAL(released()), this, SLOT(ClientConnectToServer()));
}

MainWindow::~MainWindow()
{
    killTimer(_displayTimer);
    _displayTimer = 0;
}

void MainWindow::UISetup()
{
    connect(_ui->scan_tabs, static_cast<void(QTabWidget::*)(int)>(&QTabWidget::currentChanged), [=](int i) {
        QVector<UTShadowDevice*> shadowDevices =_pa22x_client.shadowDevices();
        QString selectedIdentifier = QString("UnknowDevice_??");

        if (i == 0) {
            // Select the first PA device
            for (UTShadowDevice* device : shadowDevices) {
                if (isPAShadowDeviceIdentifier(device->identifier())) {
                    selectedIdentifier = device->identifier();
                    break;
                }
            }
        }
        else if (i == 1) {
            // Select the first PE device
            for (UTShadowDevice* device : shadowDevices) {
                if (isPEShadowDeviceIdentifier(device->identifier())) {
                    selectedIdentifier = device->identifier();
                    break;
                }
            }
        }

        _pa22x_client.setSelectedDevice(selectedIdentifier);

        if (i == 0)
            UpdatePAParametersUI();
        else
            UpdatePEParametersUI();

    });

    connect(_ui->pa_group1_image_view, &ImageView::toggled, this, [=]() {
        _pa22x_client.selectedDevice()->setSelectedGroup(0);
        UpdatePAParametersUI();
    });
    connect(_ui->pa_group2_image_view, &ImageView::toggled, this, [=]() {
        _pa22x_client.selectedDevice()->setSelectedGroup(1);
        UpdatePAParametersUI();
    });
    connect(_ui->pa_group3_image_view, &ImageView::toggled, this, [=]() {
        _pa22x_client.selectedDevice()->setSelectedGroup(2);
        UpdatePAParametersUI();
    });
    connect(_ui->pa_group4_image_view, &ImageView::toggled, this, [=]() {
        _pa22x_client.selectedDevice()->setSelectedGroup(3);
        UpdatePAParametersUI();
    });

    for (UTShadowDevice* device : _pa22x_client.shadowDevices()) {
        if(isPEShadowDeviceIdentifier(device->identifier()))
            _ui->pe_devices_selector->addItem(device->identifier());
        else if (isPAShadowDeviceIdentifier(device->identifier()))
            _ui->pa_devices_selector->addItem(device->identifier());

        if (_ui->pe_devices_selector->count())
            _ui->pe_devices_selector->setCurrentIndex(0);

        if (_ui->pa_devices_selector->count())
            _ui->pa_devices_selector->setCurrentIndex(0);
    }

    connect(_ui->pe_devices_selector, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        QString selectedIdentifier = _ui->pe_devices_selector->currentText();
        _pa22x_client.setSelectedDevice(selectedIdentifier);
        UpdatePEParametersUI();
    });

    connect(_ui->pa_devices_selector, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        QString selectedIdentifier = _ui->pa_devices_selector->currentText();
        _pa22x_client.setSelectedDevice(selectedIdentifier);
    });

    _pe_channel_select_group = new QButtonGroup(this);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_1 ,1);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_2 ,2);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_3 ,3);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_4 ,4);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_5 ,5);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_6 ,6);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_7 ,7);
    _pe_channel_select_group->addButton(_ui->pe_channel_select_8 ,8);


    UTShadowDevice* device = _pa22x_client.selectedDevice();
    if (isPAShadowDeviceIdentifier(device->identifier())) {
        _ui->scan_tabs->setCurrentIndex(0);
    }
    else if (isPEShadowDeviceIdentifier(device->identifier())) {
        _ui->scan_tabs->setCurrentIndex(1);
        for (int i = 0; i < _ui->pe_devices_selector->count(); i ++) {
            if (_ui->pe_devices_selector->itemText(i) == device->identifier())
                _ui->pe_devices_selector->setCurrentIndex(i);
        }
    }

    connect(_ui->pe_channel_select_1, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            _pa22x_client.selectedDevice()->setSelectedChannel(0);
            UpdatePEParametersUI();
    });

    connect(_ui->pe_channel_select_2, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            _pa22x_client.selectedDevice()->setSelectedChannel(1);
            UpdatePEParametersUI();
    });

    connect(_ui->pe_channel_select_3, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            _pa22x_client.selectedDevice()->setSelectedChannel(2);
             UpdatePEParametersUI();
    });

    connect(_ui->pe_channel_select_4, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            _pa22x_client.selectedDevice()->setSelectedChannel(3);
            UpdatePEParametersUI();
    });

    connect(_ui->pe_channel_select_5, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            _pa22x_client.selectedDevice()->setSelectedChannel(4);
            UpdatePEParametersUI();
    });

    connect(_ui->pe_channel_select_6, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            _pa22x_client.selectedDevice()->setSelectedChannel(5);
            UpdatePEParametersUI();
    });

    connect(_ui->pe_channel_select_7, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            _pa22x_client.selectedDevice()->setSelectedChannel(6);
            UpdatePEParametersUI();
    });

    connect(_ui->pe_channel_select_8, &QCheckBox::toggled, [=](bool checked) {
        if (checked)
            _pa22x_client.selectedDevice()->setSelectedChannel(7);
            UpdatePEParametersUI();
    });

    // PA Setting again input init
    _ui->pa_setting_again_input_step->addItem("0.1",    QVariant(0.1));
    _ui->pa_setting_again_input_step->addItem("1", 		QVariant(1));
    _ui->pa_setting_again_input_step->addItem("2",		QVariant(2));
    _ui->pa_setting_again_input_step->addItem("6",		QVariant(6));
    _ui->pa_setting_again_input_step->setCurrentIndex(2);
    connect(_ui->pa_setting_again_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pa_setting_again_input->setSingleStep(_ui->pa_setting_again_input_step->currentData().toDouble());
    });
    _ui->pa_setting_again_input->setSingleStep(_ui->pa_setting_again_input_step->currentData().toDouble());
    _ui->pa_setting_again_input->setMinimum(0.0f);
    _ui->pa_setting_again_input->setMaximum(50.5f);
    connect(_ui->pa_setting_again_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("again", QString::number(_ui->pa_setting_again_input->text().toDouble()));
    });
    _ui->pa_setting_again_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting dgain input init
    _ui->pa_setting_dgain_input_step->addItem("0.1",	QVariant(0.1));
    _ui->pa_setting_dgain_input_step->addItem("1", 		QVariant(1));
    _ui->pa_setting_dgain_input_step->addItem("2",		QVariant(2));
    _ui->pa_setting_dgain_input_step->addItem("6",		QVariant(6));
    _ui->pa_setting_dgain_input_step->setCurrentIndex(2);
    connect(_ui->pa_setting_dgain_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pa_setting_dgain_input->setSingleStep(_ui->pa_setting_dgain_input_step->currentData().toDouble());
    });
    _ui->pa_setting_dgain_input->setSingleStep(_ui->pa_setting_dgain_input_step->currentData().toDouble());
    _ui->pa_setting_dgain_input->setMinimum(0.0f);
    _ui->pa_setting_dgain_input->setMaximum(48.0f);
    connect(_ui->pa_setting_dgain_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("dgain", QString::number(_ui->pa_setting_dgain_input->text().toDouble()));
    });
    _ui->pa_setting_dgain_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting voltage input init
    _ui->pa_setting_voltage_input->setSingleStep(5);
    _ui->pa_setting_voltage_input->setMinimum(0);
    _ui->pa_setting_voltage_input->setMaximum(95);
    connect(_ui->pa_setting_voltage_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("high_voltage", QString::number(_ui->pa_setting_voltage_input->text().toDouble()));
    });
    _ui->pa_setting_voltage_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting pulse_width input init
    _ui->pa_setting_pulse_width_input_step->addItem("1", 		QVariant(1));
    _ui->pa_setting_pulse_width_input_step->addItem("5",		QVariant(5));
    _ui->pa_setting_pulse_width_input_step->addItem("10",		QVariant(10));
    _ui->pa_setting_pulse_width_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_pulse_width_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pa_setting_pulse_width_input->setSingleStep(_ui->pa_setting_pulse_width_input_step->currentData().toInt());
    });
    _ui->pa_setting_pulse_width_input->setSingleStep(_ui->pa_setting_pulse_width_input_step->currentData().toInt());
    _ui->pa_setting_pulse_width_input->setMinimum(25);
    _ui->pa_setting_pulse_width_input->setMaximum(600);
    connect(_ui->pa_setting_pulse_width_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("pulse_width", QString::number(_ui->pa_setting_pulse_width_input->text().toDouble()));
    });
    _ui->pa_setting_pulse_width_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting prf input init
    _ui->pa_setting_prf_input_step->addItem("10",	QVariant(10));
    _ui->pa_setting_prf_input_step->addItem("50",	QVariant(50));
    _ui->pa_setting_prf_input_step->addItem("100",	QVariant(100));
    _ui->pa_setting_prf_input_step->addItem("200",	QVariant(200));
    _ui->pa_setting_prf_input_step->setCurrentIndex(3);
    connect(_ui->pa_setting_prf_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i){
        (void)i;
        _ui->pa_setting_prf_input->setSingleStep(_ui->pa_setting_prf_input_step->currentData().toInt());
    });
    _ui->pa_setting_prf_input->setSingleStep(_ui->pa_setting_prf_input_step->currentData().toInt());
    _ui->pa_setting_prf_input->setMinimum(25);
    _ui->pa_setting_prf_input->setMaximum(10000);
    connect(_ui->pa_setting_prf_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i){
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("prf", QString::number(_ui->pa_setting_prf_input->text().toDouble()));
    });
    _ui->pa_setting_prf_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting range input init
    _ui->pa_setting_range_input_step->addItem("1", 		QVariant(1));
    _ui->pa_setting_range_input_step->addItem("10",		QVariant(10));
    _ui->pa_setting_range_input_step->addItem("100",	QVariant(100));
    _ui->pa_setting_range_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_range_input_step,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
                _ui->pa_setting_range_input->setSingleStep(_ui->pa_setting_range_input_step->currentData().toDouble());
    });
    _ui->pa_setting_range_input->setSingleStep(_ui->pa_setting_range_input_step->currentData().toDouble());
    _ui->pa_setting_range_input->setMinimum(0.0f);
    _ui->pa_setting_range_input->setMaximum(10000.0f);
    connect(_ui->pa_setting_range_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("range", QString::number(_ui->pa_setting_range_input->text().toDouble()));
    });
    _ui->pa_setting_range_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting filter input init
    _ui->pa_setting_filter_input->addItem("0.5M - 10M",	QVariant(kFilter0));
    _ui->pa_setting_filter_input->addItem("  4M - 12M",	QVariant(kFilter1));
    _ui->pa_setting_filter_input->addItem("  6M - 20M",	QVariant(kFilter2));
    _ui->pa_setting_filter_input->setCurrentIndex(1);
    connect(_ui->pa_setting_filter_input, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("filter", QString::number(_ui->pa_setting_filter_input->currentData().toInt()));
    });

    // PA Setting gate a pos input init
    _ui->pa_setting_gate_a_pos_input_step->addItem("0.1",	QVariant(0.1));
    _ui->pa_setting_gate_a_pos_input_step->addItem("1", 	QVariant(1));
    _ui->pa_setting_gate_a_pos_input_step->addItem("5",		QVariant(5));
    _ui->pa_setting_gate_a_pos_input_step->addItem("10",	QVariant(10));
    _ui->pa_setting_gate_a_pos_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_a_pos_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pa_setting_gate_a_pos_input->setSingleStep(_ui->pa_setting_gate_a_pos_input_step->currentData().toDouble());
    });
    _ui->pa_setting_gate_a_pos_input->setSingleStep(_ui->pa_setting_gate_a_pos_input_step->currentData().toDouble());
    _ui->pa_setting_gate_a_pos_input->setMinimum(0.0f);
    _ui->pa_setting_gate_a_pos_input->setMaximum(10000.0f);
    connect(_ui->pa_setting_gate_a_pos_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("gate_a_pos", QString::number(_ui->pa_setting_gate_a_pos_input->text().toDouble()));

    });
    _ui->pa_setting_gate_a_pos_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting gate a width input init
    _ui->pa_setting_gate_a_width_input_step->addItem("0.1",	QVariant(0.1));
    _ui->pa_setting_gate_a_width_input_step->addItem("1", 	QVariant(1));
    _ui->pa_setting_gate_a_width_input_step->addItem("5",	QVariant(5));
    _ui->pa_setting_gate_a_width_input_step->addItem("10",	QVariant(10));
    _ui->pa_setting_gate_a_width_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_a_width_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pa_setting_gate_a_width_input->setSingleStep(_ui->pa_setting_gate_a_width_input_step->currentData().toDouble());
    });
    _ui->pa_setting_gate_a_width_input->setSingleStep(_ui->pa_setting_gate_a_width_input_step->currentData().toDouble());
    _ui->pa_setting_gate_a_width_input->setMinimum(1.0f);
    _ui->pa_setting_gate_a_width_input->setMaximum(10000.0f);
    connect(_ui->pa_setting_gate_a_width_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("gate_a_width", QString::number(_ui->pa_setting_gate_a_width_input->text().toDouble()));
    });
    _ui->pa_setting_gate_a_width_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting gate a height input init
    _ui->pa_setting_gate_a_height_input_step->addItem("1", 		QVariant(1));
    _ui->pa_setting_gate_a_height_input_step->addItem("5",		QVariant(5));
    _ui->pa_setting_gate_a_height_input_step->addItem("10",		QVariant(10));
    _ui->pa_setting_gate_a_width_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_a_height_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pa_setting_gate_a_height_input->setSingleStep(_ui->pa_setting_gate_a_height_input_step->currentData().toInt());
    });
    _ui->pa_setting_gate_a_height_input->setSingleStep(_ui->pa_setting_gate_a_height_input_step->currentData().toInt());
    _ui->pa_setting_gate_a_height_input->setMinimum(0);
    _ui->pa_setting_gate_a_height_input->setMaximum(100);
    connect(_ui->pa_setting_gate_a_height_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("gate_a_height", QString::number(_ui->pa_setting_gate_a_height_input->text().toDouble()));
    });
    _ui->pa_setting_gate_a_height_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting gate b pos input init
    _ui->pa_setting_gate_b_pos_input_step->addItem("0.1",	QVariant(0.1));
    _ui->pa_setting_gate_b_pos_input_step->addItem("1", 	QVariant(1));
    _ui->pa_setting_gate_b_pos_input_step->addItem("5",		QVariant(5));
    _ui->pa_setting_gate_b_pos_input_step->addItem("10",	QVariant(10));
    _ui->pa_setting_gate_b_pos_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_b_pos_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pa_setting_gate_b_pos_input->setSingleStep(_ui->pa_setting_gate_b_pos_input_step->currentData().toDouble());
    });
    _ui->pa_setting_gate_b_pos_input->setSingleStep(_ui->pa_setting_gate_b_pos_input_step->currentData().toDouble());
    _ui->pa_setting_gate_b_pos_input->setMinimum(0.0f);
    _ui->pa_setting_gate_b_pos_input->setMaximum(10000.0f);
    connect(_ui->pa_setting_gate_b_pos_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("gate_b_pos", QString::number(_ui->pa_setting_gate_b_pos_input->text().toDouble()));
    });
    _ui->pa_setting_gate_b_pos_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting gate b width input init
    _ui->pa_setting_gate_b_width_input_step->addItem("0.1",	QVariant(0.1));
    _ui->pa_setting_gate_b_width_input_step->addItem("1", 	QVariant(1));
    _ui->pa_setting_gate_b_width_input_step->addItem("5",	QVariant(5));
    _ui->pa_setting_gate_b_width_input_step->addItem("10",	QVariant(10));
    _ui->pa_setting_gate_b_width_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_b_width_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pa_setting_gate_b_width_input->setSingleStep(_ui->pa_setting_gate_b_width_input_step->currentData().toDouble());
    });
    _ui->pa_setting_gate_b_width_input->setSingleStep(_ui->pa_setting_gate_b_width_input_step->currentData().toDouble());
    _ui->pa_setting_gate_b_width_input->setMinimum(0.0f);
    _ui->pa_setting_gate_b_width_input->setMaximum(10000.0f);
    connect(_ui->pa_setting_gate_b_width_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("gate_b_width", QString::number(_ui->pa_setting_gate_b_width_input->text().toDouble()));
    });
    _ui->pa_setting_gate_b_width_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Setting gate b height input init
    _ui->pa_setting_gate_b_height_input_step->addItem("1", 	QVariant(1));
    _ui->pa_setting_gate_b_height_input_step->addItem("5",	QVariant(5));
    _ui->pa_setting_gate_b_height_input_step->addItem("10",	QVariant(10));
    _ui->pa_setting_gate_b_height_input_step->setCurrentIndex(1);
    connect(_ui->pa_setting_gate_b_height_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pa_setting_gate_b_height_input->setSingleStep(_ui->pa_setting_gate_b_height_input_step->currentData().toInt());
    });
    _ui->pa_setting_gate_b_height_input->setSingleStep(_ui->pa_setting_gate_b_height_input_step->currentData().toInt());
    _ui->pa_setting_gate_b_height_input->setMinimum(0);
    _ui->pa_setting_gate_b_height_input->setMaximum(100);
    connect(_ui->pa_setting_gate_b_height_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("gate_b_height", QString::number(_ui->pa_setting_gate_b_height_input->text().toDouble()));
    });
    _ui->pa_setting_gate_b_height_input->findChild<QLineEdit*>()->setReadOnly(true);

    _ui->pa_setting_a_wave_index_input->setMinimum(1);
    _ui->pa_setting_a_wave_index_input->setMaximum(128);
    connect(_ui->pa_setting_a_wave_index_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("beam_index", QString::number(i));

    });
    _ui->pa_setting_a_wave_index_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PA Scanrule setting scan type input init
    _ui->pa_scan_rule_type_input->addItem("s");
    _ui->pa_scan_rule_type_input->addItem("l");
    _ui->pa_scan_rule_type_input->setCurrentIndex(0);
    // PA Scanrule setting wedge mode input init
    _ui->pa_scan_rule_wedge_mode_input->addItem("enable");
    _ui->pa_scan_rule_wedge_mode_input->addItem("disable");
    _ui->pa_scan_rule_wedge_mode_input->setCurrentIndex(0);
    // PA Scanrule setting wedge type input init
    _ui->pa_scan_rule_wedge_type_input->addItem("l");
    _ui->pa_scan_rule_wedge_type_input->addItem("s");
    _ui->pa_scan_rule_wedge_type_input->setCurrentIndex(0);

    // PE Setting gain input init
    _ui->pe_setting_gain_input_step->addItem("0.1",		QVariant(0.1));
    _ui->pe_setting_gain_input_step->addItem("1", 		QVariant(1));
    _ui->pe_setting_gain_input_step->addItem("2",		QVariant(2));
    _ui->pe_setting_gain_input_step->addItem("6",		QVariant(6));
    _ui->pe_setting_gain_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_gain_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pe_setting_gain_input->setSingleStep(_ui->pe_setting_gain_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gain_input->setSingleStep(_ui->pe_setting_gain_input_step->currentData().toDouble());
    _ui->pe_setting_gain_input->setMinimum(0.0f);
    _ui->pe_setting_gain_input->setMaximum(110.0f);
    connect(_ui->pe_setting_gain_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("gain", _ui->pe_setting_gain_input->text());
    });
    _ui->pe_setting_gain_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting voltage input init
    _ui->pe_setting_voltage_input->setSingleStep(25);
    _ui->pe_setting_voltage_input->setMinimum(-350);
    _ui->pe_setting_voltage_input->setMaximum(-50);
    connect(_ui->pe_setting_voltage_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("high_voltage", _ui->pe_setting_voltage_input->text());
    });
    _ui->pe_setting_voltage_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting pulse width input init
    _ui->pe_setting_pulse_width_input_step->addItem("1",    QVariant(1));
    _ui->pe_setting_pulse_width_input_step->addItem("5",	QVariant(5));
    _ui->pe_setting_pulse_width_input_step->addItem("10",	QVariant(10));
    connect(_ui->pe_setting_pulse_width_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pe_setting_pulse_width_input->setSingleStep(_ui->pe_setting_pulse_width_input_step->currentData().toInt());
    });
    _ui->pe_setting_pulse_width_input->setSingleStep(_ui->pe_setting_pulse_width_input_step->currentData().toInt());
    _ui->pe_setting_pulse_width_input->setMinimum(25);
    _ui->pe_setting_pulse_width_input->setMaximum(600);
    connect(_ui->pe_setting_pulse_width_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("pulse_width", _ui->pe_setting_pulse_width_input->text());
    });
    _ui->pe_setting_pulse_width_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting prf input init
    _ui->pe_setting_prf_input_step->addItem("10",	QVariant(10));
    _ui->pe_setting_prf_input_step->addItem("50",	QVariant(50));
    _ui->pe_setting_prf_input_step->addItem("100",	QVariant(100));
    _ui->pe_setting_prf_input_step->addItem("200",	QVariant(200));
    connect(_ui->pe_setting_prf_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i){
        (void)i;
        _ui->pe_setting_prf_input->setSingleStep(_ui->pe_setting_prf_input_step->currentData().toInt());
    });
    _ui->pe_setting_prf_input->setSingleStep(_ui->pe_setting_prf_input_step->currentData().toInt());
    _ui->pe_setting_prf_input->setMinimum(25);
    _ui->pe_setting_prf_input->setMaximum(10000);
    connect(_ui->pe_setting_prf_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i){
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("prf", _ui->pe_setting_prf_input->text());
    });
    _ui->pe_setting_prf_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting range input init
    _ui->pe_setting_range_input_step->addItem("1", 		QVariant(1));
    _ui->pe_setting_range_input_step->addItem("10",		QVariant(10));
    _ui->pe_setting_range_input_step->addItem("100",	QVariant(100));
    _ui->pe_setting_range_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_range_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pe_setting_range_input->setSingleStep(_ui->pe_setting_range_input_step->currentData().toDouble());
    });
    _ui->pe_setting_range_input->setSingleStep(_ui->pe_setting_range_input_step->currentData().toDouble());
    _ui->pe_setting_range_input->setMinimum(0.0f);
    _ui->pe_setting_range_input->setMaximum(10000.0f);
    connect(_ui->pe_setting_range_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("range", _ui->pe_setting_range_input->text());
    });
    _ui->pe_setting_range_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting filter input init
    _ui->pe_setting_filter_input->addItem("  2M - 3M",	QVariant(kFilter0));
    _ui->pe_setting_filter_input->addItem("0.5M - 15M",	QVariant(kFilter1));
    _ui->pe_setting_filter_input->setCurrentIndex(0);
    connect(_ui->pe_setting_filter_input, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("filter", QString::number(_ui->pe_setting_filter_input->currentData().toInt()));
    });

    // PE Setting delay input init
    _ui->pe_setting_delay_input_step->addItem("1", 		QVariant(1));
    _ui->pe_setting_delay_input_step->addItem("5",		QVariant(5));
    _ui->pe_setting_delay_input_step->addItem("10",		QVariant(10));
    connect(_ui->pe_setting_delay_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pe_setting_delay_input->setSingleStep(_ui->pe_setting_delay_input_step->currentData().toDouble());
    });
    _ui->pe_setting_delay_input->setSingleStep(_ui->pe_setting_delay_input_step->currentData().toDouble());
    _ui->pe_setting_delay_input->setMinimum(0.0f);
    _ui->pe_setting_delay_input->setMaximum(1000000.0f);
    connect(_ui->pe_setting_delay_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("delay", QString::number(_ui->pe_setting_delay_input->text().toDouble()));
    });
    _ui->pe_setting_delay_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting origin input init
    _ui->pe_setting_origin_input_step->addItem("1", 	QVariant(1));
    _ui->pe_setting_origin_input_step->addItem("5",		QVariant(5));
    _ui->pe_setting_origin_input_step->addItem("10",	QVariant(10));
    connect(_ui->pe_setting_origin_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pe_setting_origin_input->setSingleStep(_ui->pe_setting_origin_input_step->currentData().toDouble());
    });
    _ui->pe_setting_origin_input->setSingleStep(_ui->pe_setting_origin_input_step->currentData().toDouble());
    _ui->pe_setting_origin_input->setMinimum(0.0f);
    _ui->pe_setting_origin_input->setMaximum(1000000.0f);
    connect(_ui->pe_setting_origin_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("origin", QString::number(_ui->pe_setting_origin_input->text().toDouble()));
    });
    _ui->pe_setting_origin_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting dual mode input init
    _ui->pe_setting_dual_mode_input->addItem("dual");
    _ui->pe_setting_dual_mode_input->addItem("single");
    connect(_ui->pe_setting_dual_mode_input, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        if (_ui->pe_setting_dual_mode_input->currentText() == "single")
            _pa22x_client.selectedDevice()->setParameter("dual_mode", "false");
        else if (_ui->pe_setting_dual_mode_input->currentText() == "dual")
            _pa22x_client.selectedDevice()->setParameter("dual_mode", "true");
    });

    // PE Setting damping input init
    _ui->pe_setting_damping_input->addItem("80");
    _ui->pe_setting_damping_input->addItem("400");
    connect(_ui->pe_setting_damping_input, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("damping", _ui->pe_setting_damping_input->currentText());
    });

    // PE Setting velocity input init
    _ui->pe_setting_velocity_input_step->addItem("1", 		QVariant(1));
    _ui->pe_setting_velocity_input_step->addItem("10",		QVariant(10));
    _ui->pe_setting_velocity_input_step->addItem("100",		QVariant(100));
    _ui->pe_setting_velocity_input_step->addItem("1000",	QVariant(1000));
    connect(_ui->pe_setting_velocity_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pe_setting_velocity_input->setSingleStep(_ui->pe_setting_velocity_input_step->currentData().toInt());
    });
    _ui->pe_setting_velocity_input->setSingleStep(_ui->pe_setting_velocity_input_step->currentData().toInt());
    _ui->pe_setting_velocity_input->setMinimum(500);
    _ui->pe_setting_velocity_input->setMaximum(15000);
    connect(_ui->pe_setting_velocity_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("velocity", QString::number(_ui->pe_setting_velocity_input->text().toInt()));
    });
    _ui->pe_setting_velocity_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting compress input init
    _ui->pe_setting_compress_rate_input_step->addItem("1", 		QVariant(1));
    _ui->pe_setting_compress_rate_input_step->addItem("10",		QVariant(10));
    _ui->pe_setting_compress_rate_input_step->addItem("100",    QVariant(100));
    connect(_ui->pe_setting_compress_rate_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pe_setting_compress_rate_input->setSingleStep(_ui->pe_setting_compress_rate_input_step->currentData().toInt());
    });
    _ui->pe_setting_compress_rate_input->setSingleStep(_ui->pe_setting_compress_rate_input_step->currentData().toInt());
    _ui->pe_setting_compress_rate_input->setMinimum(1);
    _ui->pe_setting_compress_rate_input->setMaximum(200);
    connect(_ui->pe_setting_compress_rate_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("compress_rate", QString::number(_ui->pe_setting_compress_rate_input->text().toInt()));
    });
    _ui->pe_setting_compress_rate_input->findChild<QLineEdit*>()->setReadOnly(true);
    // PE Setting compress mode init
    _ui->pe_setting_compress_mode->addItem("average");
    _ui->pe_setting_compress_mode->addItem("peak");
    connect(_ui->pe_setting_compress_mode, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _pa22x_client.selectedDevice()->setParameter("compress_mode", _ui->pe_setting_compress_mode->currentText());
    });

    // PE Setting gate select
    _ui->pe_setting_gate_select_input->addItem(MainWindow::_gate_a_label);
    _ui->pe_setting_gate_select_input->addItem(MainWindow::_gate_b_label);
    _ui->pe_setting_gate_select_input->addItem(MainWindow::_gate_c_label);
    _ui->pe_setting_gate_select_input->addItem(MainWindow::_gate_d_label);
    connect(_ui->pe_setting_gate_select_input, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        UpdatePEParametersUI();
    });

    // PE Setting gate pos input init
    _ui->pe_setting_gate_pos_input_step->addItem("0.1",	QVariant(0.1));
    _ui->pe_setting_gate_pos_input_step->addItem("1", 	QVariant(1));
    _ui->pe_setting_gate_pos_input_step->addItem("5",	QVariant(5));
    _ui->pe_setting_gate_pos_input_step->addItem("10",	QVariant(10));
    _ui->pe_setting_gate_pos_input_step->setCurrentIndex(2);
    connect(_ui->pe_setting_gate_pos_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pe_setting_gate_pos_input->setSingleStep(_ui->pe_setting_gate_pos_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gate_pos_input->setSingleStep(_ui->pe_setting_gate_pos_input_step->currentData().toDouble());
    _ui->pe_setting_gate_pos_input->setMinimum(0.0f);
    _ui->pe_setting_gate_pos_input->setMaximum(10000.0f);
    connect(_ui->pe_setting_gate_pos_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        QString gate_pos = "";
        if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_a_label)
            gate_pos = "gate_a_pos";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_b_label)
            gate_pos = "gate_b_pos";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_c_label)
            gate_pos = "gate_c_pos";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_d_label)
            gate_pos = "gate_d_pos";

        _pa22x_client.selectedDevice()->setParameter(gate_pos, QString::number(_ui->pe_setting_gate_pos_input->text().toDouble()));
    });
    _ui->pe_setting_gate_pos_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting gate width input init
    _ui->pe_setting_gate_width_input_step->addItem("0.1",	QVariant(0.1));
    _ui->pe_setting_gate_width_input_step->addItem("1", 	QVariant(1));
    _ui->pe_setting_gate_width_input_step->addItem("5",     QVariant(5));
    _ui->pe_setting_gate_width_input_step->addItem("10",	QVariant(10));
    _ui->pe_setting_gate_width_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_gate_width_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pe_setting_gate_width_input->setSingleStep(_ui->pe_setting_gate_width_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gate_width_input->setSingleStep(_ui->pe_setting_gate_width_input_step->currentData().toDouble());
    _ui->pe_setting_gate_width_input->setMinimum(0.0f);
    _ui->pe_setting_gate_width_input->setMaximum(100.0f);
    connect(_ui->pe_setting_gate_width_input, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double i) {
        (void)i;
        QString gate_width = "";
        if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_a_label)
            gate_width = "gate_a_width";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_b_label)
            gate_width = "gate_b_width";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_c_label)
            gate_width = "gate_c_width";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_d_label)
            gate_width = "gate_d_width";

        _pa22x_client.selectedDevice()->setParameter(gate_width, QString::number(_ui->pe_setting_gate_width_input->text().toDouble()));
    });
    _ui->pe_setting_gate_width_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting gate height input init
    _ui->pe_setting_gate_height_input_step->addItem("1", 		QVariant(1));
    _ui->pe_setting_gate_height_input_step->addItem("5",		QVariant(5));
    _ui->pe_setting_gate_height_input_step->addItem("10",		QVariant(10));
    _ui->pe_setting_gate_height_input_step->setCurrentIndex(1);
    connect(_ui->pe_setting_gate_height_input_step, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        (void)i;
        _ui->pe_setting_gate_height_input->setSingleStep(_ui->pe_setting_gate_height_input_step->currentData().toDouble());
    });
    _ui->pe_setting_gate_height_input->setSingleStep(_ui->pe_setting_gate_height_input_step->currentData().toDouble());
    _ui->pe_setting_gate_height_input->setMinimum(0);
    _ui->pe_setting_gate_height_input->setMaximum(100);
    connect(_ui->pe_setting_gate_height_input, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        (void)i;
        QString gate_height = "";
        if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_a_label)
            gate_height = "gate_a_height";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_b_label)
            gate_height = "gate_b_height";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_c_label)
            gate_height = "gate_c_height";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_d_label)
            gate_height = "gate_d_height";
        _pa22x_client.selectedDevice()->setParameter(gate_height, QString::number(_ui->pe_setting_gate_height_input->text().toDouble()));
    });
    _ui->pe_setting_gate_height_input->findChild<QLineEdit*>()->setReadOnly(true);

    // PE Setting gate tracing
    connect(_ui->pe_setting_gate_tracing_input, &QCheckBox::toggled, [=](bool checked) {
        QString gate_tracing = "";
        if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_a_label)
            return;
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_b_label)
            gate_tracing = "gate_b_tracing";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_c_label)
            gate_tracing = "gate_c_tracing";
        else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_d_label)
            gate_tracing = "gate_d_tracing";

        QString value = "false";
        if (checked)
            value = "a";
        else
            value = "false";

        _pa22x_client.selectedDevice()->setParameter(gate_tracing, value);
    });

    // PE Setting rectification mode
    _ui->pe_setting_rectification_input->addItem("full");
    _ui->pe_setting_rectification_input->addItem("positive");
    _ui->pe_setting_rectification_input->addItem("negative");
    _ui->pe_setting_rectification_input->addItem("rf");
    connect(_ui->pe_setting_rectification_input, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
        if (_ui->pe_setting_rectification_input->currentText() == "full")
            _pa22x_client.selectedDevice()->setParameter("rectification", "full");
        else if (_ui->pe_setting_rectification_input->currentText() == "positive")
            _pa22x_client.selectedDevice()->setParameter("rectification", "positive");
        else if (_ui->pe_setting_rectification_input->currentText() == "negative")
            _pa22x_client.selectedDevice()->setParameter("rectification", "negative");
        else if (_ui->pe_setting_rectification_input->currentText() == "rf")
            _pa22x_client.selectedDevice()->setParameter("rectification", "rf");
    });

    _ui->pa22x_client_responder_list_view->setModel(&_pa22x_client_responder_list);
    connect(_ui->pa22x_client_command_input, &QLineEdit::editingFinished, [this] {
        auto commandString = _ui->pa22x_client_command_input->text();
        QString response;
        if (!commandString.isEmpty()) {
            _pa22x_client.getShadowCommander()->sendCommandSync(commandString, response);
            int count = _pa22x_client_responder_list.rowCount();
            _pa22x_client_responder_list.insertRow(count);
            _pa22x_client_responder_list.setData(_pa22x_client_responder_list.index(count), response);
            _ui->pa22x_client_responder_list_view->setCurrentIndex(_pa22x_client_responder_list.index(count));
        }
        _ui->pa22x_client_command_input->clear();
    });

    auto responder_list_add_item = [this](QString str) {
        int count = _pa22x_client_responder_list.rowCount();
        _pa22x_client_responder_list.insertRow(count);
        _pa22x_client_responder_list.setData(_pa22x_client_responder_list.index(count), str);
        _ui->pa22x_client_responder_list_view->setCurrentIndex(_pa22x_client_responder_list.index(count));
    };
    connect(_pa22x_client.getShadowCommander(), &ShadowCommander::responseGot, responder_list_add_item);
    connect(_pa22x_client.getShadowCommander(), &ShadowCommander::commandSent, responder_list_add_item);

    connect(_ui->pa_scan_rule_setup_button, SIGNAL(released()), this, SLOT(SetupPAScanrule()));
    connect(_ui->pa22x_client_load_button, SIGNAL(released()),	this, SLOT(LoadConfigFile()));

    UpdatePAParametersUI();
    UpdatePEParametersUI();
}

void MainWindow::ClientConnectToServer()
{
    UIDisable();
    QString serverIP = _ui->ip_address_input->text();
    if (_pa22x_client.start(serverIP)) {
        connect(_pa22x_client.getShadowCommander(), SIGNAL(disconnect()), this, SLOT(ClientDisconnectToServer()));
        _ui->view_pages->setCurrentWidget(_ui->scan_page);
        _ui->scan_tabs->setCurrentWidget(_ui->pa_tab);
        if (!_ui_setup)
            UISetup();
        _ui_setup = true;
       _displayTimer = startTimer(20); // 50fpsd
    }
    UIEnable();
}

void MainWindow::ClientDisconnectToServer()
{
    _ui->view_pages->setCurrentWidget(_ui->server_connect_page);
    if (_displayTimer) {
        killTimer(_displayTimer);
        _displayTimer = 0;
    }
    _pa22x_client.stop();
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    if (_displayTimer) {
        killTimer(_displayTimer);
        _displayTimer = 0;
    }

    _pa22x_client.stop();
}

void MainWindow::LoadConfigFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                            "Select a file", "", nullptr, nullptr,
                            QFileDialog::Option::DontUseNativeDialog);
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            while (!file.atEnd()) {
                auto line = file.readLine();
                line = line.simplified();
                if (!line.isEmpty() && line[0] != '#') {
                    _pa22x_client.getShadowCommander()->sendCommandAsync(line);
                }
            }
        }
    }
    _pa22x_client.sync();
    UpdatePAParametersUI();
}

void MainWindow::timerEvent(QTimerEvent * e)
{
    if (e->timerId() == _displayTimer) {
        PresentClientData();
    }
}

#define delayOffsetMM(ns, v)  Math::NStoMM(ns * 1000, v, 1);
#define sppe2mm(sp, v)   ((sp) * 10.0 * (v) / 1000000.0)
#define sppa2mm(sp, v)   ((sp) * 16.0 * (v) / 1000000.0)
void MainWindow::PresentClientData()
{
    // PA Scan display
    if (_ui->scan_tabs->currentIndex() == 0) {

        UTShadowDevice* device = _pa22x_client.selectedDevice();
        if (!device)
            return ;

        while(_pa22x_client.readFromDataSocket() == true)
            /*do nothing*/;

        _ui->pa_active_group_index_label->setText(QString("Group: ") +
                                                  QString::number(device->selectedGroup()+1));


        QString rectification = "";
        _ui->pa_group_active_image_view->setData(device->getImage());
        _ui->pa_group_active_image_view->draw(kImageViewDrawHorizontal, rectification);
        _ui->pa_group1_image_view->setData(device->getImage(0));
        _ui->pa_group1_image_view->draw(kImageViewDrawHorizontal, rectification);
        _ui->pa_group2_image_view->setData(device->getImage(1));
        _ui->pa_group2_image_view->draw(kImageViewDrawHorizontal, rectification);
        _ui->pa_group3_image_view->setData(device->getImage(2));
        _ui->pa_group3_image_view->draw(kImageViewDrawHorizontal, rectification);
        _ui->pa_group4_image_view->setData(device->getImage(3));
        _ui->pa_group4_image_view->draw(kImageViewDrawHorizontal, rectification);

        QString selectedBeam;
        QString gate_a_pos;
        QString gate_a_width;
        QString gate_a_height;
        QString gate_b_pos;
        QString gate_b_width;
        QString gate_b_height;
        QString range;

        device->getParameter("beam_index", selectedBeam);
        device->getParameter("gate_a_pos", gate_a_pos);
        device->getParameter("gate_a_width", gate_a_width);
        device->getParameter("gate_a_height", gate_a_height);
        device->getParameter("gate_b_pos", gate_b_pos);
        device->getParameter("gate_b_width", gate_b_width);
        device->getParameter("gate_b_height", gate_b_height);
        device->getParameter("range", range);


        X22_Waveform* beam = device->getAWave();

        _ui->pa_a_wave_view->setData(beam);
        _ui->pa_a_wave_view->setGate(gate_a_pos.toDouble()* 400 / range.toDouble(),
                                      gate_a_width.toDouble() * 400 / range.toDouble(),
                                      gate_a_height.toDouble() * 2.5 + 0.5, "a");
        _ui->pa_a_wave_view->setGate(gate_b_pos.toDouble()* 400 / range.toDouble(),
                                      gate_b_width.toDouble() * 400 / range.toDouble(),
                                      gate_b_height.toDouble() * 2.5 + 0.5, "b");


        QHash<QString, QString> scan_rule_parameter;
        device->getScanRuleParameter(scan_rule_parameter);
        QString imge_span_start = scan_rule_parameter["imge_span_start"];
        QString imge_span_end = scan_rule_parameter["imge_span_end"];

        QString scan_rule = device->getScanRule();
        QStringList scan_rule_sections = scan_rule.split(' ', QString::SkipEmptyParts);
        //  ok scan_rule [1] [angle] [refract_point] ... [beam_count] [angle] [refract_point]
        const double image_range = imge_span_end.toDouble() - imge_span_start.toDouble();
        const int iamge_width = 512;
        const double mmppx = image_range / iamge_width;
        for (int i = 0; i < (scan_rule_sections.count() - 2) / 3; i++) {
            int beam_index = scan_rule_sections.value(2 + 3 * i).toInt() - 1;
            double angle = scan_rule_sections.value(2 + 3 * i + 1).toDouble();
            double pos = scan_rule_sections.value(2 + 3 * i + 2).toDouble();
            _pa_view_parameter[device->selectedGroup()].beam_vertexs[beam_index].x0  =
                    (pos - imge_span_start.toDouble()) / mmppx;
            _pa_view_parameter[device->selectedGroup()].beam_vertexs[beam_index].x1  =
                    _pa_view_parameter[device->selectedGroup()].beam_vertexs[beam_index].x0 +
                    std::sin(angle / 180.0 * 3.14) * 400;
            _pa_view_parameter[device->selectedGroup()].beam_vertexs[beam_index].y0  = 0;
            _pa_view_parameter[device->selectedGroup()].beam_vertexs[beam_index].y1  =
                    _pa_view_parameter[device->selectedGroup()].beam_vertexs[beam_index].y0 +
                    std::cos(angle / 180.0 * 3.14) * 400;
        }

        PABeamVertex* beam_vertexs = &_pa_view_parameter[device->selectedGroup()].beam_vertexs[selectedBeam.toInt()-1];
        //qDebug("%f %f %f %f", beam_vertexs->x0, beam_vertexs->y0,  beam_vertexs->x1, beam_vertexs->y1);
        _ui->pa_group_active_image_view->setBeamMark(beam_vertexs->x0, beam_vertexs->y0,  beam_vertexs->x1, beam_vertexs->y1);

        _ui->pa_a_wave_view->draw(kImageViewDrawVertical, rectification);


        _ui->pa_image_ruler_hor_view->setAnchorPoint(Qt::AnchorTop);
        _ui->pa_image_ruler_hor_view->setRange(imge_span_start.toDouble(), imge_span_end.toDouble());

        _ui->pa_image_ruler_ver_view->setAnchorPoint(Qt::AnchorLeft);
        _ui->pa_image_ruler_ver_view->setRange(0, range.toDouble());

        _ui->pa_a_wave_ruler_hor_view->setAnchorPoint(Qt::AnchorTop);
        _ui->pa_a_wave_ruler_ver_view->setAnchorPoint(Qt::AnchorLeft);
        _ui->pa_a_wave_ruler_ver_view->setRange(0, range.toDouble());

        _ui->pa_encoder_label->setText(QString("Encoder0: %1, %2    Encoder1: %3, %4").
                                    arg(QString::number(beam->enc[0].fwd),
                                        QString::number(beam->enc[0].rvs),
                                        QString::number(beam->enc[1].rvs),
                                        QString::number(beam->enc[1].rvs)));

        QString velocity;
        device->getParameter("velocity", velocity);

        double amp0 = beam->amp0 / 2.5;
        double path0 = sppa2mm(beam->path0, velocity.toDouble()) / 2.0;
        double amp1 = beam->amp1 / 2.5;
        double path1 = sppa2mm(beam->path1, velocity.toDouble()) / 2.0;

        _ui->pa_active_group_beam_gate_a_label->setText(QString("Gate A: %1 %  %2 mm").
                                                        arg(QString::number(amp0, 'f', 1)).
                                                        arg(QString::number(path0, 'f', 1)));

        _ui->pa_active_group_beam_gate_b_label->setText(QString("Gate B: %1 %  %2 mm").
                                                        arg(QString::number(amp1, 'f', 1)).
                                                        arg(QString::number(path1, 'f', 1)));
    }
    // PE Scan display
    else if (_ui->scan_tabs->currentIndex() == 1) {
        UTShadowDevice* device = _pa22x_client.selectedDevice();
        if (!device)
            return;

        while(_pa22x_client.readFromDataSocket() == true)
            /*do nothing*/;
        X22_Waveform* wave_form = device->getAWave();

        QString value;
        double range = 0.0;
        double delay_us = 0.0;
        int velocity = 0;
        double delay_mm = 0.0;

        device->getParameter("range", value);
        range = value.toDouble();
        device->getParameter("delay", value);
        delay_us = value.toDouble();
        device->getParameter("velocity", value);
        velocity = value.toInt();
        delay_mm = delayOffsetMM(delay_us, velocity);

        double tracing_origin = 0.0;

        double gate_a_pos = 0.0;
        double gate_a_width = 0.0;
        double gate_a_height = 0.0;
        int gate_a_pos_px = 0;
        int gate_a_width_px = 0;
        int gate_a_height_px = 0;

        double gate_b_pos = 0.0;
        double gate_b_width = 0.0;
        double gate_b_height = 0.0;
        int gate_b_pos_px = 0;
        int gate_b_width_px = 0;
        int gate_b_height_px = 0;

        double gate_c_pos = 0.0;
        double gate_c_width = 0.0;
        double gate_c_height = 0.0;
        int gate_c_pos_px = 0;
        int gate_c_width_px = 0;
        int gate_c_height_px = 0;

        double gate_d_pos = 0.0;
        double gate_d_width = 0.0;
        double gate_d_height = 0.0;
        int gate_d_pos_px = 0;
        int gate_d_width_px = 0;
        int gate_d_height_px = 0;

        QString gate_b_tracing = "";
        _pa22x_client.selectedDevice()->getParameter("gate_b_tracing", gate_b_tracing);
        QString gate_c_tracing = "";
        _pa22x_client.selectedDevice()->getParameter("gate_c_tracing", gate_c_tracing);
        QString gate_d_tracing = "";
        _pa22x_client.selectedDevice()->getParameter("gate_d_tracing", gate_d_tracing);

        double amp0 = wave_form->amp0 / 2.5;
        double path0 = sppe2mm(wave_form->path0, velocity) / 2.0;
        double amp1 = wave_form->amp1 / 2.5;
        double path1 = sppe2mm(wave_form->path1, velocity) / 2.0;
        double amp2 = wave_form->amp2 / 2.5;
        double path2 = sppe2mm(wave_form->path2, velocity) / 2.0;
        double amp3 = wave_form->amp3 / 2.5;
        double path3 = sppe2mm(wave_form->path3, velocity) / 2.0;

        tracing_origin = path0;
        /*if ((int)amp0 > (int)gate_a_height)
            tracing_origin = path0;
        else
            tracing_origin = gate_a_pos;
         */

        device->getParameter("gate_a_pos", value);
        gate_a_pos = value.toDouble();
        device->getParameter("gate_a_width", value);
        gate_a_width = value.toDouble();
        device->getParameter("gate_a_height", value);
        gate_a_height = value.toDouble();
        gate_a_pos_px = (gate_a_pos)/ (range / UTShadowDevice::_pe_wave_length);
        gate_a_width_px = gate_a_width / (range / UTShadowDevice::_pe_wave_length);
        gate_a_height_px = 256 - gate_a_height * 2.56;

        device->getParameter("gate_b_pos", value);
        gate_b_pos = value.toDouble();
        if (gate_b_tracing == "a") {
            gate_b_pos += tracing_origin;
            path1 += tracing_origin;
        }
        device->getParameter("gate_b_width", value);
        gate_b_width = value.toDouble();
        device->getParameter("gate_b_height", value);
        gate_b_height = value.toDouble();
        gate_b_pos_px = gate_b_pos / (range / UTShadowDevice::_pe_wave_length);
        gate_b_width_px = gate_b_width / (range / UTShadowDevice::_pe_wave_length);
        gate_b_height_px = 256 - gate_b_height *2.56;

        device->getParameter("gate_c_pos", value);
        gate_c_pos = value.toDouble();
        if (gate_c_tracing == "a") {
            gate_c_pos += tracing_origin;
            path2 += tracing_origin;
        }
        device->getParameter("gate_c_width", value);
        gate_c_width = value.toDouble();
        device->getParameter("gate_c_height", value);
        gate_c_height = value.toDouble();
        gate_c_pos_px = gate_c_pos / (range / UTShadowDevice::_pe_wave_length);
        gate_c_width_px = gate_c_width / (range / UTShadowDevice::_pe_wave_length);
        gate_c_height_px = 256 - gate_c_height *2.56;

        device->getParameter("gate_d_pos", value);
        gate_d_pos = value.toDouble();
        if (gate_d_tracing == "a") {
            gate_d_pos += tracing_origin;
            path3 += tracing_origin;
        }
        device->getParameter("gate_d_width", value);
        gate_d_width = value.toDouble();
        device->getParameter("gate_d_height", value);
        gate_d_height = value.toDouble();
        gate_d_pos_px = gate_d_pos / (range / UTShadowDevice::_pe_wave_length);
        gate_d_width_px = gate_d_width / (range / UTShadowDevice::_pe_wave_length);
        gate_d_height_px = 256 - gate_d_height *2.56;

        _ui->pe_gate_a_pos_label->setText(QString::number(path0+delay_mm, 'f', 1) + QString(" mm"));
        _ui->pe_gate_a_height_label->setText(QString::number(amp0, 'f', 1) + QString(" %"));
        _ui->pe_gate_b_pos_label->setText(QString::number(path1+delay_mm, 'f', 1) + QString(" mm"));
        _ui->pe_gate_b_height_label->setText(QString::number(amp1, 'f', 1) + QString(" %"));
        _ui->pe_gate_c_pos_label->setText(QString::number(path2+delay_mm, 'f', 1) + QString(" mm"));
        _ui->pe_gate_c_height_label->setText(QString::number(amp2, 'f', 1) + QString(" %"));
        _ui->pe_gate_d_pos_label->setText(QString::number(path3+delay_mm, 'f', 1) + QString(" mm"));
        _ui->pe_gate_d_height_label->setText(QString::number(amp3, 'f', 1) + QString(" %"));

        _ui->pe_encoder_label->setText(QString("Encoder0: %1, %2    Encoder1: %3, %4").
                                    arg(QString::number(wave_form->enc[0].fwd),
                                        QString::number(wave_form->enc[0].rvs),
                                        QString::number(wave_form->enc[1].rvs),
                                        QString::number(wave_form->enc[1].rvs)));

        _ui->pe_wave_view->setData(wave_form);
        QString rectification = _ui->pe_setting_rectification_input->currentText();
        _ui->pe_wave_view->draw(kImageViewDrawHorizontal, rectification);
        _ui->pe_wave_view->setGate(gate_a_pos_px, gate_a_width_px, gate_a_height_px, "a");
        _ui->pe_wave_view->setGate(gate_b_pos_px, gate_b_width_px, gate_b_height_px, "b");
        _ui->pe_wave_view->setGate(gate_c_pos_px, gate_c_width_px, gate_c_height_px, "c");
        _ui->pe_wave_view->setGate(gate_d_pos_px, gate_d_width_px, gate_d_height_px, "d");

        _ui->pe_ruler_ver_view->setAnchorPoint(Qt::AnchorLeft);
        _ui->pe_ruler_ver_view->setRange(100, 0);
        _ui->pe_ruler_hor_view->setAnchorPoint(Qt::AnchorTop);
        _ui->pe_ruler_hor_view->setRange(delay_mm, range + delay_mm);
        _ui->pe_delay_mm_label->setText("delay: " + QString::number(delay_mm, 'f', 1) + " mm");

    }

}

void MainWindow::UIEnable()
{
    _ui->server_connect_button->setEnabled(true);
}

void MainWindow::UIDisable()
{
    _ui->server_connect_button->setEnabled(false);
}

void MainWindow::UpdatePEParametersUI()
{
    UTShadowDevice* selectedDevice = _pa22x_client.selectedDevice();
    if (!selectedDevice)
        return;

    _ui->pe_setting_gain_input->blockSignals(true);
    _ui->pe_setting_voltage_input->blockSignals(true);
    _ui->pe_setting_pulse_width_input->blockSignals(true);
    _ui->pe_setting_prf_input->blockSignals(true);
    _ui->pe_setting_range_input->blockSignals(true);
    _ui->pe_setting_filter_input->blockSignals(true);
    _ui->pe_setting_delay_input->blockSignals(true);
    _ui->pe_setting_origin_input->blockSignals(true);
    _ui->pe_setting_dual_mode_input->blockSignals(true);
    _ui->pe_setting_damping_input->blockSignals(true);
    _ui->pe_setting_velocity_input->blockSignals(true);
    _ui->pe_setting_compress_rate_input->blockSignals(true);
    _ui->pe_setting_compress_mode->blockSignals(true);
    _ui->pe_setting_gate_pos_input->blockSignals(true);
    _ui->pe_setting_gate_width_input->blockSignals(true);
    _ui->pe_setting_gate_height_input->blockSignals(true);
    _ui->pe_setting_gate_tracing_input->blockSignals(true);
    _ui->pe_channel_select_1->blockSignals(true);
    _ui->pe_channel_select_2->blockSignals(true);
    _ui->pe_channel_select_3->blockSignals(true);
    _ui->pe_channel_select_4->blockSignals(true);
    _ui->pe_channel_select_5->blockSignals(true);
    _ui->pe_channel_select_6->blockSignals(true);
    _ui->pe_channel_select_7->blockSignals(true);
    _ui->pe_channel_select_8->blockSignals(true);

    QString value;

    if (selectedDevice->channels() == 2) {
        _ui->pe_channel_select_1->setEnabled(true);
        _ui->pe_channel_select_2->setEnabled(true);
        _ui->pe_channel_select_3->setEnabled(false);
        _ui->pe_channel_select_4->setEnabled(false);
        _ui->pe_channel_select_5->setEnabled(false);
        _ui->pe_channel_select_6->setEnabled(false);
        _ui->pe_channel_select_7->setEnabled(false);
        _ui->pe_channel_select_8->setEnabled(false);
    }
    else if (selectedDevice->channels() == 8) {
        _ui->pe_channel_select_1->setEnabled(true);
        _ui->pe_channel_select_2->setEnabled(true);
        _ui->pe_channel_select_3->setEnabled(true);
        _ui->pe_channel_select_4->setEnabled(true);
        _ui->pe_channel_select_5->setEnabled(true);
        _ui->pe_channel_select_6->setEnabled(true);
        _ui->pe_channel_select_7->setEnabled(true);
        _ui->pe_channel_select_8->setEnabled(true);
    }

    int selectedChannel = selectedDevice->selectedChannel();
    switch (selectedChannel) {
    case 0:
        _ui->pe_channel_select_1->setChecked(true);
        break;
    case 1:
        _ui->pe_channel_select_2->setChecked(true);
        break;
    case 2:
        _ui->pe_channel_select_3->setChecked(true);
        break;
    case 3:
        _ui->pe_channel_select_4->setChecked(true);
        break;
    case 4:
        _ui->pe_channel_select_5->setChecked(true);
        break;
    case 5:
        _ui->pe_channel_select_6->setChecked(true);
        break;
    case 6:
        _ui->pe_channel_select_7->setChecked(true);
        break;
    case 7:
        _ui->pe_channel_select_8->setChecked(true);
        break;
    }

    selectedDevice->getParameter("gain", value);
    _ui->pe_setting_gain_input->setValue(value.toDouble());

    selectedDevice->getParameter("high_voltage", value);
    _ui->pe_setting_voltage_input->setValue(value.toInt());

    selectedDevice->getParameter("pulse_width", value);
    _ui->pe_setting_pulse_width_input->setValue(value.toInt());

    selectedDevice->getParameter("prf", value);
    _ui->pe_setting_prf_input->setValue(value.toInt());

    selectedDevice->getParameter("range", value);
    _ui->pe_setting_range_input->setValue(value.toDouble());

    selectedDevice->getParameter("filter", value);
    if (value.toInt() == kFilter0)
        _ui->pe_setting_filter_input->setCurrentIndex(0);
    else if (value.toInt() == kFilter1)
        _ui->pe_setting_filter_input->setCurrentIndex(1);

    selectedDevice->getParameter("delay", value);
    _ui->pe_setting_delay_input->setValue(value.toDouble());

    selectedDevice->getParameter("origin", value);
    _ui->pe_setting_origin_input->setValue(value.toDouble());

    selectedDevice->getParameter("dual_mode", value);
    if (value == "true")
        _ui->pe_setting_dual_mode_input->setCurrentIndex(0);
    else if (value == "false")
        _ui->pe_setting_dual_mode_input->setCurrentIndex(1);

    selectedDevice->getParameter("damping", value);
    if (value == "80")
       _ui->pe_setting_damping_input->setCurrentIndex(0);
    else if (value == "400")
        _ui->pe_setting_damping_input->setCurrentIndex(1);

    selectedDevice->getParameter("velocity", value);
    _ui->pe_setting_velocity_input->setValue(value.toInt());

    selectedDevice->getParameter("compress_rate", value);
    _ui->pe_setting_compress_rate_input->setValue(value.toInt());

    selectedDevice->getParameter("compress_mode", value);
    if (value == "average")
        _ui->pe_setting_compress_mode->setCurrentIndex(0);
    else if (value == "peak")
        _ui->pe_setting_compress_mode->setCurrentIndex(1);


    QString gate_pos = "";
    QString gate_width = "";
    QString gate_height = "";
    QString gate_tracing = "";
    if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_a_label) {
        gate_pos = "gate_a_pos";
        gate_width = "gate_a_width";
        gate_height = "gate_a_height";
        gate_tracing = "gate_a_tracing";
    }
    else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_b_label) {
        gate_pos = "gate_b_pos";
        gate_width = "gate_b_width";
        gate_height = "gate_b_height";
        gate_tracing = "gate_b_tracing";
    }
    else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_c_label) {
        gate_pos = "gate_c_pos";
        gate_width = "gate_c_width";
        gate_height = "gate_c_height";
        gate_tracing = "gate_c_tracing";
    }
    else if (_ui->pe_setting_gate_select_input->currentText() == MainWindow::_gate_d_label) {
        gate_pos = "gate_d_pos";
        gate_width = "gate_d_width";
        gate_height = "gate_d_height";
        gate_tracing = "gate_d_tracing";
    }

    // Gate setting
    selectedDevice->getParameter(gate_pos, value);
    _ui->pe_setting_gate_pos_input->setValue(value.toDouble());

    selectedDevice->getParameter(gate_width, value);
    _ui->pe_setting_gate_width_input->setValue(value.toDouble());

    selectedDevice->getParameter(gate_height, value);
    _ui->pe_setting_gate_height_input->setValue(value.toDouble());

    selectedDevice->getParameter(gate_tracing, value);
    if (value == "a")
        _ui->pe_setting_gate_tracing_input->setChecked(true);
    else
        _ui->pe_setting_gate_tracing_input->setChecked(false);

    _ui->pe_setting_gain_input->blockSignals(false);
    _ui->pe_setting_voltage_input->blockSignals(false);
    _ui->pe_setting_pulse_width_input->blockSignals(false);
    _ui->pe_setting_prf_input->blockSignals(false);
    _ui->pe_setting_range_input->blockSignals(false);
    _ui->pe_setting_filter_input->blockSignals(false);
    _ui->pe_setting_delay_input->blockSignals(false);
    _ui->pe_setting_origin_input->blockSignals(false);
    _ui->pe_setting_dual_mode_input->blockSignals(false);
    _ui->pe_setting_damping_input->blockSignals(false);
    _ui->pe_setting_velocity_input->blockSignals(false);
    _ui->pe_setting_compress_rate_input->blockSignals(false);
    _ui->pe_setting_compress_mode->blockSignals(false);
    _ui->pe_setting_gate_pos_input->blockSignals(false);
    _ui->pe_setting_gate_width_input->blockSignals(false);
    _ui->pe_setting_gate_height_input->blockSignals(false);
    _ui->pe_setting_gate_tracing_input->blockSignals(false);
    _ui->pe_channel_select_1->blockSignals(false);
    _ui->pe_channel_select_2->blockSignals(false);
    _ui->pe_channel_select_3->blockSignals(false);
    _ui->pe_channel_select_4->blockSignals(false);
    _ui->pe_channel_select_5->blockSignals(false);
    _ui->pe_channel_select_6->blockSignals(false);
    _ui->pe_channel_select_7->blockSignals(false);
    _ui->pe_channel_select_8->blockSignals(false);
}

void MainWindow::UpdatePAParametersUI()
{
    UTShadowDevice* selectedDevice = _pa22x_client.selectedDevice();
    if (!selectedDevice)
        return;

    _ui->pa_setting_again_input->blockSignals(true);
    _ui->pa_setting_dgain_input->blockSignals(true);
    _ui->pa_setting_voltage_input->blockSignals(true);
    _ui->pa_setting_pulse_width_input->blockSignals(true);
    _ui->pa_setting_prf_input->blockSignals(true);
    _ui->pa_setting_range_input->blockSignals(true);
    _ui->pa_setting_filter_input->blockSignals(true);
    _ui->pa_setting_gate_a_pos_input->blockSignals(true);
    _ui->pa_setting_gate_a_width_input->blockSignals(true);
    _ui->pa_setting_gate_a_height_input->blockSignals(true);
    _ui->pa_setting_gate_b_pos_input->blockSignals(true);
    _ui->pa_setting_gate_b_width_input->blockSignals(true);
    _ui->pa_setting_gate_b_height_input->blockSignals(true);
    _ui->pa_setting_a_wave_index_input->blockSignals(true);
    _ui->pa_scan_rule_type_input->blockSignals(true);
    _ui->pa_scan_rule_longitude_velocity_input->blockSignals(true);
    _ui->pa_scan_rule_shear_velocity_input->blockSignals(true);
    _ui->pa_scan_rule_probe_frequence_input->blockSignals(true);
    _ui->pa_scan_rule_probe_elements_input->blockSignals(true);
    _ui->pa_scan_rule_probe_element_distance_input->blockSignals(true);
    _ui->pa_scan_rule_probe_delay_input->blockSignals(true);
    _ui->pa_scan_rule_wedge_mode_input->blockSignals(true);
    _ui->pa_scan_rule_wedge_type_input->blockSignals(true);
    _ui->pa_scan_rule_wedge_velocity_input->blockSignals(true);
    _ui->pa_scan_rule_wedge_angle_input->blockSignals(true);
    _ui->pa_scan_rule_wedge_height_input->blockSignals(true);
    _ui->pa_scan_rule_aperture_first_element_input->blockSignals(true);
    _ui->pa_scan_rule_aperture_element_size_input->blockSignals(true);
    _ui->pa_scan_rule_start_angle_input->blockSignals(true);
    _ui->pa_scan_rule_end_angle_input->blockSignals(true);
    _ui->pa_scan_rule_focus_input->blockSignals(true);
    _ui->pa_scan_rule_lscan_begin_ele_input->blockSignals(true);
    _ui->pa_scan_rule_lscan_end_ele_input->blockSignals(true);

    QString value;

    selectedDevice->getParameter("again", value);
    _ui->pa_setting_again_input->setValue(value.toDouble());

    selectedDevice->getParameter("dgain", value);
    _ui->pa_setting_dgain_input->setValue(value.toDouble());

    selectedDevice->getParameter("high_voltage", value);
    _ui->pa_setting_voltage_input->setValue(value.toInt());

    selectedDevice->getParameter("pulse_width", value);
    _ui->pa_setting_pulse_width_input->setValue(value.toInt());

    selectedDevice->getParameter("prf", value);
    _ui->pa_setting_prf_input->setValue(value.toInt());

    selectedDevice->getParameter("range", value);
    _ui->pa_setting_range_input->setValue(value.toDouble());

    selectedDevice->getParameter("filter", value);
    if (value.toInt() == kFilter0)
        _ui->pa_setting_filter_input->setCurrentIndex(0);
    else if (value.toInt() == kFilter1)
        _ui->pa_setting_filter_input->setCurrentIndex(1);
    else if (value.toInt() == kFilter2)
        _ui->pa_setting_filter_input->setCurrentIndex(2);

    selectedDevice->getParameter("gate_a_pos", value);
    _ui->pa_setting_gate_a_pos_input->setValue(value.toDouble());

    selectedDevice->getParameter("gate_a_width", value);
    _ui->pa_setting_gate_a_width_input->setValue(value.toDouble());

    selectedDevice->getParameter("gate_a_height", value);
    _ui->pa_setting_gate_a_height_input->setValue(value.toInt());

    selectedDevice->getParameter("gate_b_pos", value);
    _ui->pa_setting_gate_b_pos_input->setValue(value.toDouble());

    selectedDevice->getParameter("gate_b_width", value);
    _ui->pa_setting_gate_b_width_input->setValue(value.toDouble());

    selectedDevice->getParameter("gate_b_height", value);
    _ui->pa_setting_gate_b_height_input->setValue(value.toInt());

    selectedDevice->getParameter("beam_index", value);
    _ui->pa_setting_a_wave_index_input->setValue(value.toInt());;

    QHash<QString, QString> scanRuleParameter;
    selectedDevice->getScanRuleParameter(scanRuleParameter);

    if (scanRuleParameter["scan_type"] == "sscan")
        _ui->pa_scan_rule_type_input->setCurrentIndex(0);
    else if (scanRuleParameter["scan_type"] == "lscan")
        _ui->pa_scan_rule_type_input->setCurrentIndex(1);

    _ui->pa_scan_rule_longitude_velocity_input->setText(scanRuleParameter["longitude_velocity"]);
    _ui->pa_scan_rule_shear_velocity_input->setText(scanRuleParameter["shear_velocity"]);
    _ui->pa_scan_rule_probe_frequence_input->setText(scanRuleParameter["probe_frequence"]);
    _ui->pa_scan_rule_probe_elements_input->setText(scanRuleParameter["probe_elements"]);
    _ui->pa_scan_rule_probe_element_distance_input->setText(scanRuleParameter["probe_element_distance"]);
    _ui->pa_scan_rule_probe_delay_input->setText(scanRuleParameter["probe_delay"]);

    if (scanRuleParameter["wedge_mode"] == "enable")
        _ui->pa_scan_rule_wedge_mode_input->setCurrentIndex(0);
    else if (scanRuleParameter["wedge_mode"] == "disable")
        _ui->pa_scan_rule_wedge_mode_input->setCurrentIndex(1);


    if (scanRuleParameter["wedge_type"] == "l")
        _ui->pa_scan_rule_wedge_type_input->setCurrentIndex(0);
    else if (scanRuleParameter["wedge_type"] == "s")
        _ui->pa_scan_rule_wedge_type_input->setCurrentIndex(1);

    _ui->pa_scan_rule_wedge_velocity_input->setText(scanRuleParameter["wedge_velocity"]);
    _ui->pa_scan_rule_wedge_angle_input->setText(scanRuleParameter["wedge_angle"]);
    _ui->pa_scan_rule_wedge_height_input->setText(scanRuleParameter["wedge_height"]);
    _ui->pa_scan_rule_aperture_first_element_input->setText(scanRuleParameter["aperture_first_element"]);
    _ui->pa_scan_rule_aperture_element_size_input->setText(scanRuleParameter["aperture_element_size"]);
    _ui->pa_scan_rule_start_angle_input->setText(scanRuleParameter["start_angle"]);
    _ui->pa_scan_rule_end_angle_input->setText(scanRuleParameter["end_angle"]);
    _ui->pa_scan_rule_focus_input->setText(scanRuleParameter["focus"]);
    _ui->pa_scan_rule_lscan_begin_ele_input->setText(scanRuleParameter["lscan_begin_ele"]);
    _ui->pa_scan_rule_lscan_end_ele_input->setText(scanRuleParameter["lscan_end_ele"]);

    _ui->pa_setting_again_input->blockSignals(false);
    _ui->pa_setting_dgain_input->blockSignals(false);
    _ui->pa_setting_voltage_input->blockSignals(false);
    _ui->pa_setting_pulse_width_input->blockSignals(false);
    _ui->pa_setting_prf_input->blockSignals(false);
    _ui->pa_setting_range_input->blockSignals(false);
    _ui->pa_setting_filter_input->blockSignals(false);
    _ui->pa_setting_gate_a_pos_input->blockSignals(false);
    _ui->pa_setting_gate_a_width_input->blockSignals(false);
    _ui->pa_setting_gate_a_height_input->blockSignals(false);
    _ui->pa_setting_gate_b_pos_input->blockSignals(false);
    _ui->pa_setting_gate_b_width_input->blockSignals(false);
    _ui->pa_setting_gate_b_height_input->blockSignals(false);
    _ui->pa_setting_a_wave_index_input->blockSignals(false);
    _ui->pa_scan_rule_type_input->blockSignals(false);
    _ui->pa_scan_rule_longitude_velocity_input->blockSignals(false);
    _ui->pa_scan_rule_shear_velocity_input->blockSignals(false);
    _ui->pa_scan_rule_probe_frequence_input->blockSignals(false);
    _ui->pa_scan_rule_probe_elements_input->blockSignals(false);
    _ui->pa_scan_rule_probe_element_distance_input->blockSignals(false);
    _ui->pa_scan_rule_probe_delay_input->blockSignals(false);
    _ui->pa_scan_rule_wedge_mode_input->blockSignals(false);
    _ui->pa_scan_rule_wedge_type_input->blockSignals(false);
    _ui->pa_scan_rule_wedge_velocity_input->blockSignals(false);
    _ui->pa_scan_rule_wedge_angle_input->blockSignals(false);
    _ui->pa_scan_rule_wedge_height_input->blockSignals(false);
    _ui->pa_scan_rule_aperture_first_element_input->blockSignals(false);
    _ui->pa_scan_rule_aperture_element_size_input->blockSignals(false);
    _ui->pa_scan_rule_start_angle_input->blockSignals(false);
    _ui->pa_scan_rule_end_angle_input->blockSignals(false);
    _ui->pa_scan_rule_focus_input->blockSignals(false);
    _ui->pa_scan_rule_lscan_begin_ele_input->blockSignals(false);
    _ui->pa_scan_rule_lscan_end_ele_input->blockSignals(false);
}

void MainWindow::SetupPAScanrule()
{
    UTShadowDevice* device = _pa22x_client.selectedDevice();
    if (!device)
        return;

    QHash<QString, QString> scanRuleParameter;

    if (_ui->pa_scan_rule_type_input->currentText() == "s")
        scanRuleParameter["scan_type"] = "sscan";
    else if (_ui->pa_scan_rule_type_input->currentText() == "l")
        scanRuleParameter["scan_type"] = "lscan";

    scanRuleParameter["longitude_velocity"] = _ui->pa_scan_rule_longitude_velocity_input->text();
    scanRuleParameter["aperture_first_element"] = _ui->pa_scan_rule_aperture_first_element_input->text();
    scanRuleParameter["aperture_element_size"] = _ui->pa_scan_rule_aperture_element_size_input->text();
    scanRuleParameter["shear_velocity"] = _ui->pa_scan_rule_shear_velocity_input->text();
    scanRuleParameter["probe_frequence"] = _ui->pa_scan_rule_probe_frequence_input->text();
    scanRuleParameter["probe_elements"] = _ui->pa_scan_rule_probe_elements_input->text();
    scanRuleParameter["probe_element_distance"] = _ui->pa_scan_rule_probe_element_distance_input->text();
    scanRuleParameter["probe_delay"] = _ui->pa_scan_rule_probe_delay_input->text();
    scanRuleParameter["wedge_mode"] = _ui->pa_scan_rule_wedge_mode_input->currentText();
    scanRuleParameter["wedge_type"] = _ui->pa_scan_rule_wedge_type_input->currentText();
    scanRuleParameter["wedge_angle"] = _ui->pa_scan_rule_wedge_angle_input->text();
    scanRuleParameter["wedge_velocity"] = _ui->pa_scan_rule_wedge_velocity_input->text();
    scanRuleParameter["wedge_height"] = _ui->pa_scan_rule_wedge_height_input->text();
    scanRuleParameter["start_angle"] = _ui->pa_scan_rule_start_angle_input->text();
    scanRuleParameter["end_angle"] = _ui->pa_scan_rule_end_angle_input->text();
    scanRuleParameter["focus"] = _ui->pa_scan_rule_focus_input->text();
    scanRuleParameter["lscan_begin_ele"] = _ui->pa_scan_rule_lscan_begin_ele_input->text();
    scanRuleParameter["lscan_end_ele"] = _ui->pa_scan_rule_lscan_end_ele_input->text();
    device->setupScanRule(scanRuleParameter);
}
