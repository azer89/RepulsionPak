
#ifndef SIMULATION_H
#define SIMULATION_H

#include "AVector.h"
#include "AMass.h"


class Simulation
{
public:
	int numOfMasses;								// number of masses in this container
	AMass** masses;									// masses are held by pointer to pointer. (Here Mass** represents a 1 dimensional array)

	Simulation()
	{
		this->numOfMasses = 0;
	}

	Simulation(int numOfMasses, float m)			// Constructor creates some masses with mass values m
	{
		this->numOfMasses = numOfMasses;

		masses = new AMass*[numOfMasses];			// Create an array of pointers

		for (int a = 0; a < numOfMasses; ++a)		// We will step to every pointer in the array
			masses[a] = new AMass(m);				// Create a Mass as a pointer and put it in the array
	}

	virtual void Release()							// delete the masses created
	{
		for (int a = 0; a < numOfMasses; ++a)		// we will delete all of them
		{
			delete(masses[a]);
			masses[a] = NULL;
		}

		delete(masses);
		masses = NULL;
	}

	AMass* GetMass(int index)
	{
		if (index < 0 || index >= numOfMasses) { return NULL; }
		return masses[index]; 
	}

	// this method will call the init() method of every mass
	virtual void Init()								
	{
		// no need
		for (int a = 0; a < numOfMasses; ++a) // We will init() every mass
			masses[a]->init();                  // call init() method of the mass
	}

	// no implementation because no forces are wanted in this basic container
	virtual void Solve()							
	{
		// in advanced containers, this method will be overrided and some forces will act on masses
	}

	// Iterate the masses by the change in time
	virtual void Simulate(float dt)
	{
		//for (int a = 0; a < numOfMasses; ++a) { masses[a]->simulate(dt); }
	}

	// The complete procedure of simulation
	virtual void Operate(float dt)					
	{
		Init();        // Step 1: reset forces to zero
		Solve();       // Step 2: apply forces
		Simulate(dt);  // Step 3: iterate the masses by the change in time
	}

};

/*
class PackingSimulation : public Simulation
{
public:
	AGraph _aGraph;

	PackingSimulation(AGraph aGraph)
	{
		this->_aGraph = aGraph;
	}
};
*/

class MotionUnderGravitation : public Simulation
{
public:
	AVector gravitation;												// the gravitational acceleration

	MotionUnderGravitation(AVector gravitation) : Simulation(1, 1.0f)	// Constructor firstly constructs its super class with 1 mass and 1 kg
	{																	// Vector3D gravitation, is the gravitational acceleration
		std::cout << "Error don't use this function :( \n";

		this->gravitation = gravitation;		

		/*masses[0]->_pos = AVector(0.0f, 0.0f);						    //set the position of the mass
		masses[0]->_vel = AVector(50.0f, 50.0f);*/						//set the velocity of the mass
	}

	//gravitational force will be applied therefore we need a "solve" method.
	virtual void Solve() 
	{
		std::cout << "Error don't use this function :( \n";
		//for (int a = 0; a < numOfMasses; ++a)                      //we will apply force to all masses (actually we have 1 mass, but we can extend it in the future)
		//	masses[a]->applyForce(gravitation * masses[a]->_m);    //gravitational force is as F = m * g. (mass times the gravitational acceleration)
	}

};


#endif