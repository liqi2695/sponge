#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//初始化列表按照声明的顺序来
StreamReassembler::StreamReassembler(const size_t capacity) 
    : _unassembleStrMap({})
    , _bytesUnassembled(0)
    , _flagEof(false)
    , _posEof(0)
    , _output(capacity) 
    , _capacity(capacity) {}
//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.

//此函数用于不能直接插入ByteStream的情况， 作为一个有序缓冲区，需处理重复合并等情况
void StreamReassembler::subStrBuffer(const std::string &data, const size_t index) {
    if(_unassembleStrMap.empty()) {
        _unassembleStrMap[index] = data;
        _bytesUnassembled += data.size();
        return;
    }
    //考虑合并情况 
    /*******
     * 1. 左边可以合并 node与左边相交：
     *      a. 被包含 则直接丢弃
     *      b. 未被包含
     * 2. 右边可以合并
     * *********/

    auto it_left = _unassembleStrMap.lower_bound(index);
    string newData = data;
    size_t newIndex = index;
    //判断index左边是否有节点
    if(it_left != _unassembleStrMap.begin()) {
        it_left--;
        if(newIndex < (it_left->first + it_left->second.size())) { 
            if( (newIndex + newData.size()) <= (it_left->first + it_left->second.size()) ) {//说明被左边这个包给包含了，直接舍弃
                return;
            }
            //那么此时就是相交的情况，此时就是前面元素多一截，后面元素多一截，
            newData = it_left->second + data.substr(it_left->first + it_left->second.size() - newIndex);
            newIndex = it_left->first;
            //新节点插入缓冲map
            _unassembleStrMap[newIndex] = newData;

            _bytesUnassembled -= it_left->second.size();
            _unassembleStrMap.erase(it_left);
        }
    }
    //开始右边的处理
    auto it_right = _unassembleStrMap.upper_bound(index);

    //如果有相交情况，那么index+data.size() 也就是该字符串末尾字符的index必须大于右边字符串的第一个字符
    while(it_right != _unassembleStrMap.end() && index + data.size() > it_right->first) {
        //如果是被右边字符串包含，则直接舍弃。也就是字符串左边比左边小，右边比右边小
        if(index > it_right->first && index + data.size() < it_right->first + it_right->second.size())
            return;
        //与右边相交的情况
        if(index + data.size() < it_right->first + it_right->second.size()) {
            newData += it_right->second.substr(index + data.size() - it_left->first);
        }
        _bytesUnassembled -= it_right->second.size();
        _unassembleStrMap.erase(it_right++);

    }
    _unassembleStrMap[newIndex] = newData;
    _bytesUnassembled += newData.size();
}


void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    


    /*************************************************************************/
    size_t firstUnread = _output.bytes_read(); //多少pop了
    size_t firstUnassembled = _output.bytes_written();
    size_t firstUnacceptable = firstUnread + _capacity;

    string newData = data;
    size_t newIndex = index;

    if(index + data.size() < firstUnassembled || index >= firstUnacceptable) {
        return;
    }
    if(index + data.size() > firstUnacceptable) {
        newData = data.substr(0, firstUnacceptable - index);
    }

    if(index <= firstUnassembled) {
        _output.write(data.substr(0, firstUnassembled - index));
        //检查缓冲区
        auto it = _unassembleStrMap.begin();
        while(it->first <= _output.bytes_written()) {
            if(it->first + it->second.size() > newIndex + newData.size()) {
                _output.write(it->second.substr(_output.bytes_written() - it->first));
            }
            _bytesUnassembled -= it->second.size();
            _unassembleStrMap.erase(it++);
        }
    } else {
        subStrBuffer(data, index);
    }

    if(eof) {
        _flagEof = true;
        _posEof = index + data.size();
    }
    if(_flagEof && _output.bytes_written() == _posEof) 
        _output.end_input();

}

size_t StreamReassembler::unassembled_bytes() const { return _bytesUnassembled; }

bool StreamReassembler::empty() const { return _bytesUnassembled == 0; }
