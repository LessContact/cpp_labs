#include "buffer.h"

#include <algorithm>
#include <sstream>

CircularBuffer::CircularBuffer() {
	buffer = nullptr;
	bufferStart = 0;
	Capacity = 0;
	Size = 0;
}

CircularBuffer::~CircularBuffer() {
	delete[] buffer;
}

CircularBuffer::CircularBuffer(int capacity) : CircularBuffer() {
	buffer = new value_type[capacity];
	Capacity = capacity;
}

CircularBuffer::CircularBuffer(int capacity, const value_type& elem) : CircularBuffer(capacity) {
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

value_type& CircularBuffer::operator[](int i) {
	return buffer[(bufferStart + i) % capacity()];
}

const value_type& CircularBuffer::operator[](int i) const {
	return buffer[(bufferStart + i) % capacity()];
}

value_type& CircularBuffer::at(int i) {
	if (i >= size() || i < 0) {
		std::stringstream excp;
		excp << "CircularBuffer: i ( " << i << ") is invalid or is greater or equals to current size (" << size() << ")";
		throw std::out_of_range(excp.str());
	}
	return (*this)[i];
}

const value_type& CircularBuffer::at(int i) const {
	if (i >= size() || i < 0) {
		std::stringstream excp;
		excp << "CircularBuffer: i ( " << i << ") is invalid or is greater or equals to current size (" << size() << ")";
		throw std::out_of_range(excp.str());
	}
	return (*this)[i];
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
	size_t newSize = std::min(Size, new_capacity);
	auto* newBuffer = new value_type[new_capacity];
	if (buffer != nullptr) {
		size_t rightPartSize = std::min(newSize, Capacity - bufferStart);

		std::copy_n(buffer + bufferStart, rightPartSize, newBuffer);
		if (rightPartSize < newSize) {
			size_t leftPartSize = newSize - rightPartSize;
			std::copy_n(buffer, leftPartSize, newBuffer + rightPartSize);
		}
	}

	delete[] buffer;
	bufferStart = 0;
	buffer = newBuffer;
	Capacity = new_capacity;
	Size = newSize;
}

void CircularBuffer::resize(int new_size, const value_type& item) {
	if (new_size <= Size) {
		Size = new_size;
		return;
	}

	if (new_size > capacity()) {
		set_capacity(new_size);
	}

	size_t fillBegin = (bufferStart + Size) % Capacity;
	size_t fillQuantity = new_size - Size;

	size_t rightPartSize = std::min(fillQuantity, Capacity - fillBegin);
	std::fill_n(buffer + fillBegin, rightPartSize, item);
	if (rightPartSize < fillQuantity) {
		size_t second_part_size = fillQuantity - rightPartSize;
		std::fill_n(buffer, second_part_size, item);
	}

	Size = new_size;
}

void CircularBuffer::swap(CircularBuffer& cb) {
	CircularBuffer a = cb;
	cb = *this;
	*this = a;
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

void CircularBuffer::insert(int pos, const value_type& item) {
	size_t shiftQuantity = size() - pos;
	size_t Position = (bufferStart + pos) % capacity();
	size_t rightPartSize = std::min(shiftQuantity, capacity() - Position - 1);

	if (rightPartSize < shiftQuantity) {
		size_t leftPartSize = shiftQuantity - rightPartSize - 1;
		std::copy_n(buffer, leftPartSize, buffer + 1);
		buffer[0] = buffer[capacity() - 1];
	}

	std::copy_n(buffer + Position, rightPartSize, buffer + Position + 1);

	if (capacity() == size()) {
		if (pos != 0) {
			bufferStart = (bufferStart + 1) % capacity();
		}
	}
	else {
		Size++;
	}

	buffer[Position] = item;
}

void CircularBuffer::erase(int first, int last) {
	if (first == last) {
		return;
	}

	size_t shiftQuantity = size() - last;
	size_t lastPos = (bufferStart + last) % capacity();
	size_t firstPos = (bufferStart + first) % capacity();

	if (firstPos <= lastPos) {
		size_t rightPartSize = std::min(shiftQuantity, capacity() - lastPos);
		std::copy_n(buffer + lastPos, rightPartSize, buffer + firstPos);
		if (rightPartSize < shiftQuantity) {
			size_t leftPartSize = std::min(shiftQuantity - rightPartSize, capacity() - lastPos);
			std::copy_n(buffer, leftPartSize, buffer + firstPos + rightPartSize);
			if (rightPartSize + leftPartSize < shiftQuantity) {
				size_t third_part_size = shiftQuantity - rightPartSize - leftPartSize;
				std::copy_n(buffer + leftPartSize, third_part_size, buffer);
			}
		}
	}
	else {
		size_t rightPartSize = std::min(shiftQuantity, capacity() - firstPos);
		std::copy_n(buffer + lastPos, rightPartSize, buffer + firstPos);
		if (rightPartSize < shiftQuantity) {
			size_t leftPartSize = shiftQuantity - rightPartSize;
			std::copy_n(buffer + lastPos + rightPartSize, leftPartSize, buffer);
		}
	}
	Size -= (last - first);
}

void CircularBuffer::clear() {
	Size = 0;
}

bool operator==(const CircularBuffer& a, const CircularBuffer& b) {
	if (a.size() != b.size()) {
		return false;
	}

	for (size_t i = 0; i < a.size(); i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}

bool operator!=(const CircularBuffer& a, const CircularBuffer& b) {
	return !(a == b);
}
