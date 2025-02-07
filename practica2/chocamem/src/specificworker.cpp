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

 static int walkc = 0;
 static int spiralc = 0;
 static int circle = 0;
 static int inc = 0;
 static bool t = false;

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(MapPrx& mprx) : GenericWorker(mprx)
{
	setState(State::idle);
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
	
	RoboCompCommonBehavior::Parameter par = params.at("InnerModelPath");
	innerModel = std::make_shared<InnerModel>(par.value);
	xmin = std::stoi(params.at("xmin").value);
	xmax = std::stoi(params.at("xmax").value);
	ymin = std::stoi(params.at("ymin").value);
	ymax = std::stoi(params.at("ymax").value);
	tilesize = std::stoi(params.at("tilesize").value);

	// Scene
 	scene.setSceneRect(xmin, ymin, fabs(xmin)+fabs(xmax), fabs(ymin)+fabs(ymax));
 	view.setScene(&scene);
 	view.scale(1, -1);
 	view.setParent(scrollArea);
 	view.fitInView(scene.sceneRect(), Qt::KeepAspectRatio );
	grid.initialize( TDim{ tilesize, xmin, xmax, ymin, ymax}, TCell{true, false, nullptr} );

	qDebug() << "Grid initialize ok";

	for(auto &[key, value] : grid)
 	{
	 	value.rect = scene.addRect(-tilesize/2,-tilesize/2, 100,100, QPen(Qt::NoPen));			
		value.rect->setPos(key.x,key.z);
	}

 	robot = scene.addRect(QRectF(-200, -200, 400, 400), QPen(), QBrush(Qt::blue));
 	noserobot = new QGraphicsEllipseItem(-50,100, 100,100, robot);
 	noserobot->setBrush(Qt::magenta);

	view.show();
	return true;
}

void SpecificWorker::initialize(int period)
{
	std::cout << "Initialize worker" << std::endl;

	this->Period = period;
	timer.start(period);
	qDebug() << "End initialize";

}

void SpecificWorker::compute()
{

    readRobotState(ldata);

    switch(SpecificWorker::actual_state)
    {
        case State::idle:
            idle();
        break;
        case State::walk:
            walk();
        break;
        case State::turn:
            turn();
        break;
        case State::randTurn:
            randTurn();
        break;
		case State::spiral:
            spiral();
        break;
    }
}

void SpecificWorker::readRobotState(RoboCompLaser::TLaserData &ldata)
{
	try
	{
		differentialrobot_proxy->getBaseState(bState);
		innerModel->updateTransformValues("base", bState.x, 0, bState.z, 0, bState.alpha, 0);
		ldata = laser_proxy->getLaserData();
		
		//draw robot
		robot->setPos(bState.x, bState.z);
		robot->setRotation(-180.*bState.alpha/M_PI);

		//update occupied cells
		updateOccupiedCells(bState, ldata);
	}
	catch(const Ice::Exception &e)
	{
		std::cout << "Error reading from Laser" << e << std::endl;
	}
	//Resize world widget if necessary, and render the world
	if (view.size() != scrollArea->size())
	 		view.setFixedSize(scrollArea->width(), scrollArea->height());
}

void SpecificWorker::updateOccupiedCells(const RoboCompGenericBase::TBaseState &bState, const RoboCompLaser::TLaserData &ldata)
{
	InnerModelLaser *n = innerModel->getNode<InnerModelLaser>(QString("laser"));
	for(auto l: ldata)
	{
		auto r = n->laserTo(QString("world"), l.dist, l.angle);	// r is in world reference system
		// we set the cell corresponding to r as occupied 
		auto [valid, cell] = grid.getCell(r.x(), r.z()); 
		if(valid)
		{
			cell.free = false;
			cell.rect->setBrush(Qt::darkRed);
		}
	}

	auto [valid, cell] = grid.getCell(bState.x, bState.z);
	if(!(cell.visited)){
		cell.visited = true;
		cell.rect->setBrush(Qt::black);
	}

}

void SpecificWorker::setState(SpecificWorker::State a_state){
    SpecificWorker::actual_state = a_state;
} 

void SpecificWorker::idle(){
    setState(SpecificWorker::State::walk);
}

void SpecificWorker::walk()
{
    // Sort from min to max distance to objects or wall
    std::sort( ldata.begin(), ldata.end(), [](RoboCompLaser::TData a, RoboCompLaser::TData b){ return     a.dist < b.dist; });

    if(ldata.front().dist < threshold){
        t = true; 
        setState(SpecificWorker::State::turn); 
		      
    }
    else{
		iteration = 0;
		if(ldata.front().dist < 400){
            differentialrobot_proxy->setSpeedBase(700, 0);
            spiralc++;
			walkc++;
			
        } else {
			if(ldata.front().dist < 250){
            	differentialrobot_proxy->setSpeedBase(500, 0);
            	spiralc++;
				walkc++;
        	}else{
        		differentialrobot_proxy->setSpeedBase(1000, 0);
				spiralc++;
            	walkc++;
        }
		}

    }
	if(spiralc>30){
		spiralc = 0;
		setState(SpecificWorker::State::spiral);
	}
    if(walkc>40){
        walkc = 0;
        setState(SpecificWorker::State::randTurn);
    }
}

void SpecificWorker::turn()
{
	// Sort from max to min distance to objects or wall
    std::sort( ldata.begin(), ldata.end(), [](RoboCompLaser::TData a, RoboCompLaser::TData b){ return     a.dist > b.dist; });
	
	if(iteration == 0) {
		iteration = 1;
		if(ldata.front().angle > 0) turning = 1; // left
		else						turning = 2; // right
	}

    if (t == true){
		if(turning == 1){
			differentialrobot_proxy->setSpeedBase(0, rot); 
		}else{
			differentialrobot_proxy->setSpeedBase(0, -rot);  
		} 
		usleep(rand()%(1500000-100000+1) + 100000);
        t = false;
		spiralc = 0;
    }	
    else {
		setState(SpecificWorker::State::walk);
	}
        
} 

void SpecificWorker::spiral()
{
	if(ldata.front().dist < 400 || ldata[ldata.size()/2].dist < 400 || ldata.back().dist < 400){
		t = true;
		differentialrobot_proxy->setSpeedBase(100, 0);
		spiralc = 0;
		setState(SpecificWorker::State::turn);
	}
	else{
		if(ldata.front().dist > 400 && circle < 100){
			differentialrobot_proxy->setSpeedBase(200+inc, 0.5);
			circle++;
			inc = inc+50;
			if(inc >400){
				inc = 0;
			}
			
	 		setState(SpecificWorker::State::spiral);
		}
		else{
			circle = 0;
			inc = 0;
			setState(SpecificWorker::State::walk);
		}
		
	}
	 

}

void SpecificWorker::randTurn()
{
    static int giro = 0;
    giro    = rand()%10;

	if(giro==0){
    	differentialrobot_proxy->setSpeedBase(0, -rot);
	} else{
    	differentialrobot_proxy->setSpeedBase(0, rot);
	}
    setState(SpecificWorker::State::walk);
 }



///////////////////////////////////////////////////////////////////77
////  SUBSCRIPTION
/////////////////////////////////////////////////////////////////////

void SpecificWorker::RCISMousePicker_setPick(const Pick &myPick)
{
//subscribesToCODE

}



