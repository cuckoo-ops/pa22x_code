#ifndef PEAVERAGER_H
#define PEAVERAGER_H
#include <QMutex>

class PEAverager
{
public:
    PEAverager(int data_length);
    ~PEAverager();
    void setLevel(int level);
    int getLevel();
    void setData(unsigned char* data);
    unsigned char* getData();
    void lock();
    void unlock();
private:
    QMutex _data_lock;
    void clearBuffer();
    void buffer2Output();
    void average();

    int _level;
    int _level_counter;
    int _data_length;
    unsigned int* _buffer;
    unsigned char* _output;

};
#endif // PEAVERAGER_H
