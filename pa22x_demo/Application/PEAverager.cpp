#include <cstring>
#include "PEAverager.h"

PEAverager::PEAverager(int data_length)
    : _level(1),
      _level_counter(0),
      _data_length(data_length),
      _buffer(new unsigned int[_data_length]()),
      _output(new unsigned char[_data_length]())
{

}

PEAverager::~PEAverager()
{
    if (_buffer)
        delete [] _buffer;

    if (_output)
        delete [] _output;
}

void PEAverager::setLevel(int level)
{
    lock();

    if (_buffer)
        memset(_buffer, 0, _data_length * sizeof(unsigned int));

    if (_output)
        memset(_output, 0, _data_length * sizeof(unsigned char));

    _level = level;
    _level_counter = 0;

    if (_level == 0)
        _level = 1;

    unlock();
}

int PEAverager::getLevel()
{
    return _level;
}

void PEAverager::clearBuffer()
{
    memset(_buffer, 0, _data_length * sizeof(unsigned int));
}

void PEAverager::buffer2Output()
{
    for (int i = 0; i < _data_length; i ++)
        _output[i] = (unsigned char)_buffer[i];
}

void PEAverager::average()
{
    for (int i = 0; i < _data_length; i ++) {
        _buffer[i] /= _level;
    }
    _level_counter = 0;
}

void PEAverager::setData(unsigned char* data)
{
    lock();
    for (int i = 0; i < _data_length; i ++)
        _buffer[i] += data[i];

    _level_counter ++;

    if (_level_counter == _level) {
        average();
        buffer2Output();
        clearBuffer();
    }
    unlock();
}

unsigned char* PEAverager::getData()
{
    return _output;
}

void PEAverager::lock()
{
    _data_lock.lock();
}

void PEAverager::unlock()
{
    _data_lock.unlock();
}
