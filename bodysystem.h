/*
 * Copyright 1993-2015 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

#ifndef __BODYSYSTEM_H__
#define __BODYSYSTEM_H__

#include <algorithm>
#include <string>
#include <cstdio>


enum NBodyConfig
{
    NBODY_CONFIG_RANDOM,
    NBODY_CONFIG_SHELL,
    NBODY_CONFIG_EXPAND,
    NBODY_NUM_CONFIGS
};

enum BodyArray
{
    BODYSYSTEM_POSITION,
    BODYSYSTEM_VELOCITY,
};

template <typename T> struct vec3
{
    typedef float   Type;
}; // dummy
template <>           struct vec3<float>
{
    typedef float3  Type;
};
template <>           struct vec3<double>
{
    typedef double3 Type;
};

template <typename T> struct vec4
{
    typedef float   Type;
}; // dummy
template <>           struct vec4<float>
{
    typedef float4  Type;
};
template <>           struct vec4<double>
{
    typedef double4 Type;
};


class string;

// BodySystem abstract base class
template <typename T>
class BodySystem
{
    public: // methods
        BodySystem(int numBodies) {}
        virtual ~BodySystem() {}

        virtual void loadTipsyFile(const std::string &filename) = 0;

        virtual void update(T deltaTime) = 0;

        virtual void setSoftening(T softening) = 0;
        virtual void setDamping(T damping) = 0;

        virtual T *getArray(BodyArray array) = 0;
        virtual void   setArray(BodyArray array, const T *data) = 0;

        virtual unsigned int getCurrentReadBuffer() const = 0;

        virtual unsigned int getNumBodies() const = 0;

        virtual void   synchronizeThreads() const {};

    protected: // methods
        BodySystem() {} // default constructor

        virtual void _initialize(int numBodies) = 0;
        virtual void _finalize() = 0;
};

inline float3
scalevec(float3 &vector, float scalar)
{
    float3 rt = vector;
    rt.x *= scalar;
    rt.y *= scalar;
    rt.z *= scalar;
    return rt;
}

inline float
normalize(float3 &vector)
{
    float dist = sqrtf(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);

    if (dist > 1e-6)
    {
        vector.x /= dist;
        vector.y /= dist;
        vector.z /= dist;
    }

    return dist;
}

inline float
dot(float3 v0, float3 v1)
{
    return v0.x*v1.x+v0.y*v1.y+v0.z*v1.z;
}

inline float3
cross(float3 v0, float3 v1)
{
    float3 rt;
    rt.x = v0.y*v1.z-v0.z*v1.y;
    rt.y = v0.z*v1.x-v0.x*v1.z;
    rt.z = v0.x*v1.y-v0.y*v1.x;
    return rt;
}


// utility function
template <typename T>
void randomizeBodies(NBodyConfig config, T *pos, T *vel, float *color, float clusterScale,
                     float velocityScale, int numBodies, bool vec4vel) 
{
	
	switch (config)
    {
        default:
		case NBODY_CONFIG_SHELL: //rotating ball of positive masses  option 1
		{
			float scale = clusterScale;
			float vscale = scale * velocityScale;
			int p = 0, v = 0;
			int i = 0;
			while (i < numBodies)//for(int i=0; i < numBodies; i++)
			{
				float x, y, z;
				//if (i % 2 == 0) //64 If you want a shell over the cube
				//{
					x = rand() / (float)RAND_MAX * 16 - 8;
					y = rand() / (float)RAND_MAX * 8 - 4;
					z = rand() / (float)RAND_MAX * 4 - 2;

				//}
				//else64 If you want a shell over the cube
				//{64 If you want a shell over the cube
					//x = rand() / (float)RAND_MAX * 10 - 5;
					//y = rand() / (float)RAND_MAX * 10 - 5;
					//z = rand() / (float)RAND_MAX * 10 - 5;

				//}


				float3 point = { x, y, z };
				//float lenSqr = dot(point, point);
				//if (lenSqr > 1)
				//	continue;

				pos[p++] = point.x * scale; // pos.x
				pos[p++] = point.y * scale; // pos.y
				pos[p++] = point.z * scale; // pos.z

				if (i % 64 == 0) //64
				{
					pos[p++] = -63.0f; // mass -
				}
				else
				{
					pos[p++] = 1.0f; // mass +
				}

				//x = 0.0f; // * (rand() / (float) RAND_MAX * 2 - 1);
				//y = 0.0f; // * (rand() / (float) RAND_MAX * 2 - 1);
				//z = 1.0f; // * (rand() / (float) RAND_MAX * 2 - 1);
				//float3 axis = { x, y, z };
				//normalize(axis);

				//if (1 - dot(point, axis) < 1e-6)
				//{
				//	axis.x = point.y;
				//	axis.y = point.x;
				//	normalize(axis);
				//}

				//if (point.y < 0) axis = scalevec(axis, -1);
				//float3 vv = { (float)pos[4 * i], (float)pos[4 * i + 1], (float)pos[4 * i + 2] };
				//vv = cross(vv, axis);

				//	vel[v++] = vv.x * vscale ;
				//	vel[v++] = vv.y * vscale ;
				//	vel[v++] = vv.z * vscale ;// mass -

				vel[v++] = 0 ;
				vel[v++] = 0 ;
				vel[v++] = 0 ;// mass -
				
				if (vec4vel)
				{
					vel[v++] = 1 / fabs(pos[p - 1]); // inverse mass
				}

				i++;
			}
		}
		break;

        case NBODY_CONFIG_RANDOM: // Config Random sphere with rotating initial velocities option 2
		{
			
			float scale = clusterScale;
			//float scale = clusterScale * std::max<float>(1.0f, numBodies / (1024.0f));
			float vscale = scale * velocityScale;
			float inner = 1.0f * scale;
			float outer = 1.3f * scale; //4.0f

			int p = 0, v = 0;
			int i = 0;

			while (i < numBodies)//for(int i=0; i < numBodies; i++)
			{
				float3 point;
				//const int scale = 16;

					point.x = rand() / (float)RAND_MAX * 16 - 8;
					point.y = rand() / (float)RAND_MAX * 8 - 4;
					point.z = rand() / (float)RAND_MAX * 4 - 2;



				//float lenSqr = dot(point, point);

				//if (lenSqr > 1)
				//	continue;

				float3 velocity;
				velocity.x = 0;//rand() / (float)RAND_MAX * 2 - 1;
				velocity.y = 0;// rand() / (float)RAND_MAX * 2 - 1;
				velocity.z = 0;//rand() / (float)RAND_MAX * 2 - 1;
				//lenSqr = dot(velocity, velocity);

				//if (lenSqr > 1)
				//	continue;

				pos[p++] = point.x * scale; // pos.x
				pos[p++] = point.y * scale; // pos.y
				pos[p++] = point.z * scale; // pos.z
				if (i % 64 == 0) //64
				{
					pos[p++] = -63.0f; // mass -


				}
				else
				{
					pos[p++] = 1.0f; // mass +

				}

				//vel[v++] = point.x * vscale;
				//vel[v++] = point.y * vscale;
				//vel[v++] = point.z * vscale;
				vel[v++] = 0;
				vel[v++] = 0;
				vel[v++] = 0;// mass -

				if (vec4vel)
				{
					vel[v++] = 1 / fabs(pos[p - 1]); // inverse mass
				}

				i++;
			}
		}
		break;

		
		

        case NBODY_CONFIG_EXPAND: // config VLS random rectangle with rotation and random velocities option 3
		{
			float scale = clusterScale;
			//float scale = clusterScale * std::max<float>(1.0f, numBodies / (1024.0f));
			float vscale = scale * velocityScale;


			int p = 0, v = 0;
			int i = 0;

			while (i < numBodies)//for(in t i=0; i < numBodies; i++)
			{
				float3 point;
				//const int scale = 16;
				point.x = rand() / (float)RAND_MAX * 2 - 1;
				point.y = rand() / (float)RAND_MAX * 2 - 1;
				point.z = rand() / (float)RAND_MAX * 2 - 1;
				float lenSqr = dot(point, point);

				if (lenSqr > 1)
					continue;

				pos[p++] = point.x * scale; // pos.x
				pos[p++] = point.y * scale; // pos.y
				pos[p++] = point.z * scale; // pos.z

				if (i % 64 == 0) //64
				{
					pos[p++] = -63.0f; // mass -
				}
				else
				{
					pos[p++] = 1.0f; // mass +
				}

				vel[v++] = point.x * vscale; // pos.x
				vel[v++] = point.y * vscale; // pos.x
				vel[v++] = point.z * vscale; // pos.x

				if (vec4vel) vel[v++] = 1.0f; // inverse mass

				i++;
			}
		}
		break;
    }

    if (color) //check
    {
		int v = 0; int c=0;

        for (int i=0; i < numBodies; i++)
        {
            //const int scale = 16;
			c = c + 3;
			if (pos[c] < 0.0f) 
			{
				//jaune MAsses Negs
				color[v++] = 0.0f; //0.9
				color[v++] = 1.0f; //0.1
				color[v++] = 0.0f; //0.1
				color[v++] = 1.0f; //1

			}

			else 
			{
				// rouge  masse Pos
				color[v++] = 3.0f; //0.1 
				color[v++] = 0.0f; //0.9 
				color[v++] = 0.0f; //0.7 
				color[v++] = 0.0f; //1.0
			}
						
			c = c + 1;
        }
    }

}



#endif // __BODYSYSTEM_H__
