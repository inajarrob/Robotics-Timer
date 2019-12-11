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
		std::string innermodel_path = par.value;
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
	try
	{
		differentialrobot_proxy->getBaseState(bState);
		innerModel->updateTransformValues("base", bState.x, 0, bState.z, 0, bState.alpha, 0);
	
	}
	catch(const Ice::Exception &e)
	{
		std::cout << "Error reading from Robot" << e << std::endl;
	}
 	try
	{
		ldata = laser_proxy->getLaserData(); 
	}
	catch(const Ice::Exception &e)
	{
		std::cout << "Error reading from lASER" << e << std::endl;
	}
}

bool SpecificWorker::GotoPoint_atTarget()
{
	auto x = abs(c.x - bState.x);
	auto z = abs(c.z - bState.z);
	auto d = sqrt((x*x) + (z*z));
	return (d<=150);
}

void SpecificWorker::GotoPoint_go(string nodo, float x, float y, float alpha)
{
	c.setCoords(x, y, alpha);
	differentialrobot_proxy->setSpeedBase(400, 0);
}

void SpecificWorker::GotoPoint_stop()
{
	differentialrobot_proxy->setSpeedBase(0, 0);
}

void SpecificWorker::GotoPoint_turn(float speed)
{
	if(speed > 1){
		speed = 1;
	} 
	if(speed < -1){
		speed = -1;
	}
	differentialrobot_proxy->setSpeedBase(0, speed);
}

void SpecificWorker::RCISMousePicker_setPick(Pick myPick)
{
	//subscribesToCODE
}


