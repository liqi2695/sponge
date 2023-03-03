#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`


using namespace std;


size_t ByteStream::write(const string &data) {
    size_t length = data.size();
    //写入，并返回写入的字节数
    if(data.size() > _remainCapacity) {
        length = _remainCapacity;
    }
    _writeBytes += length;
    _remainCapacity -= length;
    
    for(size_t i = 0; i < length; i++) {
        buffer.push_back(data[i]);
    }
    
    return length;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {

    size_t length = len;
    if(length > (_capacity - _remainCapacity)) {
        length = (_capacity - _remainCapacity);
    }
    return buffer.substr(0, length);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 

    size_t length = len;
    if(length > (_capacity -  _remainCapacity)) {
        _popBytes += _capacity;
        buffer = "";
        _remainCapacity = _capacity;
        return;
    }
    
    buffer = buffer.erase(0, length);
    _remainCapacity = _capacity - buffer.size();
    _popBytes += length;
    return;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {

    std::string str = peek_output(len);
    pop_output(len);
    return str;
}

void ByteStream::end_input() { _isEnd = 1; }

bool ByteStream::input_ended() const { return _isEnd; }

size_t ByteStream::buffer_size() const { return _capacity - _remainCapacity; }

bool ByteStream::buffer_empty() const { return buffer.empty(); }

bool ByteStream::eof() const { return _isEnd && buffer_empty(); }

size_t ByteStream::bytes_written() const { return _writeBytes; }

size_t ByteStream::bytes_read() const { return _popBytes; }

size_t ByteStream::remaining_capacity() const { return _remainCapacity; }
