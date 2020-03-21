#include "Vec3.h"

template <class T>
Vec3<T>::Vec3(T* vals):
vals(vals)
{};

template <class T>
Vec3<T>::~Vec3(){
	delete vals;
};

template <class T>
Vec3<T>& Vec3<T>::operator-= (Vec3 const& rt){
	vals[0] -= rt.getX();
	vals[1] -= rt.getY();
	vals[2] -= rt.getZ();
	return *this;
};

template <class T>
Vec3<T>& Vec3<T>::operator+= (Vec3 const& rt){
	vals[0] += rt.getX();
	vals[1] += rt.getY();
	vals[2] += rt.getZ();
	return *this;
};
