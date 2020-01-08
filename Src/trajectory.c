#include "trajectory.h"
#include "inverse_kinematics.h"
#include "pid.h"
#include "timer.h"
#include <math.h>
#include <stdbool.h>

TRAJECTORY_TypeDef TRAJECTORY;

void TRAJECTORY_Init()
{
	TRAJECTORY.xCurrent = 0.0;
	TRAJECTORY.yCurrent = 0.0;
	TRAJECTORY.zCurrent = 0.0;

	TRAJECTORY.xTarget = 0.0;
	TRAJECTORY.yTarget = 0.0;
	TRAJECTORY.zTarget = 0.0;

	TRAJECTORY.xStart = 0,0;
	TRAJECTORY.yStart = 0.0;
	TRAJECTORY.zStart = 0.0;

	TRAJECTORY.xLength = 0.0;
	TRAJECTORY.yLength = 0.0;
	TRAJECTORY.zLength = 0.0;

	TRAJECTORY.velocity = 0.0;
	TRAJECTORY.totalTime = 0.0;
	TRAJECTORY.trackLength = 0.0;

	TRAJECTORY.timeStart = 0.0;
	TRAJECTORY.time = 0.0;

	TRAJECTORY.theta0 = 0.0;
	TRAJECTORY.theta1 = 0.0;
	TRAJECTORY.theta2 = 0.0;

	TRAJECTORY.busy = false;
}

void TRAJECTORY_SetTrajectory(TRAJECTORY_TypeDef* TRAJECTORY, float x0, float y0, float z0, float xk, float yk, float zk, float velocity)
{
	TRAJECTORY->xStart = x0;
	TRAJECTORY->yStart = y0;
	TRAJECTORY->zStart = z0;
	TRAJECTORY->xTarget = xk;
	TRAJECTORY->yTarget = yk;
	TRAJECTORY->zTarget = zk;
	TRAJECTORY->velocity = velocity;

	TRAJECTORY->xLength = (TRAJECTORY->xTarget - TRAJECTORY->xStart);
	TRAJECTORY->yLength = (TRAJECTORY->yTarget - TRAJECTORY->yStart);
	TRAJECTORY->zLength = (TRAJECTORY->zTarget - TRAJECTORY->zStart);
	TRAJECTORY->trackLength = sqrt(TRAJECTORY->xLength * TRAJECTORY->xLength + TRAJECTORY->yLength * TRAJECTORY->yLength + TRAJECTORY->zLength * TRAJECTORY->zLength);
	TRAJECTORY->totalTime = TRAJECTORY->trackLength/TRAJECTORY->velocity;
}

bool TRAJECTORY_StartTrajectory(TRAJECTORY_TypeDef* TRAJECTORY)
{
	TRAJECTORY->busy = true;
	TRAJECTORY->timeStart = TIMER_GetTime(&TIMER_0);
}

bool TRAJECTORY_IsBusy(TRAJECTORY_TypeDef* TRAJECTORY)
{
	return TRAJECTORY->busy;
}

bool TRAJECTORY_MakeStep(TRAJECTORY_TypeDef* TRAJECTORY)
{

TRAJECTORY->time = TIMER_GetTime(&TIMER_0) - TRAJECTORY->timeStart;

if(TRAJECTORY->busy)
{
	if(TRAJECTORY->time > TRAJECTORY->totalTime)
	{
		TRAJECTORY->busy = false;
		return false;
	}
	else
	{
		TRAJECTORY->xCurrent = TRAJECTORY->xStart + TRAJECTORY->xLength * (TRAJECTORY->time/TRAJECTORY->totalTime);
		TRAJECTORY->yCurrent = TRAJECTORY->yStart + TRAJECTORY->yLength * (TRAJECTORY->time/TRAJECTORY->totalTime);
		TRAJECTORY->zCurrent = TRAJECTORY->zStart + TRAJECTORY->zLength * (TRAJECTORY->time/TRAJECTORY->totalTime);

		if(delta_calcInverse(TRAJECTORY->xCurrent, TRAJECTORY->yCurrent, TRAJECTORY->zCurrent, &TRAJECTORY->theta0, &TRAJECTORY->theta1, &TRAJECTORY->theta2) == 0)
		{
			TRAJECTORY->minustheta0 = -TRAJECTORY->theta0;
			TRAJECTORY->minustheta1 = -TRAJECTORY->theta1;
			TRAJECTORY->minustheta2 = -TRAJECTORY->theta2;


			PID_SetRequired(&PID_0_Pos, -TRAJECTORY->theta0);
			PID_SetRequired(&PID_1_Pos, -TRAJECTORY->theta1);
			PID_SetRequired(&PID_2_Pos, -TRAJECTORY->theta2);

			return true;
		}
	}
}
else return false;
}
