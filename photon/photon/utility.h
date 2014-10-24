
#ifndef _photon_utility_h_
#define _photon_utility_h_

namespace pm
{

float random();


float collide(const fw::Vector3 *center, float radious, const fw::Vector3 *base, const fw::Vector3 *dir);
float collide(const fw::Vector3 *v0, const fw::Vector3 *v1, const fw::Vector3 *v2, const fw::Vector3 *base, const fw::Vector3 *dir);

void reflect(const fw::Vector3 *center, const fw::Vector3 *intersection, fw::Vector3 *dir);
void reflect(const fw::Vector3 *v0, const fw::Vector3 *v1, const fw::Vector3 *v2, fw::Vector3 *dir);

void refract(fw::Vector3 *dest, const fw::Vector3 *in, const fw::Vector3 *normal, float eta1, float eta2);

void ortho_basis(fw::Matrix4 *basis, const fw::Vector3 *normal);

inline float average(fw::Vector3 &v)
{
	return (v.x + v.y + v.z) / 3.0f;
}

inline void diffuse_reflect()
{

}


}

// MT function prototype
void init_genrand(unsigned long s);
void init_by_array(unsigned long init_key[], int key_length);
unsigned long genrand_int32(void);
long genrand_int31(void);
double genrand_real1(void);
double genrand_real2(void);
double genrand_real3(void);
double genrand_res53(void);

#endif