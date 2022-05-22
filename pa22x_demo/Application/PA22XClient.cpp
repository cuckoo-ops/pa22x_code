#include <QtEndian>
#include <cmath>
#include <QDebug>
#include "PA22XClient.h"
#include "mtld/mtld.h"
#include "NDTMath.h"
//------------------------------------------------------------------------------
//
bool isPAShadowDeviceIdentifier(const QString& identifier)
{
    QRegularExpression re("PADevice_(\\d+)");
    if (re.match(identifier).hasMatch())
        return true;
    return false;
}
//------------------------------------------------------------------------------
//
bool isPEShadowDeviceIdentifier(const QString& identifier)
{
    QRegularExpression re("(PE2|PE8)Device_(\\d+)");
    if (re.match(identifier).hasMatch())
        return true;
    return false;
}
//------------------------------------------------------------------------------
//
bool isPAShadowDevice(UTShadowDevice* device)
{
    if (isPAShadowDeviceIdentifier(device->identifier()))
        return true;
}
//------------------------------------------------------------------------------
//
bool isPEShadowDevice(UTShadowDevice* device)
{
    if (isPEShadowDeviceIdentifier(device->identifier()))
        return true;
}
//------------------------------------------------------------------------------
//
UTShadowDevice::UTShadowDevice(const QString& identifier, ShadowCommander* commander)
    :_identifier(identifier), _device_number(65535), _commander(commander)
{
    qDebug() << "Create Shadow Device:" << _identifier;
    QRegularExpression re("(PE2|PE8|PA)Device_(\\d+)");
    QRegularExpressionMatch match = re.match(_identifier);
    if (match.hasMatch())
        _device_number = match.captured(2).toInt();
}
//------------------------------------------------------------------------------
//
UTShadowDevice::~UTShadowDevice()
{
}
//------------------------------------------------------------------------------
//
const QString& UTShadowDevice::identifier() const
{
    return _identifier;
}
//------------------------------------------------------------------------------
//
uint32_t UTShadowDevice::deviceNumber() const
{
    return _device_number;
}
//------------------------------------------------------------------------------
//
UTShadowPADevice::UTShadowPADevice(const QString& identifier, ShadowCommander* commander)
    : UTShadowDevice(identifier, commander), _selected_group(0)
{
    init();
}
//------------------------------------------------------------------------------
//
void UTShadowPADevice::init()
{
    for (int i = 0; i < UTShadowPADevice::_max_group; i ++) {

        QHash<QString, QString>* parameter = new QHash<QString, QString>;
        (*parameter)["again"] = "36.0";
        (*parameter)["dgain"] = "20.0";
        (*parameter)["high_voltage"] = "45";
        (*parameter)["pulse_width"] = "100";
        (*parameter)["prf"] = "1000";
        (*parameter)["range"] = "200.0";
        (*parameter)["filter"] = "0";
        (*parameter)["gate_a_pos"] = "0.0";
        (*parameter)["gate_a_width"] = "10.0";
        (*parameter)["gate_a_height"] = "50";
        (*parameter)["gate_b_pos"] = "0.0";
        (*parameter)["gate_b_width"] = "10.0";
        (*parameter)["gate_b_height"] = "50";
        (*parameter)["beam_index"] = "1";
        (*parameter)["velocity"] = "";
        _parameters.append(parameter);

        QHash<QString, QString>* scan_rules_parameter = new QHash<QString, QString>;
        (*scan_rules_parameter)["scan_type"] = "sscan";
        (*scan_rules_parameter)["longitude_velocity"] = "5900";
        (*scan_rules_parameter)["shear_velocity"] = "3230";
        (*scan_rules_parameter)["probe_frequence"] = "2.5";
        (*scan_rules_parameter)["probe_elements"] = "16";
        (*scan_rules_parameter)["probe_element_distance"] = "0.6";
        (*scan_rules_parameter)["probe_delay"] = "0.0";
        (*scan_rules_parameter)["wedge_mode"] = "disable";
        (*scan_rules_parameter)["wedge_type"] = "l";
        (*scan_rules_parameter)["wedge_velocity"] = "2337";
        (*scan_rules_parameter)["wedge_angle"] = "36.0";
        (*scan_rules_parameter)["wedge_height"] = "12.5";
        (*scan_rules_parameter)["aperture_first_element"] = "1";
        (*scan_rules_parameter)["aperture_element_size"] = "16";
        (*scan_rules_parameter)["start_angle"] = "-45";
        (*scan_rules_parameter)["end_angle"] = "45";
        (*scan_rules_parameter)["focus"] = "20";
        (*scan_rules_parameter)["lscan_begin_ele"] = "1";
        (*scan_rules_parameter)["lscan_end_ele"] = "16";
        (*scan_rules_parameter)["imge_span_start"] = "";
        (*scan_rules_parameter)["imge_span_end"] = "";
        _scan_rules_parameters.append(scan_rules_parameter);
    }
}
//------------------------------------------------------------------------------
//
void UTShadowPADevice::shadowConfig()
{
    QString response;
    uint32_t group = selectedGroup();
    QHash<QString, QString>* parameter = nullptr;

    parameter = _parameters[group];
    _commander->sendCommandSync(QString("set dev_select %1").arg(deviceNumber()), response);
    _commander->sendCommandSync(QString("set group_mode 1"), response);
    _commander->sendCommandSync(QString("set group_select %1").arg(group+1), response);
    _commander->sendCommandSync(QString("set data_stop"), response);

    _commander->sendCommandSync(QString("set prf %1").arg((*parameter)["prf"]), response);
    _commander->sendCommandSync(QString("set again %1").arg((*parameter)["again"]), response);
    _commander->sendCommandSync(QString("set dgain %1").arg((*parameter)["dgain"]), response);
    _commander->sendCommandSync(QString("set high_voltage %1").arg((*parameter)["high_voltage"]), response);
    _commander->sendCommandSync(QString("set pulse_width %1").arg((*parameter)["pulse_width"]), response);
    _commander->sendCommandSync(QString("set range %1").arg((*parameter)["range"]), response);
    _commander->sendCommandSync(QString("set filter %1").arg((*parameter)["filter"]), response);
    _commander->sendCommandSync(QString("set gate_pos a %1 %2 %3").
                                arg((*parameter)["gate_a_pos"]).
                                arg((*parameter)["gate_a_width"]).
                                arg((*parameter)["gate_a_height"]),
                                response);
    _commander->sendCommandSync(QString("set gate_pos b %1 %2 %3").
                                arg((*parameter)["gate_b_pos"]).
                                arg((*parameter)["gate_b_width"]).
                                arg((*parameter)["gate_b_height"]),
                                response);

    parameter = _scan_rules_parameters[group];
    _commander->sendCommandSync(QString("set scan_type %1"). arg((*parameter)["scan_type"]), response);
    if ((*parameter)["scan_type"] == "l") {
        _commander->sendCommandSync(QString("set lscan_ele %1 %2").
                                    arg((*parameter)["lscan_begin_ele"]).
                                    arg((*parameter)["lscan_end_ele"]),
                                    response);
    }

    _commander->sendCommandSync(QString("set velocity %1 %2").
                                arg((*parameter)["longitude_velocity"]).
                                arg((*parameter)["shear_velocity"]),
                                response);
    _commander->sendCommandSync(QString("set aperture %1 %2").
                                arg((*parameter)["aperture_first_element"]).
                                arg((*parameter)["aperture_element_size"]),
                                response);
    _commander->sendCommandSync(QString("set probe %1 %2 %3 %4").
                                arg((*parameter)["probe_frequence"]).
                                arg((*parameter)["probe_elements"]).
                                arg((*parameter)["probe_element_distance"]).
                                arg((*parameter)["probe_delay"]),
                                response);
    _commander->sendCommandSync(QString("set wedge %1 %2 %3 %4 %5").
                                arg((*parameter)["wedge_mode"]).
                                arg((*parameter)["wedge_type"]).
                                arg((*parameter)["wedge_angle"]).
                                arg((*parameter)["wedge_velocity"]).
                                arg((*parameter)["wedge_height"]),
                                response);
    _commander->sendCommandSync(QString("set angle %1 %2").
                                arg((*parameter)["start_angle"]).
                                arg((*parameter)["end_angle"]),
                                response);
    _commander->sendCommandSync(QString("set focus %1").
                                arg((*parameter)["focus"]),
                                response);

    _commander->sendCommandSync(QString("set scan_rule"), response);
    _commander->sendCommandSync(QString("set smooth true"), response);
    _commander->sendCommandSync(QString("set data_start"), response);
    _commander->sendCommandSync(QString("get img_scale"), response);

    if (response.contains(QRegularExpression("OK\\s+img_scale", QRegularExpression::CaseInsensitiveOption))) {
        QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
        (*parameter)["imge_span_start"] = response_sections[2] ;
        (*parameter)["imge_span_end"] = response_sections[3];
    }

    parameter = _parameters[group];
    _commander->sendCommandSync(QString("get velocity"), response);
    if (response.contains(QRegularExpression("OK\\s+velocity", QRegularExpression::CaseInsensitiveOption))) {
        QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
        (*parameter)["velocity"] = response_sections[2];
    }
    _commander->sendCommandSync(QString("get scan_rule"), response);
    if (response.contains(QRegularExpression("Ok\\s+scan_rule", QRegularExpression::CaseInsensitiveOption))) {
        _scan_rule[_selected_group] = response;
    }
}
//------------------------------------------------------------------------------
//
UTShadowPADevice::~UTShadowPADevice()
{
    qDeleteAll(_parameters);
    _parameters.clear();
}
//------------------------------------------------------------------------------
//
void UTShadowPADevice::copyImageData(int index, const unsigned char* data, size_t length)
{
    int group = index;
    memcpy(_pa_image[group], data, length);
}
//------------------------------------------------------------------------------
//
void UTShadowPADevice::copyAWaveData(int index, const unsigned char* data, size_t length)
{
    int group = index;
    memcpy(_pa_awave[group], data, length);
}
//------------------------------------------------------------------------------
//
bool UTShadowPADevice::setParameter(const QString& name, const QString& value)
{
    QString response;
    uint32_t group = selectedGroup();
    QHash<QString, QString>* parameter = _parameters[group];
    QHash<QString, QString>* scan_rules_parameter = _scan_rules_parameters[group];


    if (name == "gate_a_pos" || name == "gate_a_width" || name == "gate_a_height") {
        (*parameter)[name] = value;
        QString pos = (*parameter)["gate_a_pos"];
        QString weidth = (*parameter)["gate_a_width"];
        QString height = (*parameter)["gate_a_height"];
        _commander->sendCommandSync(QString("set gate_pos a %1 %2 %3").
                                    arg(pos).
                                    arg(weidth).
                                    arg(height),
                                    response);
        return true;
    }
    else if (name == "gate_b_pos" || name == "gate_b_width" || name == "gate_b_height") {
        (*parameter)[name] = value;
        QString pos = (*parameter)["gate_b_pos"];
        QString weidth = (*parameter)["gate_b_width"];
        QString height = (*parameter)["gate_b_height"];
        _commander->sendCommandSync(QString("set gate_pos b %1 %2 %3").
                                    arg(pos).
                                    arg(weidth).
                                    arg(height),
                                    response);
        return true;
    }
    else if (name == "beam_index") {
        (*parameter)[name] = value;
    }
    else if (name == "range") {
        // When update "range", we should get the "img_scale" parameters.
        (*parameter)[name] = value;
        _commander->sendCommandSync(QString("set %1 %2").
                                    arg(name).
                                    arg((*parameter)[name]), response);

        _commander->sendCommandSync(QString("get img_scale"), response);
        if (response.contains(QRegularExpression("OK\\s+img_scale", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*scan_rules_parameter)["imge_span_start"] = response_sections[2] ;
            (*scan_rules_parameter)["imge_span_end"] = response_sections[3];
        }
    }
    else {
        if (parameter->keys().contains(name)) {
            (*parameter)[name] = value;
            _commander->sendCommandSync(QString("set %1 %2").
                                        arg(name).
                                        arg((*parameter)[name]), response);
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
//
bool UTShadowPADevice::getParameter(const QString& name, QString& value)
{
    uint32_t group = selectedGroup();
    QHash<QString, QString>* parameter = _parameters[group];
    if (parameter->keys().contains(name)) {
        value = (*parameter)[name];
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
//
bool UTShadowPADevice::setupScanRule(QHash<QString, QString> scan_rule_parameter)
{
    uint32_t group = selectedGroup();
    QHash<QString, QString>* parameter = _scan_rules_parameters[group];

    (*parameter)["scan_type"] = scan_rule_parameter["scan_type"];
    (*parameter)["longitude_velocity"] = scan_rule_parameter["longitude_velocity"];
    (*parameter)["shear_velocity"] = scan_rule_parameter["shear_velocity"];
    (*parameter)["aperture_first_element"] = scan_rule_parameter["aperture_first_element"];
    (*parameter)["aperture_element_size"] = scan_rule_parameter["aperture_element_size"];
    (*parameter)["probe_frequence"] = scan_rule_parameter["probe_frequence"];
    (*parameter)["probe_elements"] = scan_rule_parameter["probe_elements"];
    (*parameter)["probe_element_distance"] = scan_rule_parameter["probe_element_distance"];
    (*parameter)["probe_delay"] = scan_rule_parameter["probe_delay"];

    (*parameter)["wedge_mode"] = scan_rule_parameter["wedge_mode"];
    (*parameter)["wedge_type"] = scan_rule_parameter["wedge_type"];
    (*parameter)["wedge_angle"] = scan_rule_parameter["wedge_angle"];
    (*parameter)["wedge_velocity"] = scan_rule_parameter["wedge_velocity"];
    (*parameter)["wedge_height"] = scan_rule_parameter["wedge_height"];

    (*parameter)["start_angle"] = scan_rule_parameter["start_angle"];
    (*parameter)["end_angle"] = scan_rule_parameter["end_angle"];
    (*parameter)["focus"] = scan_rule_parameter["focus"];
    (*parameter)["lscan_begin_ele"] = scan_rule_parameter["lscan_begin_ele"];
    (*parameter)["lscan_end_ele"] = scan_rule_parameter["lscan_end_ele"];

    QString response;
    _commander->sendCommandSync("set data_stop", response);
    _commander->sendCommandSync(QString("set scan_type %1").
                                arg((*parameter)["scan_type"]), response);
    if ((*parameter)["scan_type"] == "l") {
        _commander->sendCommandSync(QString("set lscan_ele %1 %2").
                                    arg((*parameter)["lscan_begin_ele"]).
                                    arg((*parameter)["lscan_end_ele"]),
                                    response);
    }
    _commander->sendCommandSync(QString("set velocity %1 %2").
                                arg((*parameter)["longitude_velocity"]).
                                arg((*parameter)["shear_velocity"]),
                                response);
    _commander->sendCommandSync(QString("set aperture %1 %2").
                                arg((*parameter)["aperture_first_element"]).
                                arg((*parameter)["aperture_element_size"]),
                                response);
    _commander->sendCommandSync(QString("set probe %1 %2 %3 %4").
                                arg((*parameter)["probe_frequence"]).
                                arg((*parameter)["probe_elements"]).
                                arg((*parameter)["probe_element_distance"]).
                                arg((*parameter)["probe_delay"]),
                                response);
    _commander->sendCommandSync(QString("set wedge %1 %2 %3 %4 %5").
                                arg((*parameter)["wedge_mode"]).
                                arg((*parameter)["wedge_type"]).
                                arg((*parameter)["wedge_angle"]).
                                arg((*parameter)["wedge_velocity"]).
                                arg((*parameter)["wedge_height"]),
                                response);
    _commander->sendCommandSync(QString("set angle %1 %2").
                                arg((*parameter)["start_angle"]).
                                arg((*parameter)["end_angle"]),
                                response);
    _commander->sendCommandSync(QString("set focus %1").
                                arg((*parameter)["focus"]),
                                response);

    _commander->sendCommandSync(QString("set scan_rule"), response);
    _commander->sendCommandSync(QString("set smooth true"), response);

    _commander->sendCommandSync(QString("set data_start"), response);

    // When setup scan rule, get the parameters which we need to do the drawing process
    _commander->sendCommandSync(QString("get img_scale"), response);
    if (response.contains(QRegularExpression("OK\\s+img_scale", QRegularExpression::CaseInsensitiveOption))) {
        QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
        (*parameter)["imge_span_start"] = response_sections[2] ;
        (*parameter)["imge_span_end"] = response_sections[3];
    }

    parameter = _parameters[group];
    _commander->sendCommandSync(QString("get velocity"), response);
    if (response.contains(QRegularExpression("OK\\s+velocity", QRegularExpression::CaseInsensitiveOption))) {
        QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
        (*parameter)["velocity"] = response_sections[2];
    }
    _commander->sendCommandSync(QString("get scan_rule"), response);
    if (response.contains(QRegularExpression("Ok\\s+scan_rule", QRegularExpression::CaseInsensitiveOption))) {
        _scan_rule[_selected_group] = response;
    }
    return true;
}
//------------------------------------------------------------------------------
//
bool UTShadowPADevice::getScanRuleParameter(QHash<QString, QString>& scan_rule_parameter)
{
    uint32_t group = selectedGroup();
    QHash<QString, QString>* parameter = _scan_rules_parameters[group];
    scan_rule_parameter["scan_type"] = (*parameter)["scan_type"];
    scan_rule_parameter["longitude_velocity"] = (*parameter)["longitude_velocity"];
    scan_rule_parameter["shear_velocity"] = (*parameter)["shear_velocity"];
    scan_rule_parameter["aperture_first_element"] = (*parameter)["aperture_first_element"];
    scan_rule_parameter["aperture_element_size"] = (*parameter)["aperture_element_size"];
    scan_rule_parameter["probe_frequence"] = (*parameter)["probe_frequence"];
    scan_rule_parameter["probe_elements"] = (*parameter)["probe_elements"];
    scan_rule_parameter["probe_element_distance"] = (*parameter)["probe_element_distance"];
    scan_rule_parameter["probe_delay"] = (*parameter)["probe_delay"];
    scan_rule_parameter["wedge_mode"] = (*parameter)["wedge_mode"];
    scan_rule_parameter["wedge_type"] = (*parameter)["wedge_type"];
    scan_rule_parameter["wedge_angle"] = (*parameter)["wedge_angle"];
    scan_rule_parameter["wedge_velocity"] = (*parameter)["wedge_velocity"];
    scan_rule_parameter["wedge_height"] = (*parameter)["wedge_height"];
    scan_rule_parameter["start_angle"] = (*parameter)["start_angle"];
    scan_rule_parameter["end_angle"] = (*parameter)["end_angle"];
    scan_rule_parameter["focus"] = (*parameter)["focus"];
    scan_rule_parameter["imge_span_start"] = (*parameter)["imge_span_start"];
    scan_rule_parameter["imge_span_end"] = (*parameter)["imge_span_end"];
    scan_rule_parameter["lscan_begin_ele"] = (*parameter)["lscan_begin_ele"];
    scan_rule_parameter["lscan_end_ele"] = (*parameter)["lscan_end_ele"];
    return true;
}
//------------------------------------------------------------------------------
//
const QString& UTShadowPADevice::getScanRule()
{
    return _scan_rule[_selected_group];
}
//------------------------------------------------------------------------------
//
bool UTShadowPADevice::setScanRule(int index, const QString scan_rule)
{
    _scan_rule[index] = scan_rule;
    return true;
}
//------------------------------------------------------------------------------
//
QHash<QString, QString>* UTShadowPADevice::getParameters(int index)
{
    return _parameters[index];
}
//------------------------------------------------------------------------------
//
QHash<QString, QString>* UTShadowPADevice::getScanRuleParameters(int index)
{
    return _scan_rules_parameters[index];
}
//------------------------------------------------------------------------------
//
int UTShadowPADevice::selectedGroup()
{
    return _selected_group;
}
//------------------------------------------------------------------------------
//
bool UTShadowPADevice::setSelectedGroup(int group)
{
    QString response;
    _commander->sendCommandSync("set data_stop", response);
    _selected_group = group;
    _commander->sendCommandSync(QString("set group_select %1").arg(_selected_group+1), response);
    _commander->sendCommandSync("set data_start", response);
    return true;
}
//------------------------------------------------------------------------------
//
int UTShadowPADevice::selectedChannel()
{
    return -1;
}
//------------------------------------------------------------------------------
//
bool UTShadowPADevice::setSelectedChannel(int channel)
{
    return false;
}
//------------------------------------------------------------------------------
//
int UTShadowPADevice::groups()
{
    return UTShadowPADevice::_max_group;
}
//------------------------------------------------------------------------------
//
int UTShadowPADevice::channels()
{
    return 0;
}
//------------------------------------------------------------------------------
//
X22_Waveform* UTShadowPADevice::getAWave(int index)
{
    int32_t group = -1;
    if (index == -1)
        group = selectedGroup();
    else
        group = index;

    if (group >= 0 && group <UTShadowPADevice::_max_group) {
        QHash<QString, QString>* parameter = _parameters[group];
        int beam_index = (*parameter)["beam_index"].toInt();
        return &_pa_awave[group][beam_index];
    }
    return nullptr;
}
//------------------------------------------------------------------------------
//
unsigned char* UTShadowPADevice::getImage(int index)
{
    int32_t group = -1;
    if (index == -1)
        group = selectedGroup();
    else
        group = index;

    if (group >= 0 && group <UTShadowPADevice::_max_group) {
        return _pa_image[group];
    }

    return nullptr;
}
//------------------------------------------------------------------------------
//
void UTShadowPADevice::sync()
{
    QString response;

    _commander->sendCommandSync(QString("set dev_select %1").arg(_device_number), response);
    if (!response.contains(QRegularExpression("OK\\s+dev_select", QRegularExpression::CaseInsensitiveOption)))
        return;

    for (int group = 0; group < _max_group; group ++) {
        int groupNumber = group + 1;
        _commander->sendCommandSync(QString("set group_select %1").arg(groupNumber), response);
        if (!response.contains(QRegularExpression("OK\\s+group_select", QRegularExpression::CaseInsensitiveOption)))
            continue;

        QHash<QString, QString>* parameter = _parameters[group];

        _commander->sendCommandSync(QString("get again"), response);
        if (response.contains(QRegularExpression("OK\\s+again", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["again"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get dgain"), response);
        if (response.contains(QRegularExpression("OK\\s+dgain", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["dgain"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get high_voltage"), response);
        if (response.contains(QRegularExpression("OK\\s+high_voltage", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["high_voltage"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get pulse_width"), response);
        if (response.contains(QRegularExpression("OK\\s+pulse_width", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["pulse_width"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get prf"), response);
        if (response.contains(QRegularExpression("OK\\s+prf", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["prf"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get range"), response);
        if (response.contains(QRegularExpression("OK\\s+range", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["range"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get filter"), response);
        if (response.contains(QRegularExpression("OK\\s+filter", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["filter"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get velocity"), response);
        if (response.contains(QRegularExpression("OK\\s+velocity", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["velocity"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get gate_pos a"), response);
        if (response.contains(QRegularExpression("OK\\s+gate_pos", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gate_a_pos"] = response_sections[3];
            (*parameter)["gate_a_width"] = response_sections[4];
            (*parameter)["gate_a_height"] = response_sections[5];
        }

        _commander->sendCommandSync(QString("get gate_pos b"), response);
        if (response.contains(QRegularExpression("OK\\s+gate_pos", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gate_b_pos"] = response_sections[3];
            (*parameter)["gate_b_width"] = response_sections[4];
            (*parameter)["gate_b_height"] = response_sections[5];
        }

        QHash<QString, QString>* scan_rules_parameter = _scan_rules_parameters[group];

        _commander->sendCommandSync(QString("get scan_type"), response);
        if (response.contains(QRegularExpression("OK\\s+scan_type", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*scan_rules_parameter)["scan_type"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get velocity"), response);
        if (response.contains(QRegularExpression("OK\\s+velocity", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*scan_rules_parameter)["longitude_velocity"] = response_sections[2];
            (*scan_rules_parameter)["shear_velocity"] = response_sections[3];
        }

        _commander->sendCommandSync(QString("get aperture"), response);
        if (response.contains(QRegularExpression("OK\\s+aperture", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*scan_rules_parameter)["aperture_first_element"] = response_sections[2];
            (*scan_rules_parameter)["aperture_element_size"] = response_sections[3];
        }

        _commander->sendCommandSync(QString("get probe"), response);
        if (response.contains(QRegularExpression("OK\\s+probe", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*scan_rules_parameter)["probe_frequence"] = response_sections[2];
            (*scan_rules_parameter)["probe_elements"] = response_sections[3];
            (*scan_rules_parameter)["probe_element_distance"] = response_sections[4];
            (*scan_rules_parameter)["probe_delay"] = response_sections[5];
        }

        _commander->sendCommandSync(QString("get wedge"), response);
        if (response.contains(QRegularExpression("OK\\s+wedge", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*scan_rules_parameter)["wedge_mode"] = response_sections[2];
            (*scan_rules_parameter)["wedge_type"] = response_sections[3];
            (*scan_rules_parameter)["wedge_angle"] = response_sections[4];
            (*scan_rules_parameter)["wedge_velocity"] = response_sections[5];
            (*scan_rules_parameter)["wedge_height"] = response_sections[6];
        }

        _commander->sendCommandSync(QString("get angle"), response);
        if (response.contains(QRegularExpression("OK\\s+angle", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*scan_rules_parameter)["start_angle"] = response_sections[2];
            (*scan_rules_parameter)["end_angle"] = response_sections[3];
        }

        _commander->sendCommandSync(QString("get focus"), response);
        if (response.contains(QRegularExpression("OK\\s+focus", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*scan_rules_parameter)["focus"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get img_scale"), response);
        if (response.contains(QRegularExpression("OK\\s+img_scale", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*scan_rules_parameter)["imge_span_start"] = response_sections[2];
            (*scan_rules_parameter)["imge_span_end"] = response_sections[3];
        }

        _commander->sendCommandSync(QString("get scan_rule"), response);
        if (response.contains(QRegularExpression("Ok\\s+scan_rule", QRegularExpression::CaseInsensitiveOption))) {
            _scan_rule[group] = response;
        }
    }
}
//------------------------------------------------------------------------------
//
UTShadowPEDevice::UTShadowPEDevice(const QString& identifier, ShadowCommander* commander)
    : UTShadowDevice(identifier, commander), _selected_channel(0)
{
}
//------------------------------------------------------------------------------
//
void UTShadowPEDevice::init()
{
    for (int i = 0; i < channels(); i ++) {
        QHash<QString, QString>* parameter = new QHash<QString, QString>;
        (*parameter)["gain"] = "32.0";
        (*parameter)["high_voltage"] = "-250";
        (*parameter)["pulse_width"] = "125";
        (*parameter)["prf"] = "500";
        (*parameter)["range"] = "200";
        (*parameter)["delay"] = "0.0";
        (*parameter)["origin"] = "0.0";
        (*parameter)["filter"] = "0";
        (*parameter)["dual_mode"] = "false";
        (*parameter)["damping"] = "80";
        (*parameter)["velocity"] = "5900";
        (*parameter)["compress_rate"] = "10";
        (*parameter)["compress_mode"] = "average";
        (*parameter)["gate_a_pos"] = "20.0";
        (*parameter)["gate_a_width"] = "35.0";
        (*parameter)["gate_a_height"] = "20";
        (*parameter)["gate_a_measure"] = "peek";
        (*parameter)["gate_a_tracing"] = "false";
        (*parameter)["gate_b_pos"] = "25.0";
        (*parameter)["gate_b_width"] = "35.0";
        (*parameter)["gate_b_height"] = "40";
        (*parameter)["gate_b_measure"] = "peek";
        (*parameter)["gate_b_tracing"] = "false";
        (*parameter)["gate_c_pos"] = "30.0";
        (*parameter)["gate_c_width"] = "35.0";
        (*parameter)["gate_c_height"] = "60";
        (*parameter)["gate_c_measure"] = "peek";
        (*parameter)["gate_c_tracing"] = "false";
        (*parameter)["gate_d_pos"] = "35.0";
        (*parameter)["gate_d_width"] = "35.0";
        (*parameter)["gate_d_height"] = "80";
        (*parameter)["gate_d_measure"] = "peek";
        (*parameter)["gate_d_tracing"] = "false";
        (*parameter)["rectification"] = "full";

        _parameters.append(parameter);

        X22_Waveform* awave = new X22_Waveform();
        _pe_awaves.append(awave);
    }
}
//------------------------------------------------------------------------------
//
void UTShadowPEDevice::shadowConfig()
{
    QString response;
    qDebug() << "shadowConfig: " << _identifier;
    _commander->sendCommandSync(QString("set dev_select %1").arg(deviceNumber()), response);

    for (int channel = 0; channel < channels(); channel++) {
        uint32_t channelNumber = channel + 1;
        _commander->sendCommandSync(QString("set ch_select %1").arg(channelNumber), response);

        QHash<QString, QString>* parameter = _parameters[channel];

        _commander->sendCommandSync(QString("set gain %1").arg((*parameter)["gain"]), response);
        _commander->sendCommandSync(QString("set high_voltage %1").arg((*parameter)["high_voltage"]), response);
        _commander->sendCommandSync(QString("set pulse_width %1").arg((*parameter)["pulse_width"]), response);
        _commander->sendCommandSync(QString("set prf %1").arg((*parameter)["prf"]), response);
        _commander->sendCommandSync(QString("set range %1").arg((*parameter)["range"]), response);
        _commander->sendCommandSync(QString("set delay %1").arg((*parameter)["delay"]), response);
        _commander->sendCommandSync(QString("set filter %1").arg((*parameter)["filter"]), response);
        _commander->sendCommandSync(QString("set dual_mode %1").arg((*parameter)["dual_mode"]), response);
        _commander->sendCommandSync(QString("set damping %1").arg((*parameter)["damping"]), response);
        _commander->sendCommandSync(QString("set velocity %1").arg((*parameter)["velocity"]), response);
        _commander->sendCommandSync(QString("set compress_rate %1").arg((*parameter)["compress_rate"]), response);
        _commander->sendCommandSync(QString("set compress_mode %1").arg((*parameter)["compress_mode"]), response);
        _commander->sendCommandSync(QString("set rectification %1").arg((*parameter)["rectification"]), response);


        _commander->sendCommandSync(QString("set gate_pos a %1 %2 %3").
                                    arg((*parameter)["gate_a_pos"]).
                                    arg((*parameter)["gate_a_width"]).
                                    arg((*parameter)["gate_a_height"]),
                                    response);

        _commander->sendCommandSync(QString("set gate_ctl a %1 %2").
                                    arg((*parameter)["gate_a_measure"]).
                                    arg((*parameter)["gate_a_tracing"]),
                                    response);

        _commander->sendCommandSync(QString("set gate_pos b %1 %2 %3").
                                    arg((*parameter)["gate_b_pos"]).
                                    arg((*parameter)["gate_b_width"]).
                                    arg((*parameter)["gate_b_height"]),
                                    response);

        _commander->sendCommandSync(QString("set gate_ctl b %1 %2").
                                    arg((*parameter)["gate_b_measure"]).
                                    arg((*parameter)["gate_b_tracing"]),
                                    response);

        _commander->sendCommandSync(QString("set gate_pos c %1 %2 %3").
                                    arg((*parameter)["gate_c_pos"]).
                                    arg((*parameter)["gate_c_width"]).
                                    arg((*parameter)["gate_c_height"]),
                                    response);

        _commander->sendCommandSync(QString("set gate_ctl c %1 %2").
                                    arg((*parameter)["gate_c_measure"]).
                                    arg((*parameter)["gate_c_tracing"]),
                                    response);

        _commander->sendCommandSync(QString("set gate_pos d %1 %2 %3").
                                    arg((*parameter)["gate_d_pos"]).
                                    arg((*parameter)["gate_d_width"]).
                                    arg((*parameter)["gate_d_height"]),
                                    response);

        _commander->sendCommandSync(QString("set gate_ctl d %1 %2").
                                    arg((*parameter)["gate_d_measure"]).
                                    arg((*parameter)["gate_d_tracing"]),
                                    response);
    }
    _commander->sendCommandSync(QString("set data_start"), response);
    _commander->sendCommandSync(QString("set ch_select %1").arg(_selected_channel+1), response);
}
//------------------------------------------------------------------------------
//
UTShadowPEDevice::~UTShadowPEDevice()
{
    qDeleteAll(_parameters);
    _parameters.clear();

    qDeleteAll(_pe_awaves);
    _pe_awaves.clear();
}
//------------------------------------------------------------------------------
//
void UTShadowPEDevice::copyAWaveData(int index, const unsigned char* data, size_t length)
{
    int channel = index;
    memcpy(_pe_awaves[channel], data, length);
}
//------------------------------------------------------------------------------
//
void UTShadowPEDevice::copyImageData(int index, const unsigned char* data, size_t length)
{
}
//------------------------------------------------------------------------------
//
#define delayOffsetMM(ns, v)  Math::NStoMM(ns * 1000, v, 1);
bool UTShadowPEDevice::setParameter(const QString& name, const QString& value)
{
    QString response;
    uint32_t channel = selectedChannel();
    QHash<QString, QString>* parameter = _parameters[channel];

    if (name == "gate_a_pos" || name == "gate_a_width" || name == "gate_a_height") {        
        (*parameter)[name] = value;
        QString pos = (*parameter)["gate_a_pos"];
        QString weidth = (*parameter)["gate_a_width"];
        QString height = (*parameter)["gate_a_height"];
        _commander->sendCommandSync(QString("set gate_pos a %1 %2 %3").
                                    arg(pos).
                                    arg(weidth).
                                    arg(height),
                                    response);
        return true;
    }
    else if (name == "gate_b_pos" || name == "gate_b_width" || name == "gate_b_height") {
        (*parameter)[name] = value;
        QString pos = (*parameter)["gate_b_pos"];
        QString weidth = (*parameter)["gate_b_width"];
        QString height = (*parameter)["gate_b_height"];
        _commander->sendCommandSync(QString("set gate_pos b %1 %2 %3").
                                    arg(pos).
                                    arg(weidth).
                                    arg(height),
                                    response);
        return true;
    }
    else if (name == "gate_c_pos" || name == "gate_c_width" || name == "gate_c_height") {
        (*parameter)[name] = value;
        QString pos = (*parameter)["gate_c_pos"];
        QString weidth = (*parameter)["gate_c_width"];
        QString height = (*parameter)["gate_c_height"];
        _commander->sendCommandSync(QString("set gate_pos c %1 %2 %3").
                                    arg(pos).
                                    arg(weidth).
                                    arg(height),
                                    response);
        return true;
    }
    else if (name == "gate_d_pos" || name == "gate_d_width" || name == "gate_d_height") {
        (*parameter)[name] = value;
        QString pos = (*parameter)["gate_d_pos"];
        QString weidth = (*parameter)["gate_d_width"];
        QString height = (*parameter)["gate_d_height"];
        _commander->sendCommandSync(QString("set gate_pos d %1 %2 %3").
                                    arg(pos).
                                    arg(weidth).
                                    arg(height),
                                    response);
        return true;
    }
    else if (name == "gate_a_tracing") {
    }
    else if (name == "gate_b_tracing") {
        if (value == "a" || value == "false") {
            (*parameter)[name] = value;
            _commander->sendCommandSync(QString("set gate_ctl b peek %1").
                                        arg(QString(value)),
                                        response);
        }
    }
    else if (name == "gate_c_tracing") {
        if (value == "a" || value == "false") {
            (*parameter)[name] = value;
            _commander->sendCommandSync(QString("set gate_ctl c peek %1").
                                        arg(QString(value)),
                                        response);
        }
    }
    else if (name == "gate_d_tracing") {
        if (value == "a" || value == "false") {
            (*parameter)[name] = value;
            _commander->sendCommandSync(QString("set gate_ctl d peek %1").
                                        arg(QString(value)),
                                        response);
        }
    }
    else if (name == "origin") {
        (*parameter)["origin"] = value;
        double value = (*parameter)["origin"].toDouble() + (*parameter)["delay"].toDouble();
        _commander->sendCommandSync(QString("set delay %1").
                                    arg(QString::number(value)),
                                    response);
        return true;
    }
    else if (name == "delay") {
        (*parameter)["delay"] = value;
        double value = (*parameter)["origin"].toDouble() + (*parameter)["delay"].toDouble();
        _commander->sendCommandSync(QString("set delay %1").
                                    arg(QString::number(value)),
                                    response);
        return true;
    }
    else {
        if (parameter->keys().contains(name)) {
            (*parameter)[name] = value;
            _commander->sendCommandSync(QString("set %1 %2").
                                        arg(name).
                                        arg((*parameter)[name]), response);
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
//
bool UTShadowPEDevice::getParameter(const QString& name, QString& value)
{
    uint32_t channel = selectedChannel();
    QHash<QString, QString>* parameter = _parameters[channel];
    if (parameter->keys().contains(name)) {
        value = (*parameter)[name];
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
//
bool UTShadowPEDevice::setupScanRule(QHash<QString, QString> scan_rule_parameter)
{
    return false;
}
//------------------------------------------------------------------------------
//
bool UTShadowPEDevice::getScanRuleParameter(QHash<QString, QString>& scan_rule_parameter)
{
    return false;
}
//------------------------------------------------------------------------------
//
const QString& UTShadowPEDevice::getScanRule()
{
    return "";
}
//------------------------------------------------------------------------------
//
bool UTShadowPEDevice::setScanRule(int index, const QString scan_rule)
{
    return false;
}
//------------------------------------------------------------------------------
//
QHash<QString, QString>* UTShadowPEDevice::getParameters(int index)
{
    return _parameters[index];
}
//------------------------------------------------------------------------------
//
QHash<QString, QString>* UTShadowPEDevice::getScanRuleParameters(int index)
{
    return nullptr;
}
//------------------------------------------------------------------------------
//
int UTShadowPEDevice::selectedGroup()
{
    return -1;
}
//------------------------------------------------------------------------------
//
bool UTShadowPEDevice::setSelectedGroup(int group)
{
    return false;
}
//------------------------------------------------------------------------------
//
int UTShadowPEDevice::selectedChannel()
{
    return _selected_channel;
}
//------------------------------------------------------------------------------
//
bool UTShadowPEDevice::setSelectedChannel(int channel)
{
    QString response;
    _selected_channel = channel;
    _commander->sendCommandSync(QString("set ch_select %1").arg(_selected_channel+1), response);
    return true;
}
//------------------------------------------------------------------------------
//
int UTShadowPEDevice::groups()
{
    return -1;
}
//------------------------------------------------------------------------------
//
X22_Waveform* UTShadowPEDevice::getAWave(int index)
{
    uint32_t channel = -1;
    if (index == -1)
        channel = selectedChannel();
    else
        channel = index;

    if (channel >= 0 && channel < channels())
        return _pe_awaves[channel];

    return nullptr;
}
//------------------------------------------------------------------------------
//
unsigned char* UTShadowPEDevice::getImage(int index)
{
    return nullptr;
}
//------------------------------------------------------------------------------
//
void UTShadowPEDevice::sync()
{
    QString response;
    _commander->sendCommandSync(QString("set dev_select %1").arg(_device_number), response);
    if (!response.contains(QRegularExpression("OK\\s+dev_select", QRegularExpression::CaseInsensitiveOption)))
        return;

    for (int channel = 0; channel < channels(); channel ++) {
        int channelNumber = channel + 1;
        _commander->sendCommandSync(QString("set ch_select %1").arg(channelNumber), response);
        if (!response.contains(QRegularExpression("OK\\s+ch_select", QRegularExpression::CaseInsensitiveOption)))
            continue;

        QHash<QString, QString>* parameter = _parameters[channel];

        _commander->sendCommandSync(QString("get gain"), response);
        if (response.contains(QRegularExpression("OK\\s+gain", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gain"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get high_voltage"), response);
        if (response.contains(QRegularExpression("OK\\s+high_voltage", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["high_voltage"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get pulse_width"), response);
        if (response.contains(QRegularExpression("OK\\s+pulse_width", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["pulse_width"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get prf"), response);
        if (response.contains(QRegularExpression("OK\\s+prf", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["prf"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get range"), response);
        if (response.contains(QRegularExpression("OK\\s+range", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["range"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get delay"), response);
        if (response.contains(QRegularExpression("OK\\s+delay", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            QString delayTemp = response_sections[2];
            QString origin = (*parameter)["origin"];
            double delay = delayTemp.toDouble() - (*parameter)["origin"].toDouble();
            (*parameter)["delay"] = QString::number(delay);
        }

        _commander->sendCommandSync(QString("get filter"), response);
        if (response.contains(QRegularExpression("OK\\s+filter", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["filter"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get dual_mode"), response);
        if (response.contains(QRegularExpression("OK\\s+dual_mode", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["dual_mode"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get damping"), response);
        if (response.contains(QRegularExpression("OK\\s+damping", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["damping"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get velocity"), response);
        if (response.contains(QRegularExpression("OK\\s+velocity", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["velocity"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get compress_rate"), response);
        if (response.contains(QRegularExpression("OK\\s+compress_rate", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["compress_rate"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get rectification"), response);
        if (response.contains(QRegularExpression("OK\\s+rectification", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["rectification"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get compress_mode"), response);
        if (response.contains(QRegularExpression("OK\\s+compress_mode", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["compress_mode"] = response_sections[2];
        }

        _commander->sendCommandSync(QString("get gate_pos a"), response);
        if (response.contains(QRegularExpression("OK\\s+gate_pos", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gate_a_pos"] = response_sections[3];
            (*parameter)["gate_a_width"] = response_sections[4];
            (*parameter)["gate_a_height"] = response_sections[5];
        }

        _commander->sendCommandSync(QString("get gate_ctl a"), response);
        if (response.contains(QRegularExpression("OK\\s+gate_ctl", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gate_a_measure"] = response_sections[3];
            (*parameter)["gate_a_tracing"] = response_sections[4];
        }

        _commander->sendCommandSync(QString("get gate_pos b"), response);
        if (response.contains(QRegularExpression("OK\\s+gate_pos", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gate_b_pos"] = response_sections[3];
            (*parameter)["gate_b_width"] = response_sections[4];
            (*parameter)["gate_b_height"] = response_sections[5];
        }

        _commander->sendCommandSync(QString("get gate_ctl b"), response);
        if (response.contains(QRegularExpression("OK\\s+gate_ctl", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gate_b_measure"] = response_sections[3];
            (*parameter)["gate_b_tracing"] = response_sections[4];
        }

        _commander->sendCommandSync(QString("get gate_pos c"), response);
        if (response.contains(QRegularExpression("OK\\s+gate_pos", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gate_c_pos"] = response_sections[3];
            (*parameter)["gate_c_width"] = response_sections[4];
            (*parameter)["gate_c_height"] = response_sections[5];
        }

        _commander->sendCommandSync(QString("get gate_ctl c"), response);
        if (response.contains(QRegularExpression("OK\\s+gate_ctl", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gate_c_measure"] = response_sections[3];
            (*parameter)["gate_c_tracing"] = response_sections[4];
        }

        _commander->sendCommandSync(QString("get gate_pos d"), response);
        if (response.contains(QRegularExpression("OK\\s+gate_pos", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gate_d_pos"] = response_sections[3];
            (*parameter)["gate_d_width"] = response_sections[4];
            (*parameter)["gate_d_height"] = response_sections[5];
        }

        _commander->sendCommandSync(QString("get gate_ctl d"), response);
        if (response.contains(QRegularExpression("OK\\s+gate_ctl", QRegularExpression::CaseInsensitiveOption))) {
            QStringList response_sections = response.split(' ', QString::SkipEmptyParts);
            (*parameter)["gate_d_measure"] = response_sections[3];
            (*parameter)["gate_d_tracing"] = response_sections[4];
        }
    }
}
//------------------------------------------------------------------------------
//
UTShadowPE2Device::UTShadowPE2Device(const QString& identifier, ShadowCommander* commander)
    : UTShadowPEDevice(identifier, commander)
{
    init();
}
//------------------------------------------------------------------------------
//
UTShadowPE2Device::~UTShadowPE2Device()
{
}
//------------------------------------------------------------------------------
//
int UTShadowPE2Device::channels()
{
    return UTShadowPE2Device::_max_channel;
}
//------------------------------------------------------------------------------
//
UTShadowPE8Device::UTShadowPE8Device(const QString& identifier, ShadowCommander* commander)
    : UTShadowPEDevice(identifier, commander)
{
    init();
}
//------------------------------------------------------------------------------
//
UTShadowPE8Device::~UTShadowPE8Device()
{
}
//------------------------------------------------------------------------------
//
int UTShadowPE8Device::channels()
{
    return UTShadowPE8Device::_max_channel;
}
//------------------------------------------------------------------------------
//
ShadowCommander::ShadowCommander(const QHostAddress& server_address)
{
    _server_address = server_address;
}
//------------------------------------------------------------------------------
//
bool ShadowCommander::connect()
{
    _socket.connectToHost(_server_address, 2202);
    _socket.waitForConnected(4000);
    if (_socket.state() != QAbstractSocket::ConnectedState)
        return false;
    return true;
}
//------------------------------------------------------------------------------
//
QTcpSocket& ShadowCommander::socket()
{
    return _socket;
}
//------------------------------------------------------------------------------
//
ShadowCommander::~ShadowCommander()
{
    if (_socket.state() == QAbstractSocket::ConnectedState)
        _socket.disconnectFromHost();
}
//------------------------------------------------------------------------------
//
void ShadowCommander::sendCommandSync(const QString& commandString, QString& response)
{
    if (_socket.state() == QAbstractSocket::ConnectedState) {
        _socket.readAll();  // clear receive buffer.
        _socket.write((commandString + "\r\n\r\n").toLatin1());
        _socket.flush();
        emit commandSent(commandString);

        // wait for response.
        QString rcvData;
        while (_socket.waitForReadyRead(5000)) {
            rcvData.append(_socket.readAll());
            if (rcvData.right(4) == "\r\n\r\n") {
                QStringList lst = rcvData.split("\r\n\r\n", QString::SkipEmptyParts);
                auto smp = lst[0].simplified();
                qDebug() << lst[0];
                response = lst[0];
                emit responseGot(lst[0]);

                if (lst[0].contains(QRegularExpression("OK\\s+close_dev", QRegularExpression::CaseInsensitiveOption))) {
                    emit disconnect();
                }
                return;
            }
        }
        response = "no response";
        emit responseGot("timeout");
    }
}
//------------------------------------------------------------------------------
//
void ShadowCommander::sendCommandAsync(const QString& commandString)
{
    if (_socket.state() == QAbstractSocket::ConnectedState) {
        _socket.readAll(); // clear receive buffer.
        _socket.write((commandString + "\r\n\r\n").toLatin1());
        _socket.flush();

        // wait for response.
        QString rcvData;
        QString response;
        while (_socket.waitForReadyRead(5000)) {
            rcvData.append(_socket.readAll());
            if (rcvData.right(4) == "\r\n\r\n") {
                QStringList lst = rcvData.split("\r\n\r\n", QString::SkipEmptyParts);
                auto smp = lst[0].simplified();
                qDebug() << lst[0];
                response = lst[0];
                emit responseGotAsync(lst[0]);
                if (lst[0].contains(QRegularExpression("OK\\s+close_dev", QRegularExpression::CaseInsensitiveOption))) {
                    emit disconnect();
                }
                return;
            }
        }
        response = "no response";
        emit responseGot("timeout");
    }
}
//------------------------------------------------------------------------------
//
PA22XClient::PA22XClient(QObject* parent)
    : QObject(parent),
      _selected_device(nullptr),
      _data_source(nullptr),
      _commander(nullptr),
      _is_running(false)
{
}
//------------------------------------------------------------------------------
//
PA22XClient::~PA22XClient()
{
    deleteDataSource();
    deleteShadowDevices();
    deleteCommander();
}
//------------------------------------------------------------------------------
//
void PA22XClient::deviceIdentifier(uint32_t deviceNumber, QString& identifier)
{
    identifier = QString("UnknowDevice_??");
    if (deviceNumber >= 0 && deviceNumber <= 9)
        identifier = "PADevice_" + QString::number(deviceNumber);
    else if (deviceNumber >= 20 && deviceNumber <= 29)
        identifier = "PE2Device_" + QString::number(deviceNumber);
    else if (deviceNumber >= 80 && deviceNumber <= 89)
        identifier = "PE8Device_" + QString::number(deviceNumber);
}
//------------------------------------------------------------------------------
//
UTShadowDevice* PA22XClient::deviceByIdentifier(const QString& identifier)
{
    for (UTShadowDevice* device : _shadow_devices) {
        if (!device->identifier().compare(identifier, Qt::CaseInsensitive))
            return device;
    }
    return nullptr;
}
//------------------------------------------------------------------------------
//
UTShadowDevice* PA22XClient::deviceByNumber(const uint32_t deviceNumber)
{
    for (UTShadowDevice* device : _shadow_devices) {
        if (!device->deviceNumber() == deviceNumber)
            return device;
    }
    return nullptr;
}
//------------------------------------------------------------------------------
//
bool PA22XClient::start(const QString& serverIP)
{
    if (!_server_address.setAddress(serverIP))
        return false;

    if (!createCommander())
        return false;

    if (!createDataSource())
        return false;

    if (!createShadowDevices())
        return false;

    initShadowDevices();
    _selected_device = _shadow_devices[0];
    _is_running = true;

    return true;
}
//------------------------------------------------------------------------------
//
void PA22XClient::stop()
{
    if (!_is_running)
        return;

    QString response;
    if (_commander) {
        if (_commander->socket().state() == QAbstractSocket::ConnectedState) {
            _commander->sendCommandSync(QString("set data_stop"), response);
            _commander->sendCommandSync(QString("set close_dev"), response);
        }
    }
    deleteShadowDevices();
    deleteDataSource();
    deleteCommander();
    _is_running = false;
}
//------------------------------------------------------------------------------
//
bool PA22XClient::isRunning() {
    return _is_running;
}
//------------------------------------------------------------------------------
//
bool PA22XClient::createCommander()
{
    _commander = new ShadowCommander(_server_address);
    return _commander->connect();
}
//------------------------------------------------------------------------------
//
void PA22XClient::deleteCommander()
{
    if (_commander) {
        delete _commander;
        _commander = nullptr;
    }
}
//------------------------------------------------------------------------------
//
ShadowCommander* PA22XClient::getShadowCommander()
{
    return _commander;
}
//------------------------------------------------------------------------------
//
bool PA22XClient::createDataSource()
{
    connect(&_data_socket, SIGNAL(readyRead()), this, SLOT(readFromDataSocket()));

    _data_socket.connectToHost(_server_address, 2201);
    _data_socket.waitForConnected(4000);
    if (_data_socket.state() != QAbstractSocket::ConnectedState)
        return false;

    _data_source = new MTLDQTcpSocketReadWrite(&_data_socket);
    _data_source->begin();
    return true;
}
//------------------------------------------------------------------------------
//
void PA22XClient::deleteDataSource()
{
    if (_data_socket.state() == QAbstractSocket::ConnectedState)
        _data_socket.disconnectFromHost();
    disconnect(&_data_socket, SIGNAL(readyRead()), this, SLOT(readFromDataSocket()));

    if (_data_source) {
        _data_source->end();
        delete _data_source;
        _data_source = nullptr;
    }
}
//------------------------------------------------------------------------------
//
bool PA22XClient::createShadowDevices()
{
    QString response;

    _commander->sendCommandSync("set open_dev", response);
    if (!response.contains("ok", Qt::CaseInsensitive))
        return false;

    _commander->sendCommandSync("get dev_info", response);
    if (!response.contains("ok", Qt::CaseInsensitive))
        return false;

    response.remove("ok dev_info", Qt::CaseInsensitive);

    QRegularExpression re("(PE2|PE8|PA)Device_(\\d+)");
    QRegularExpressionMatchIterator i = re.globalMatch(response);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        if (match.hasMatch()) {
            QString identifier = match.captured(0);
            QString type = match.captured(1);
            QString number = match.captured(2);

            if (!type.compare("PE2", Qt::CaseInsensitive))
                _shadow_devices.append(new UTShadowPE2Device(identifier, _commander));
            else if (!type.compare("PE8", Qt::CaseInsensitive))
                _shadow_devices.append(new UTShadowPE8Device(identifier, _commander));
            else if (!type.compare("PA", Qt::CaseInsensitive))
                _shadow_devices.append(new UTShadowPADevice(identifier, _commander));
        }
    }
    return true;
}
//------------------------------------------------------------------------------
//
void PA22XClient::deleteShadowDevices()
{
    qDeleteAll(_shadow_devices);
    _shadow_devices.clear();
}
//------------------------------------------------------------------------------
//
void PA22XClient::initShadowDevices()
{
    for (UTShadowDevice* device : _shadow_devices)
        device->shadowConfig();
}
//------------------------------------------------------------------------------
//
QVector<UTShadowDevice*>& PA22XClient::shadowDevices()
{
    return _shadow_devices;
}
//------------------------------------------------------------------------------
//
UTShadowDevice* PA22XClient::selectedDevice()
{
    return _selected_device;
}
//------------------------------------------------------------------------------
//
void PA22XClient::setSelectedDevice(const QString& identifier)
{
    QString response;

    if (identifier == "UnknowDevice_??") {
        _selected_device = nullptr;
        return;
    }

    if (_selected_device) {
        _commander->sendCommandSync(QString("set dev_select %1").
                                    arg(_selected_device->deviceNumber()),
                                    response);
        _commander->sendCommandSync("set data_stop", response);
    }

    _selected_device = deviceByIdentifier(identifier);

    if (_selected_device) {
        _commander->sendCommandSync(QString("set dev_select %1").
                                    arg(_selected_device->deviceNumber()),
                                    response);
        _commander->sendCommandSync("set data_start", response);
    }
}

//------------------------------------------------------------------------------
//
bool PA22XClient::readFromDataSocket()
{
    MTLDPacket package;
    X22_DataHeader *header;
    auto error = _data_source->read(package);

    if (error == ME_SUCCESS) {
        header = (X22_DataHeader *)package.get_data();
        QString identifier;
        deviceIdentifier(header->_device, identifier);

        UTShadowDevice* device = deviceByIdentifier(identifier);

        if (device) {
            if (package.get_type() == QString("awavepa")) {
                uint32_t groupIndex = header->_group - 1;
                device->copyAWaveData(groupIndex,
                                      package.get_data() + sizeof(X22_DataHeader),
                                      UTShadowDevice::_nb_pa_beams * sizeof(X22_Waveform));
            }
            else if (package.get_type() == QString("simg512")) {
                uint32_t groupIndex = header->_group - 1;
                device->copyImageData(groupIndex,
                                      package.get_data() + sizeof(X22_DataHeader),
                                      UTShadowPADevice::_pa_image_height * UTShadowPADevice::_pa_image_width);
            }
            else if (package.get_type() == QString("awavepe")) {
                uint32_t channelIndex = header->_channel - 1;
                device->copyAWaveData(channelIndex,
                                      package.get_data() + sizeof(X22_DataHeader),
                                      sizeof(X22_Waveform));
            }
        }
        return true;
    }

    return false;
}
//------------------------------------------------------------------------------
//
void PA22XClient::sync()
{
    for (UTShadowDevice* device : _shadow_devices)
        device->sync();

    QString response;
    _commander->sendCommandSync(QString("set dev_select %1").arg(_selected_device->deviceNumber()), response);
    if (isPAShadowDevice(_selected_device))
        _commander->sendCommandSync(QString("set group_select %1").arg(_selected_device->selectedGroup()+1), response);
    else if (isPEShadowDevice(_selected_device))
        _commander->sendCommandSync(QString("set ch_select %1").arg(_selected_device->selectedChannel()+1), response);
}

