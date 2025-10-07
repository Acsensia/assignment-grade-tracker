#pragma once
#include <iostream>
#include <fstream>
namespace PROJECT {
	using namespace std;
	template <typename T> class List {
	protected:
		struct Node {
			Node(T data, Node* prev = 0, Node* next = 0) {
				this->prev = prev;
				this->next = next;
				this->data = data;
			}

			Node* prev;
			Node* next;
			T data;
		};
		Node* head = 0;
		Node* tail = 0;
		int nodeCount = 0;
		//bool dontClearMemoryOn_Clear = false;

		Node* getNodeAddressByIndex(int index) {
			if (index < 0 || index > nodeCount) {
				return this->tail;
			}
			else {
				Node* currentlySelected;
				if (index > nodeCount / 2) {
					currentlySelected = this->tail;
					for (int i = nodeCount - 1; i > index; i--) {
						currentlySelected = currentlySelected->prev;
					}
				}
				else {
					currentlySelected = this->head;
					for (int i = 0; i < index; i++) {
						currentlySelected = currentlySelected->next;
					}
				}
				return currentlySelected;
			}
		}

	public:
		/*enum copyModes {
			copyData,
			copyPointers
		} copyMode = copyData;*/
		List() {}
		List(T* dataArray, int length) { add(dataArray, length); }
		List(T data) {
			Node* firstElement = new Node(data);
			this->head = this->tail = firstElement;
			this->nodeCount = 1;
		}
		~List() { this->clear(); }

		int length() {
			return nodeCount;
		}
		T getValue(int index) {
			if (nodeCount) return getNodeAddressByIndex(index)->data;
			else throw "No elements in the list!";
		}
		T* getReference(int index) {
			if (nodeCount) return  &(getNodeAddressByIndex(index)->data);
			else throw "No elements in the list!";
		}
		List& addBlank(int index = -1) {
			T* blank = new T;
			add(*blank, index);
			delete blank;
			return *this;
		}
		List& add(T data, int index = -1) {
			Node* addedElement = new Node(data);
			if (nodeCount) {
				Node* indexElement = getNodeAddressByIndex(index);
				if ((index >= this->length() || index < 0) && indexElement == this->tail) {
					//^^^additional check to make inserting elements before the last element possible.
					indexElement->next = addedElement;
					addedElement->prev = indexElement;
					this->tail = addedElement;
				}
				else if (indexElement == this->head) {
					indexElement->prev = addedElement;
					addedElement->next = indexElement;
					this->head = addedElement;
				}
				else {
					Node* before_indexElement = indexElement->prev;
					indexElement->prev = addedElement;
					before_indexElement->next = addedElement;
					addedElement->prev = before_indexElement;
					addedElement->next = indexElement;
				}
			}
			else {
				this->tail = this->head = addedElement;
			}
			nodeCount++;
			return *this;
		}
		List& add(T* dataArray, int length, int index = -1) {
			for (int i = length - 1; i > -1; i--) { add(dataArray[i], index); }
			return *this;
		}
		List<T>& remove(int index) {
			if (nodeCount == 1) {
				if (this->tail == this->head) delete this->tail;
				else {
					delete this->tail;
					delete this->head;
				}
				this->tail = this->head = 0;
				nodeCount = 0;
			}
			else if (!nodeCount) {
				throw "No elements in the list!";
			}
			else {
				Node* indexElement = getNodeAddressByIndex(index);
				if (indexElement == this->tail) {
					Node* before_indexElement = indexElement->prev;
					before_indexElement->next = 0;
					this->tail = before_indexElement;
				}
				else if (indexElement == this->head) {
					Node* after_indexElement = indexElement->next;
					after_indexElement->prev = 0;
					this->head = after_indexElement;
				}
				else {
					Node* before_indexElement = indexElement->prev;
					Node* after_indexElement = indexElement->next;
					before_indexElement->next = after_indexElement;
					after_indexElement->prev = before_indexElement;
				}
				delete indexElement;
				nodeCount--;
			}
			return *this;
		}
		List<T>& clear() {
			if (this->nodeCount > 1) {
				Node* currentlySelected;
				Node* nextElement;
				currentlySelected = nextElement = this->head;
				for (int i = 0; i < this->nodeCount; i++) {
					nextElement = currentlySelected->next;
					delete currentlySelected;
					currentlySelected = nextElement;
				}
				this->nodeCount = 0;
				this->tail = this->head = 0;
			}
			else if (this->nodeCount == 1) {
				remove(0);
			}
			return *this;
		}
		List<T>& operator=(List<T>& other) {////
			clear();
			for (int i = 0; i < other.length(); i++) add(*other.getReference(i));////
			return *this;
		}
		int findByValue(T data) {
			Node* currentlySelected = this->tail;
			int i = this->nodeCount - 1;
			for (; i > -1; i--) {
				if (currentlySelected->data == data) break;
				currentlySelected = currentlySelected->prev;
			}
			return i;
		}
		tuple<bool, string> toFile(const string& fileName, ofstream& out) {
			Node* currentlySelected = this->head;
			try {
				for (int i = 0; i < this->length(); i++) {
					out.write(reinterpret_cast<char*>(&currentlySelected->data), sizeof(T));
					currentlySelected = currentlySelected->next;
				}
			}
			catch (int errorNumber) {
				out.close();
				return make_tuple(false, "Error while saving data to file \"" + fileName + "\",  error number: " + to_string(errorNumber) + ".");
			}
			return make_tuple(true, "");
		}
		tuple<bool, string> fromFile(const string& fileName, ifstream& in, int numberOfIterations = -1) {
			this->clear();

			try {
				T data;

				if (numberOfIterations < 0) {
					while (in.peek() != EOF)
					{
						try { in.read(reinterpret_cast<char*>(&data), sizeof(T)); }
						catch (int errorNumber) { break; }
						this->add(data);
					}
				}
				else {
					for (int currentIteration = 0; currentIteration < numberOfIterations; currentIteration++) {
						try { in.read(reinterpret_cast<char*>(&data), sizeof(T)); }
						catch (int errorNumber) { break; }
						this->add(data);
					}
				}
			}
			catch (int errorNumber) {
				in.close();
				return make_tuple(false, "Error while loading data from file \"" + fileName + "\",  error number: " + to_string(errorNumber) + ".");
			}
			return make_tuple(true, "");
		}

		tuple<bool, string> toFile(const string& fileName) {
			ofstream out;
			try { out.open(fileName, ios::binary); }
			catch (int errorNumber) {
				return make_tuple(false, "Can't open file \"" + fileName + "\",  error number: " + to_string(errorNumber) + ".");
			}

			tuple<bool, string> fileSaveResult;
			fileSaveResult = toFile(fileName, out);

			out.close();
			return fileSaveResult;
		}
		tuple<bool, string> fromFile(const string& fileName, int numberOfIterations = -1) {
			ifstream in;
			try { in.open(fileName, ios::binary); }
			catch (int errorNumber) {
				return make_tuple(false, "Can't open file \"" + fileName + "\",  error number: " + to_string(errorNumber) + ".");
			}
			if (!in.good()) {
				in.close();
				return make_tuple(false, "File \"" + fileName + "\" doesn't exist.");
			}
			if (in.peek() == ifstream::traits_type::eof()) {
				in.close();
				return make_tuple(false, "File \"" + fileName + "\" is empty.");
			}

			tuple<bool, string> fileLoadResult;
			fileLoadResult = fromFile(fileName, in, numberOfIterations);

			in.close();
			return fileLoadResult;
		}
	};
}