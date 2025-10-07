#pragma once
namespace ASD {
	template<class CLASS_TYPE> class ObjectPtrHolder {
	public:
		CLASS_TYPE* objectPtr;
	};
	template<class CLASS_TYPE, typename RETURN_TYPE, typename ARG_TYPE> class Functor : public ObjectPtrHolder<CLASS_TYPE> {
	public:
		//CLASS_TYPE* objectPtr;
		RETURN_TYPE(CLASS_TYPE::* memberFunction) (ARG_TYPE);
		RETURN_TYPE operator () (ARG_TYPE arg) { return (*this->objectPtr.*this->memberFunction)(arg); }
	};
	template<class CLASS_TYPE, typename RETURN_TYPE> class Functor_no_args : public ObjectPtrHolder<CLASS_TYPE> {
	public:
		//CLASS_TYPE* objectPtr;
		RETURN_TYPE(CLASS_TYPE::* memberFunction) ();
		RETURN_TYPE operator () () { return (*this->objectPtr.*this->memberFunction)(); }
	};
	template<class CLASS_TYPE, typename ARG_TYPE> class Functor_void : public ObjectPtrHolder<CLASS_TYPE> {
	public:
		//CLASS_TYPE* objectPtr;
		void(CLASS_TYPE::* memberFunction) (ARG_TYPE);
		void operator () (ARG_TYPE arg) { return (*this->objectPtr.*this->memberFunction)(arg); }
	};
	template<class CLASS_TYPE> class Functor_void_no_args : public ObjectPtrHolder<CLASS_TYPE> {
	public:
		//CLASS_TYPE* objectPtr;
		void(CLASS_TYPE::* memberFunction) ();
		void operator () () { return (*this->objectPtr.*this->memberFunction)(); }
	};
}