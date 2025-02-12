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

/**
       \brief
       @author authorname
*/



#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H

#include <genericworker.h>
#include <innermodel/innermodel.h>
#include <stdlib.h>
#include <QVector>
#include <math.h>
#include <Qt>
#include<QLineF>
const int threshold = 200; // 300 milimeters
const float a = 0.5;
const float b = 0.5;

class SpecificWorker : public GenericWorker
{
Q_OBJECT
public:
	SpecificWorker(TuplePrx tprx);
	~SpecificWorker();
	bool setParams(RoboCompCommonBehavior::ParameterList params);


	void RCISMousePicker_setPick(Pick myPick);
	enum class State {idle, goToAndWalk, turn, skirt};
	SpecificWorker::State actual_state;
	RoboCompGenericBase::TBaseState bState;
	RoboCompLaser::TLaserData ldata;
	QVec r;

	struct Coords
	{
		Pick pick;
		QMutex mutex;
		std::atomic<bool> active = false;
		float a;
		float b;
		float n;

		Pick getCoords(){
			QMutexLocker ml(&mutex);
			return pick;
		}
		void setCoords(Pick p, const RoboCompGenericBase::TBaseState &bState, const QVec &r2){
			QMutexLocker ml(&mutex);
			pick = p;
			active.store(true);
			a = r2.x() - bState.x;
			b = -(r2.z() - bState.z);
			n = -(a*bState.z) - (b*bState.x);
		}
		bool isActive()
		{
			//active=true si tenemos pick
			return active.load();
		}
	};
	Coords c;
	double rot;
	double d;
	double s;
	double forwardSpeed;
	bool turning = false;
	bool tVisible = false;


public slots:
	void compute();
	void initialize(int period);
private:
	std::shared_ptr<InnerModel> innerModel;
	bool checkInTarget();
	void idle();
	void goToAndWalk();
	void turn();
	void skirt();
	bool targetVisible();
	bool inLine();
	bool withoutObject(RoboCompLaser::TLaserData ld);


	bool GotoPoint_atTarget();
	void GotoPoint_go(string nodo, float x, float y, float alpha);
	void GotoPoint_stop();
	void GotoPoint_turn(float speed);
};

#endif
