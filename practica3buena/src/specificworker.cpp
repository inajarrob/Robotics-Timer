/*
 *    Copyright (C)2019 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "specificworker.h"

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(TuplePrx tprx) : GenericWorker(tprx)
{
	actual_state = State::idle;
	s = -((log(a))/(b*b));
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
	std::cout << "Destroying SpecificWorker" << std::endl;
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
//       THE FOLLOWING IS JUST AN EXAMPLE
//	To use innerModelPath parameter you should uncomment specificmonitor.cpp readConfig method content
	try
	{
		RoboCompCommonBehavior::Parameter par = params.at("InnerModelPath");
		innerModel = std::make_shared<InnerModel>(par.value);
	}
		catch(const std::exception &e) { qFatal("Error reading config params"); }
	return true;
}

void SpecificWorker::initialize(int period)
{
	std::cout << "Initialize worker" << std::endl;
	this->Period = period;
	timer.start(Period);

}

void SpecificWorker::compute()
{
//computeCODE
//QMutexLocker locker(mutex);
//	try
//	{
//		camera_proxy->getYImage(0,img, cState, bState);
//		memcpy(image_gray.data, &img[0], m_width*m_height*sizeof(uchar));
//		searchTags(image_gray);
//	}
//	catch(const Ice::Exception &e)
//	{
//		std::cout << "Error reading from Camera" << e << std::endl;
//	}
	differentialrobot_proxy->getBaseState(bState);
	innerModel->updateTransformValues("base", bState.x, 0, bState.z, 0, bState.alpha, 0);
	ldata = laser_proxy->getLaserData();

	switch(actual_state){
		case State::idle:
			idle();
		break;
		case State::goToAndWalk:
			goToAndWalk();
		break;
		case State::turn:
			turn();
		break;
		case State::skirt:
			skirt();
		break;
	}
}

void SpecificWorker::idle()
{
	if(c.isActive())
	{
		SpecificWorker::actual_state = SpecificWorker::State::goToAndWalk;
	}
}

// Orientarse
void SpecificWorker::goToAndWalk(){
	// hasta que no este orientado al pick sigue girando	
	r = innerModel->transform("base", QVec::vec3(c.pick.x, 0, c.pick.z), "world");
		
	// hacer arcotangente para saber rotacion
	rot = atan2(r.x(),r.z());

	/*double gauss = exp(M_E, -exp(s*));
	forwardSpeed = 800**/

	// Sort from min to max distance to objects or wall
	auto v = ldata;
    std::sort(v.begin(), v.end(), [](RoboCompLaser::TData a, RoboCompLaser::TData b){ return     a.dist < b.dist; });

	if(v.front().dist < threshold)
		actual_state = State::turn;
	
	if(fabs(rot) > 1){
		// Robot orientado
		if(fabs(rot) < 0.05) {
			differentialrobot_proxy->setSpeedBase(0,0);
			//actual_state = State::walk;
			return;
		}
		differentialrobot_proxy->setSpeedBase(0,rot);
	} else{
		if(checkInTarget()){
			differentialrobot_proxy->setSpeedBase(0,0);
			c.active.store(false);
			actual_state = State::idle;
		} else{
			if(d > 600)
				differentialrobot_proxy->setSpeedBase(400, rot);
			else
				differentialrobot_proxy->setSpeedBase(d, rot);
			}
	}
}

bool SpecificWorker::checkInTarget(){
	auto x = abs(c.pick.x - bState.x);
	auto z = abs(c.pick.z - bState.z);
	d = sqrt((x*x) + (z*z));
	cout << "DISTANCIA: " << d << endl;
	return (d<=100);
}

void SpecificWorker::turn(){
	auto v = ldata;
	std::sort(v.begin(), v.end(), [](RoboCompLaser::TData a, RoboCompLaser::TData b){ return     a.dist < b.dist; });

	differentialrobot_proxy->setSpeedBase(0, 0.2);
		cout << fabs(v[0].angle) << endl;
	if((fabs(v[0].angle) >= 1.45) && (fabs(v[0].angle) <= 1.60)){
		actual_state = State::skirt;
		cout << "EH LETS GO" << endl;
	}
	
}

void SpecificWorker::skirt(){
	// tenemos que bordear el objeto que tenemos a la izda o la dcha
	auto v = ldata;
	std::sort(v.begin()+50, v.end(), [](RoboCompLaser::TData a, RoboCompLaser::TData b){ return     a.dist < b.dist; });
	cout << v[50].dist << endl;
	if(v[50].dist < 400){
		differentialrobot_proxy->setSpeedBase(100, 0);
	} else {
		if(targetVisible()){
			actual_state = State::goToAndWalk;
		}
		differentialrobot_proxy->setSpeedBase(0, -0.2);
	}
	
}


/*
void SpecificWorker::walk(){	
	if(checkInTarget()){
		differentialrobot_proxy->setSpeedBase(0,0);
		c.active.store(false);
		actual_state = State::idle;
	} else{
		if(d < 500)
			differentialrobot_proxy->setSpeedBase(500,0);
	}
}
void SpecificWorker::goTo(){
	// hasta que no este orientado al pick sigue girando	
	r = innerModel->transform("base", QVec::vec3(c.pick.x, 0, c.pick.z), "world");
	
	// hacer arcotangente para saber rotacion
	double rot = atan2(r.x(),r.z());

	// Robot orientado
	if(fabs(rot) < 0.05) {
		differentialrobot_proxy->setSpeedBase(0,0);
		actual_state = State::walk;
		return;
	}
	differentialrobot_proxy->setSpeedBase(0,rot);
}
*/


void SpecificWorker::RCISMousePicker_setPick(Pick myPick)
{
//subscribesToCODE
	c.setCoords(myPick);
}


/* HAY QUE MODIFICAR ESTADOS Y HACER UN ORIENTAR Y AVANZAR A LA VEZ (FOTO ISA EJEM) 
*/