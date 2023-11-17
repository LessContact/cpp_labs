#pragma once
#include <initializer_list>
#include <cstddef>
#include <algorithm>
#include <sstream>
typedef char value_type;

class CircularBuffer {
private:
	value_type* buffer;
	size_t bufferStart;
	size_t Capacity;
	size_t Size;
public:
	CircularBuffer();
	~CircularBuffer();
	CircularBuffer(const CircularBuffer& cb);

	//Конструирует буфер заданной ёмкости.
	explicit CircularBuffer(size_t capacity);
	//Конструирует буфер заданной ёмкости, целиком заполняет его элементом elem.
	CircularBuffer(size_t capacity, const value_type& elem);
	CircularBuffer(std::initializer_list<value_type> list);

	//Доступ по индексу. Не проверяют правильность индекса.
	value_type& operator[](int i);
	const value_type& operator[](int i) const;

	//Доступ по индексу. Методы бросают исключение в случае неверного индекса.
	value_type& at(int i);
	const value_type& at(int i) const; 

	value_type& front(); //Ссылка на первый элемент.
	value_type& back();  //Ссылка на последний элемент.
	const value_type& front() const;
	const value_type& back() const;

	//Линеаризация - сдвинуть кольцевой буфер так, что его первый элемент
	//переместится в начало аллоцированной памяти. Возвращает указатель 
	//на первый элемент.
	value_type* linearize();
	//Проверяет, является ли буфер линеаризованным.
	bool is_linearized() const;
	//Сдвигает буфер так, что по нулевому индексу окажется элемент 
	//с индексом new_begin.
	void rotate(int new_begin);
	//Количество элементов, хранящихся в буфере.
	size_t size() const;
	bool empty() const;
	//true, если size() == capacity().
	bool full() const;
	//Количество свободных ячеек в буфере.
	size_t reserve() const;
	//ёмкость буфера
	size_t capacity() const;

	void set_capacity(size_t new_capacity);
	//Изменяет размер буфера.
	//В случае расширения, новые элементы заполняются элементом item.
	void resize(size_t new_size, const value_type& item = value_type());
	//Оператор присваивания.
	CircularBuffer& operator=(CircularBuffer cb);
	//Обменивает содержимое буфера с буфером cb.
	void swap(CircularBuffer& cb);

	//Добавляет элемент в конец буфера. 
	//Если текущий размер буфера равен его ёмкости, то переписывается
	//первый элемент буфера (т.е., буфер закольцован). 
	void push_back(const value_type& item = value_type());
	//Добавляет новый элемент перед первым элементом буфера. 
	//Аналогично push_back, может переписать последний элемент буфера.
	void push_front(const value_type& item = value_type());
	//удаляет последний элемент буфера.
	void pop_back();
	//удаляет первый элемент буфера.
	void pop_front();

	//Вставляет элемент item по индексу pos. Ёмкость буфера остается неизменной.
	void insert(size_t pos, const value_type& item = value_type());
	//Удаляет элементы из буфера в интервале [first, last).
	void erase(size_t first, size_t last);
	//Очищает буфер.
	void clear();
};

bool operator==(const CircularBuffer& a, const CircularBuffer& b);
bool operator!=(const CircularBuffer& a, const CircularBuffer& b);
