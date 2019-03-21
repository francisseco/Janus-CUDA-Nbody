#ifndef __TIPSY_H__
#define __TIPSY_H__

#include <string>

using namespace std;

#define MAXDIM 3

typedef float Real;

struct gas_particle
{
    Real mass;
    Real pos[MAXDIM];
    Real vel[MAXDIM];
    Real rho;
    Real temp;
    Real hsmooth;
    Real metals ;
    Real phi ;
} ;

//struct gas_particle *gas_particles;

struct dark_particle
{
    Real mass;
    Real pos[MAXDIM];
    Real vel[MAXDIM];
    Real eps;
    int phi ;
} ;

//struct dark_particle *dark_particles;

struct star_particle
{
    Real mass;
    Real pos[MAXDIM];
    Real vel[MAXDIM];
    Real metals ;
    Real tform ;
    Real eps;
    int phi ;
} ;

//struct star_particle *star_particles;

struct dump
{
    double time ;
    int nbodies ;
    int ndim ;
    int nsph ;
    int ndark ;
    int nstar ;
} ;

typedef struct dump header ;

template <typename real4>
void read_tipsy_file(vector<real4> &bodyPositions,
                     vector<real4> &bodyVelocities,
                     vector<int> &bodiesIDs,
                     const std::string &fileName,
                     int &NTotal,
                     int &NFirst,
                     int &NSecond,
                     int &NThird)
{
    /*
       Read in our custom version of the tipsy file format written by
       Jeroen Bedorf.  Most important change is that we store particle id on the
       location where previously the potential was stored.
    */

    char fullFileName[256];
    sprintf(fullFileName, "%s", fileName.c_str());

    cout << "Trying to read file: " << fullFileName << endl;

	ifstream inputFile(fullFileName, ios::in | ios::binary);

	//////

	//std::ifstream source;                    // build a read-Stream
	//source.open(fullFileName, std::ios_base::in);  // open data
	//////

    if (!inputFile.is_open())
    {
        cout << "Can't open input file \n";
        exit(EXIT_SUCCESS);
    }

    dump  h;
    inputFile.read((char *)&h, sizeof(h));

    int idummy;
    real4 positions;
    real4 velocity;


    //Read tipsy header
    NTotal        = h.nbodies;
    NFirst        = h.ndark;
    NSecond       = h.nstar;
    NThird        = h.nsph;
	cerr << "NTotal " << NTotal << " NFirst " << NFirst << "NSecond " << NSecond << "NThird "<< NThird << endl;
    //Start reading
    int particleCount = 0;

	//Negative mass particle
    dark_particle d;
	//positive mass particle
    star_particle s;
	
	
	///////////////
	//int pId = 100000000;
	//int debg = 0;
	///////////Lecture fichier INPUT///////////
	//for (std::string line; std::getline(source, line); )   //read stream line by line
	//{
	//std::istringstream in(line);      //make a stream for the line itself
	//Real m;
	//Real x;
	//Real y;
	//Real z;
	//Real vx;
	//Real vy;
	//Real vz;
	//in >> x >> y >> z >> m >> vx >> vy >> vz;
	//////INSERT CODE
	//velocity.w = 0.0f;
	////if (fabs(z) > 1.5)
		//{
		//	positions.w = -m;
		//}
		//else
		//{
		//	positions.w = m;
		//}
		//cerr << m << endl;
		//positions.x = x;
		//positions.y = y;
		//positions.z = z;
		//velocity.x = 0.0f;
		//velocity.y = 0.0f;
		//velocity.z = 0.0f;
		//velocity.x = vx;
		//velocity.y = vy;
		//velocity.z = vz;// CMD tells us velocity variations at the origins at like, 1 to the 100000. so let's not bug this too mucho
		//idummy = pId;
		//bodyPositions.push_back(positions);
		//bodyVelocities.push_back(velocity);
		//bodiesIDs.push_back(idummy);
		//pId++;
		//if (debg < 10)
		//{
		//	cerr << "Indice debg For " << debg << endl;
		//	cerr << "Star s.eps " << velocity.w << endl;
		//	cerr << "Star idummy " << idummy << endl;
		//	debg++;
		//}

		///////
		//particlepos.push_back({ x,y,z,m });
		//original: 
		//particlevel.push_back({ vx,vy,vz,0.0 });
		//No veloc:
		//particlevel.push_back({ 0.0,0.0,0.0,0.0 });
		//}
	//////////////////////




	for (int i = 0; i < NTotal; i++)
	{		


		if (i < NFirst)
		{
			inputFile.read((char *)&d, sizeof(d));
			velocity.w = d.eps;
			positions.w = -d.mass/5.1230f;
			//positions.x = d.pos[0];
			//positions.y = d.pos[1];
			//positions.z = d.pos[2];
			float x, y, z;
			x = rand() / (float)RAND_MAX * 40 - 20;
			y = rand() / (float)RAND_MAX * 40 - 20;
			z = rand() / (float)RAND_MAX * 40 - 20;
			float3 point = { x, y, z };
			positions.x = point.x;
			positions.y = point.y;
			positions.z = point.z;
			velocity.x = 0.0f;//d.vel[0];
			velocity.y = 0.0f;//d.vel[1];
			velocity.z = 0.0f;//d.vel[2];
			idummy = d.phi;
		}
		else
		{
			inputFile.read((char *)&s, sizeof(s));
			velocity.w = s.eps;
			positions.w = s.mass;
			//positions.x = s.pos[0];
			//positions.y = s.pos[1];
			//positions.z = s.pos[2];
			float x, y, z;
			x = rand() / (float)RAND_MAX * 13 - 6.5;
			y = rand() / (float)RAND_MAX * 13 - 6.5;
			z = rand() / (float)RAND_MAX * 13 - 6.5;
			float3 point = { x, y, z };
			positions.x = point.x;
			positions.y = point.y;
			positions.z = point.z;
			velocity.x = 0.0f;//s.vel[0];
			velocity.y = 0.0f;//s.vel[1];
			velocity.z = 0.0f;//s.vel[2];
			idummy = s.phi;
		}

		bodyPositions.push_back(positions);
		bodyVelocities.push_back(velocity);
		bodiesIDs.push_back(idummy);

		particleCount++;
	}//end for

    // round up to a multiple of 256 bodies since our kernel only supports that...
    //////////////
		int newTotal = NTotal;
		//int newTotal = pId - 100000000;
		//NTotal = newTotal;

    if (NTotal % 256)//256 original value
    {
        newTotal = ((NTotal / 256) + 1) * 256;
		cerr << "debug if modulo 256" << endl;
    }
	//pId remplace NTOtal
    for (int i = NTotal; i < newTotal; i++)
    {
	   positions.w = positions.x = positions.y = positions.z = 0;
	    positions.w = positions.x = positions.y = positions.z = 0;
	    velocity.x = velocity.y = velocity.z = 0;
	    bodyPositions.push_back(positions);
	    bodyVelocities.push_back(velocity);
	    bodiesIDs.push_back(i);
	    NFirst++;
	}

    //pId = newTotal - 100000000;
	NTotal = newTotal;

    inputFile.close();    
	
	//NFirst        = 0;
    //NSecond       = 0;
    //NThird        = 0;

    cerr << "Read " << NTotal << " bodies" << endl;
}

#endif //__TIPSY_H__
