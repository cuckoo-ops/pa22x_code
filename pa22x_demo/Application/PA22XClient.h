#ifndef PA22XCLIENT_H
#define PA22XCLIENT_H
#include <memory>
#include <map>
#include <QObject>
#include <QByteArray>
#include <QString>
#include <chrono>
#include <QStringListModel>
#include <QTcpSocket>
#include <QVector>
#include "mtld.h"

bool isPAShadowDeviceIdentifier(const QString& identifier);
bool isPEShadowDeviceIdentifier(const QString& identifier);

struct X22_DataHeader {
    uint32_t _device;		// 0 ~ 3
    union {					// 4 ~ 7
        uint32_t _group;
        uint32_t _channel;
    };
    uint8_t  rev[120];		// 8 ~ 127
} ;

struct X22_Encoder {
    uint32_t fwd;
    uint32_t rvs;
};

typedef enum _Filter {
    kFilter0,
    kFilter1,
    kFilter2
} Filter;


struct X22_Waveform {
    uint8_t  waveP[448];  // 0 ~ 447
    uint16_t path0;       // 448 449
    uint8_t  amp0;        // 450
    uint8_t  rev0;        // 451
    uint16_t path1;       // 452 453
    uint8_t  amp1;        // 454
    uint8_t  rev1;        // 455
    uint16_t path2;       // 456 457
    uint8_t  amp2;        // 458
    uint8_t  rev2;        // 459
    uint16_t path3;       // 460 461
    uint8_t  amp3;        // 462
    uint8_t  rev3;        // 463
    uint8_t  ch;          // 464
    uint8_t  rev4[47];    // 465 ~ 511
    uint8_t  waveN[448];  // 0 ~ 447
    X22_Encoder enc[2];      // 448 ~ 463
    uint8_t  rev5[48];    //
};

//------------------------------------------------------------------------------
//
class ShadowCommander : public QObject {
    Q_OBJECT
public:
    ShadowCommander(const QHostAddress& serverAddress);
    ~ShadowCommander();
    void sendCommandSync(const QString& commandString, QString& response);
    void sendCommandAsync(const QString& commandString);
    bool connect();
    QTcpSocket& socket();

private:
    QTcpSocket _socket;
    QHostAddress _server_address;

signals:
    void responseGot(QString);
    void responseGotAsync(QString);
    void commandSent(QString);
    void disconnect();
};
//------------------------------------------------------------------------------
//
class UTShadowDevice {
public:
    const static int _nb_pa_beams = 128;
    const static int _pa_image_width = 512;
    const static int _pa_image_height = 400;
    const static int _pe_wave_length = 448;

    UTShadowDevice(const QString& identifier, ShadowCommander* commander);
    virtual void init() = 0;
    virtual void shadowConfig() = 0;
    virtual ~UTShadowDevice();
    const QString& identifier() const;
    uint32_t deviceNumber() const;
    virtual bool setParameter(const QString& name, const QString& value) = 0;
    virtual bool getParameter(const QString& name, QString& value) = 0;
    virtual bool setupScanRule(QHash<QString, QString> scan_rule_parameter) = 0;
    virtual bool getScanRuleParameter(QHash<QString, QString>& scan_rule_parameter) = 0;
    virtual const QString& getScanRule() = 0;
    virtual bool setScanRule(int index, const QString scan_rule) = 0;
    virtual QHash<QString, QString>* getParameters(int index) = 0;
    virtual QHash<QString, QString>* getScanRuleParameters(int index) = 0;
    virtual int selectedGroup() = 0;
    virtual bool setSelectedGroup(int group) = 0;
    virtual int selectedChannel() = 0;
    virtual bool setSelectedChannel(int channel) = 0;
    virtual int groups() = 0;
    virtual int channels() = 0;
    virtual void copyImageData(int index, const unsigned char* data, size_t length) = 0;
    virtual void copyAWaveData(int index, const unsigned char* data, size_t length) = 0;
    virtual X22_Waveform* getAWave(int index = -1) = 0;
    virtual unsigned char* getImage(int index = -1) = 0;
    virtual void sync() = 0;
protected:
    QString _identifier;
    uint32_t _device_number;
    ShadowCommander* _commander;
};
//------------------------------------------------------------------------------
//
class UTShadowPADevice : public UTShadowDevice {
public:
    const static int _max_group = 4;
    UTShadowPADevice(const QString& identifier, ShadowCommander* commander);
    virtual ~UTShadowPADevice();
    virtual void init();
    virtual void shadowConfig();
    virtual bool setParameter(const QString& name, const QString& value);
    virtual bool getParameter(const QString& name, QString& value);
    virtual bool setupScanRule(QHash<QString, QString> scan_rule_parameter);
    virtual bool getScanRuleParameter(QHash<QString, QString>& scan_rule_parameter);
    virtual QHash<QString, QString>* getParameters(int index);
    virtual QHash<QString, QString>* getScanRuleParameters(int index);
    virtual const QString& getScanRule();
    virtual bool setScanRule(int index, const QString scan_rule);
    virtual int selectedGroup();
    virtual bool setSelectedGroup(int group);
    virtual int selectedChannel();
    virtual bool setSelectedChannel(int channel);
    virtual int groups();
    virtual int channels();
    virtual void copyImageData(int index, const unsigned char* data, size_t length);
    virtual void copyAWaveData(int index, const unsigned char* data, size_t length);
    virtual X22_Waveform* getAWave(int index = -1);
    virtual unsigned char* getImage(int index = -1);
    virtual void sync();

private:
    int _selected_group;
    QString _scan_rule[_max_group];
    QVector<QHash<QString, QString>*> _parameters;
    QVector<QHash<QString, QString>*> _scan_rules_parameters;
    unsigned char _pa_image[_max_group][_pa_image_height*_pa_image_width];
    X22_Waveform _pa_awave[_max_group][_nb_pa_beams];
};
//------------------------------------------------------------------------------
//
class UTShadowPEDevice : public UTShadowDevice {
public:
    UTShadowPEDevice(const QString& identifier, ShadowCommander* commander);
    virtual ~UTShadowPEDevice();
    virtual void init();
    virtual void shadowConfig();
    virtual bool setParameter(const QString& name, const QString& value);
    virtual bool getParameter(const QString& name, QString& value);
    virtual bool setupScanRule(QHash<QString, QString> scan_rule_parameter);
    virtual bool getScanRuleParameter(QHash<QString, QString>& scan_rule_parameter);
    virtual QHash<QString, QString>* getParameters(int index);
    virtual QHash<QString, QString>* getScanRuleParameters(int index);
    virtual const QString& getScanRule();
    virtual bool setScanRule(int index, const QString scan_rule);
    virtual int selectedGroup();
    virtual bool setSelectedGroup(int group);
    virtual int selectedChannel();
    virtual bool setSelectedChannel(int channel);
    virtual int groups();
    virtual int channels() = 0;
    virtual void copyImageData(int index, const unsigned char* data, size_t length);
    virtual void copyAWaveData(int index, const unsigned char* data, size_t length);
    virtual X22_Waveform* getAWave(int index = -1);
    virtual unsigned char* getImage(int index = -1);
    virtual void sync();
protected:
    int _selected_channel;
    QVector<QHash<QString, QString>*> _parameters;
    QVector<X22_Waveform*> _pe_awaves;
};
//------------------------------------------------------------------------------
//
class UTShadowPE2Device : public UTShadowPEDevice {
public:
    const static int _max_channel = 2;
    UTShadowPE2Device(const QString& identifier, ShadowCommander* commander);
    virtual ~UTShadowPE2Device();
    virtual int channels();
};
//------------------------------------------------------------------------------
//
class UTShadowPE8Device : public UTShadowPEDevice {
public:
    const static int _max_channel = 8;
    UTShadowPE8Device(const QString& identifier, ShadowCommander* commander);
    virtual ~UTShadowPE8Device();
    virtual int channels();
};
//------------------------------------------------------------------------------
//
class PA22XClient : public QObject
{
    Q_OBJECT

public:
    PA22XClient(QObject* parent = nullptr);
    ~PA22XClient();
    bool start(const QString& serverIP);
    void stop();
    QVector<UTShadowDevice*>& shadowDevices();
    UTShadowDevice* selectedDevice();
    void setSelectedDevice(const QString& identifier);
    ShadowCommander* getShadowCommander();
    void sync();
    bool isRunning();

private:
    void getResponse(const QString& responseString);
    bool createDataSource();
    void deleteDataSource();
    bool createCommander();
    void deleteCommander();
    bool createShadowDevices();
    void deleteShadowDevices();
    void initShadowDevices();
    void deviceIdentifier(uint32_t deviceNumber, QString& identifier);
    UTShadowDevice* deviceByIdentifier(const QString& identifier);
    UTShadowDevice* deviceByNumber(const uint32_t deviceNumber);
    UTShadowDevice* _selected_device;
    QTcpSocket _data_socket;
    IMTLDReadWrite* _data_source;
    QHostAddress _server_address;
    QVector<UTShadowDevice*> _shadow_devices;
    ShadowCommander* _commander;
    bool _is_running;

public slots:
    bool readFromDataSocket();

};
#endif // PA22XCLIENT_H

