#pragma once
//Not in use, I do not know how to create generic classes...
template <class T>
class Vec3{
private:
	T vals[3];

public:
	Vec3(T* vals);
	~Vec3();
public:
	inline T* getVals(){
		return vals;
	}
public:
	inline T getX(){ return vals[0]; };
	inline T getY(){ return vals[1]; };
	inline T getZ(){ return vals[2]; };
	Vec3& operator-= (Vec3 const& rt);
	Vec3& operator+= (Vec3 const& rt);
};