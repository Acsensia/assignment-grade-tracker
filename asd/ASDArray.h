#pragma once
#include <iostream>
namespace ASD {
	template<class T>
	class Array
	{
		T * ar;
		unsigned int count, size, grow;
		public:
			Array(unsigned int g = 10);
			Array(T element);
			Array(Array & copy) { ar = 0; Copy(copy);}

			~Array() { SetSize(0); }

			unsigned int GetCount() { return count; }
			unsigned int GetSize() { return size; }
			unsigned int GetGrow() { return grow; }

			T& operator [](unsigned int i) { return ar[i]; }
			Array & operator = (Array & copy) { return Copy(copy);}

			//todo
			Array & operator<< (Array & copy) { return Copy(copy); }
			//

			Array & SetSize(unsigned int s);
			Array & Copy(Array & copy);
			
			
			T& RemoveAt(int index) {
				if (index >= 0 && index < count) {
					T* result = new T[count - 1];
					int resultIndex = 0;
					for (int beforeIndex = 0; beforeIndex < index; beforeIndex++, resultIndex++) {
						result[resultIndex] = ar[beforeIndex];
					}

					for (int afterIndex = index + 1; afterIndex < count; afterIndex++, resultIndex++) {
						result[resultIndex] = ar[afterIndex];
					}

					RemoveAll();
					for (int index = 0; index < resultIndex; index++) {
						Add(result[index]);
					}
					delete[] result;
				}
				else count--;
				return *ar;
			}

			//todo
			Array& Add(Array& copy);
			Array Slice(unsigned int start, unsigned int finish);
			Array SubArray(unsigned int i, unsigned int _count);
			unsigned int FindIndex(T);
			//

			Array & Add(T v) {
				if (size == count) { SetSize(size + grow); }
				ar[count++] = v;
				return *this;
			}
			Array& AddAt(T v, int index) {
				if (index >= 0 && index < count) {
					T* result = new T[count + 1];
					int resultIndex = 0;
					for (int beforeIndex = 0; beforeIndex < index; beforeIndex++, resultIndex++) {
						result[resultIndex] = ar[beforeIndex];
					}

					result[index] = v;

					for (int afterIndex = index + 1; afterIndex < count; afterIndex++, resultIndex++) {
						result[resultIndex] = ar[afterIndex];
					}

					RemoveAll();
					for (int index = 0; index < resultIndex; index++) {
						Add(result[index]);
					}
					delete[] result;
				}
				else Add(v);
				return *this;
			}

			Array & RemoveAll() { count = 0; return *this; }
			Array & FreeExtra() { SetSize(GetCount());  return *this; }
	};

	template<class T>
	Array<T>::Array(unsigned int g) {
		if (g == 0) g = 5;
		grow = g;
		count = 0;
		size = grow;
		ar = 0;
		SetSize(size);
	}

	template<class T>
	Array<T>::Array(T element) : Array() {
		Add(element);
	}

	template<class T>
	Array<T> & Array<T>::Copy(Array & copy) {
		SetSize(0);
		SetSize(copy.GetCount());
		count = copy.GetCount();
		grow = copy.GetGrow();
		for (unsigned int i = 0; i < count; i++)
			ar[i] = copy[i];
		return *this;
	}

	template<class T>
	Array<T> & Array<T>::SetSize(unsigned int s) {
		size = s;
		if (size < count) count = size;
		if (s == 0) {
			delete[]ar;
			ar = 0;
			return *this;
		}
		T *arNew = new T[size];
		if (ar) {
			for (unsigned int i = 0; i < count; i++)
				arNew[i] = ar[i];
			delete[] ar;
		}
		ar = arNew;
		return *this;
	}
}
