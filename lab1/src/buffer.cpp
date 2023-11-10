#include "buffer.h"

CircularBuffer::CircularBuffer() {
	buffer = nullptr;
	bufferStart = 0;
	Capacity = 0;
	Size = 0;
}

CircularBuffer::~CircularBuffer() {
	delete[] buffer;
}

CircularBuffer::CircularBuffer(size_t capacity) : CircularBuffer() {
	buffer = new value_type[capacity];
	Capacity = capacity;
}

CircularBuffer::CircularBuffer(size_t capacity, const value_type& elem) : CircularBuffer(capacity) {
	std::fill_n(buffer, capacity, elem);
	Size = capacity;
}

CircularBuffer::CircularBuffer(const CircularBuffer& cb) {
	buffer = new value_type[cb.capacity()];
	for (size_t i = 0; i < cb.size(); ++i) {
		buffer[i] = cb.buffer[i];
	}
	Capacity = cb.capacity();
	bufferStart = cb.bufferStart;
	Size = cb.size();
}

CircularBuffer::CircularBuffer(std::initializer_list<value_type> list) : CircularBuffer(list.size()) {
	size_t i = 0;
	for (value_type val : list) {
		buffer[i++] = val;
	}
	Size = list.size();
}

value_type& CircularBuffer::operator[](int i) {
	return buffer[(bufferStart + i) % capacity()];
}

const value_type& CircularBuffer::operator[](int i) const {
	return buffer[(bufferStart + i) % capacity()];
}

value_type& CircularBuffer::at(int i) {
	if (i >= size() || i < 0) {
		std::stringstream excp;
		excp << "CircularBuffer: i: (" << i << ") is invalid or is greater or equals to current size: (" << size() << ")";
		throw std::out_of_range(excp.str());
	}
	return buffer[(bufferStart + i) % capacity()];
}

const value_type& CircularBuffer::at(int i) const {
	if (i >= size() || i < 0) {
		std::stringstream excp;
		excp << "CircularBuffer: i: (" << i << ") is invalid or is greater or equals to current size: (" << size() << ")";
		throw std::out_of_range(excp.str());
	}
	return buffer[(bufferStart + i) % capacity()];
}

size_t CircularBuffer::size() const {
	return Size;
}

bool CircularBuffer::empty() const {
	return size() == 0;
}

size_t CircularBuffer::capacity() const {
	return Capacity;
}

value_type& CircularBuffer::front() {
	return buffer[bufferStart];
}

value_type& CircularBuffer::back() {
	return buffer[(bufferStart + size()) % capacity()];
}

const value_type& CircularBuffer::front() const {
	return buffer[bufferStart];
}

const value_type& CircularBuffer::back() const {
	return buffer[(bufferStart + size()) % capacity()];
}

value_type* CircularBuffer::linearize() {
	std::rotate(buffer, buffer + bufferStart, buffer + capacity());
	bufferStart = 0;
	return buffer;
}

bool CircularBuffer::is_linearized() const {
	return bufferStart == 0;
}

void CircularBuffer::rotate(int new_begin) {
	std::rotate(buffer, buffer + new_begin, buffer + capacity());
}

bool CircularBuffer::full() const {
	return size() == capacity();
}

size_t CircularBuffer::reserve() const {
	return capacity() - size();
}

void CircularBuffer::set_capacity(size_t new_capacity) {
	if (new_capacity == capacity()) {
		return;
	}
	size_t newSize = std::min(Size, new_capacity); // if it shrinks
	auto* newBuffer = new value_type[new_capacity];
	if (!is_linearized()) {
        linearize();
    }
	if (buffer != nullptr) {
		std::copy_n(buffer, newSize, newBuffer);
	}
	delete[] buffer;
	bufferStart = 0;
	buffer = newBuffer;
	Capacity = new_capacity;
	Size = newSize;
}

void CircularBuffer::resize(size_t new_size, const value_type& item) {
	if (new_size <= Size) {
		Size = new_size;
		return;
	}
	if (new_size > capacity()) {
		set_capacity(new_size);
	}
	if (!is_linearized()) {
		linearize();
	}
	size_t fillQuantity = new_size - size();
	std::fill_n(buffer + size(), fillQuantity, item);
	Size = new_size;
}

void CircularBuffer::swap(CircularBuffer& cb) {
	std::swap(buffer, cb.buffer);
	std::swap(bufferStart, cb.bufferStart);
	std::swap(Size, cb.Size);
	std::swap(Capacity, cb.Capacity);
}

void CircularBuffer::push_back(const value_type& item) {
	if (capacity() == 0) {
		return;
	}

	buffer[(bufferStart + size()) % capacity()] = item;
	if (size() != capacity()) {
		Size++;
	}
	else {
		bufferStart = (bufferStart + 1) % capacity();
	}
}

void CircularBuffer::push_front(const value_type& item) {
	if (capacity() == 0) {
		return;
	}

	buffer[(bufferStart + capacity() - 1) % capacity()] = item;
	bufferStart = (bufferStart + capacity() - 1) % capacity();
	if (size() != capacity()) {
		Size++;
	}
}

void CircularBuffer::pop_back() {
	Size--;
}

void CircularBuffer::pop_front() {
	Size--;
	bufferStart = (bufferStart + 1) % capacity();
}

void CircularBuffer::insert(size_t pos, const value_type& item) {
	size_t shift_quantity = size() - pos;
	size_t absolute_pos = (bufferStart + pos) % capacity();


	if (capacity() == size()) {
		if (pos != 0) {
			bufferStart = (bufferStart + 1) % capacity();
		}
	}
	else {
		++Size;
	}

	buffer_[absolute_pos] = item;
}

void CircularBuffer::erase(size_t first, size_t last) {
	if (!is_linearized()) {
		linearize();
	}
	size_t remainder = capacity() - last;
	size_t diff = last - first;
	size_t lastPos = (bufferStart + last) % capacity();
	size_t firstPos = (bufferStart + first) % capacity();
	if (diff > 0) {
		std::copy_n(buffer + lastPos, std::min(remainder, diff), buffer + firstPos);
		Size -= diff;
	}
	else {
		std::copy_n(buffer + lastPos + 1, 1, buffer + firstPos);
		Size -= 1;
	}
}

void CircularBuffer::clear() {
	Size = 0;
}

bool operator==(const CircularBuffer& a, const CircularBuffer& b) {
	if (a.size() != b.size()) {
		return false;
	}

	for (size_t i = (size_t)0; i < a.size(); i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}

bool operator!=(const CircularBuffer& a, const CircularBuffer& b) {
	return !(a == b);
}

CircularBuffer& CircularBuffer::operator=(CircularBuffer cb) {
	this->swap(cb);
	return *this;
}
