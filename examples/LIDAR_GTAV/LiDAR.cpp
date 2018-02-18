#include "LiDAR.h"
//#include "Scenario.h"
#include <math.h>
#include <stdio.h>



#define PI 3.1415926535898
#define D2R PI/180.0

//#define DEBUG_CONFIG
//#define DEBUG_LOG
#define DEBUG_GRAPHICS_LIDAR
//#define SAVE_POINTCLOUD


LiDAR::LiDAR() {
	m_pPointClouds = NULL;
	m_maxRange = 0;
	m_vertiUpLimit = 0;
	m_vertiUnLimit = 0;
	m_horizLeLimit = 0;
	m_horizRiLimit = 0;
	m_vertiSmplNum = 0;
	m_horizSmplNum = 0;
	m_vertiResolu = 0;
	m_horizResolu = 0;
	m_camera = 0;
	m_ownCar = 0;
	m_initType = _LIDAR_NOT_INIT_YET_;
	m_isAttach = false;
}

LiDAR::~LiDAR() {
	DestroyLiDAR();
}

void LiDAR::Init2DLiDAR_SmplNum(float maxRange, int horizSmplNum, float horizLeLimit, float horizRiLimit) {
	if (m_initType != _LIDAR_NOT_INIT_YET_)
		DestroyLiDAR();

	m_horizSmplNum = horizSmplNum;
	m_maxRange = maxRange;
	if (horizRiLimit <= horizLeLimit) {
		printf("\nHorizontal FOV angle parameters error");
		return;
	}
	m_horizLeLimit = horizLeLimit;
	m_horizRiLimit = horizRiLimit;
	m_horizResolu = (m_horizLeLimit + 360.0 - m_horizRiLimit) / m_horizSmplNum;
	if (!m_pPointClouds)
		m_pPointClouds = (float *)malloc(m_horizSmplNum * sizeof(float));
	if (m_pPointClouds == NULL)
		printf("\nLiDAR: memory alloc err");

	m_initType = _LIDAR_INIT_AS_2D_;

#ifdef DEBUG_CONFIG
	printf("\nDEBUG_CONFIG: function: %s", __FUNCTION__);
	printf("\ncamera=%d, ownCar=%d, maxRange=%f, horizSmplNum=%d, horizLeLimit=%f, horizRiLimit=%f",
		m_camera, m_ownCar, m_maxRange, m_horizSmplNum, m_horizLeLimit, m_horizRiLimit);
	printf("\nHorizontal FOV(yaw definition): %f to %f", -m_horizLeLimit, 360.0 - m_horizRiLimit);
	printf("\nHorizontal angel resolution(deg): %f", m_horizResolu);
	printf("\n");
#endif // DEBUG_CONFIG
}

void LiDAR::Init3DLiDAR_SmplNum(float maxRange, int horizSmplNum, float horizLeLimit, float horizRiLimit,
	int vertiSmplNum, float vertiUpLimit, float vertiUnLimit) {

	if (m_initType != _LIDAR_NOT_INIT_YET_)
		DestroyLiDAR();

	m_vertiSmplNum = vertiSmplNum;
	m_horizSmplNum = horizSmplNum;
	m_maxRange = maxRange;
	//Vertical:
	if (vertiUnLimit <= vertiUpLimit) {
		printf("\nVertical FOV angle parameters error");
		return;
	}
	m_vertiUpLimit = vertiUpLimit;
	m_vertiUnLimit = vertiUnLimit;
	m_vertiResolu = (m_vertiUnLimit - m_vertiUpLimit) / m_vertiSmplNum;

	//Horizontal: 
	if (horizRiLimit <= horizLeLimit) {
		printf("\nHorizontal FOV angle parameters error");
		return;
	}
	m_horizLeLimit = horizLeLimit;
	m_horizRiLimit = horizRiLimit;
	m_horizResolu = (m_horizLeLimit + 360.0 - m_horizRiLimit) / m_horizSmplNum;

	if (!m_pPointClouds)
		m_pPointClouds = (float *)malloc(m_vertiSmplNum * m_horizSmplNum * sizeof(float));

	if (m_pPointClouds == NULL)
		printf("\nLiDAR: memory alloc err");

	m_initType = _LIDAR_INIT_AS_3D_;

#ifdef DEBUG_CONFIG
	printf("\nDEBUG_CONFIG: function: %s", __FUNCTION__);
	printf("\ncamera=%d, ownCar=%d, maxRange=%f, horizSmplNum=%d, horizLeLimit=%f, horizRiLimit=%f, vertiSmplNum=%d, vertiUpLimit=%f, vertiUnLimit=%f",
		m_camera, m_ownCar, m_maxRange, m_horizSmplNum, m_horizLeLimit, m_horizRiLimit, m_vertiSmplNum, m_vertiUpLimit, m_vertiUnLimit);
	printf("\nHorizontal FOV(yaw definition): %f to %f", -m_horizLeLimit, 360.0 - m_horizRiLimit);
	printf("\nVertical FOV(pitch definition): %f to %f", 90.0 - m_vertiUpLimit, 90.0 - m_vertiUnLimit);
	printf("\nHorizontal angel resolution(deg): %f", m_horizResolu);
	printf("\nVertical angel resolution(deg): %f", m_vertiResolu);
	printf("\n");
#endif // DEBUG_CONFIG
}

void LiDAR::Init2DLiDAR_FOV(float maxRange, float horizFOV, float horizAngResolu) {
	Init2DLiDAR_SmplNum(maxRange, horizFOV / horizAngResolu, horizFOV / 2, 360.0 - horizFOV / 2);
}

void LiDAR::Init3DLiDAR_FOV(float maxRange, float horizFOV, float horizAngResolu, float vertiFOV, float vertiAngResolu) {
	Init3DLiDAR_SmplNum(maxRange, horizFOV / horizAngResolu, horizFOV / 2, 360.0 - horizFOV / 2, vertiFOV / vertiAngResolu, 90.0 - vertiFOV / 2, 90.0 + vertiFOV / 2);
}

void LiDAR::AttachLiDAR2Camera(Cam camera, Entity ownCar) {
	if (!m_isAttach) {
		m_camera = camera;
		m_ownCar = ownCar;
		m_isAttach = true;
	}
	else
		printf("\nThe LiDAR has been attached to an entity");
}

void LiDAR::DestroyLiDAR() {

	if (m_pPointClouds) {
		free(m_pPointClouds);
		m_pPointClouds = NULL;
	}

	m_maxRange = 0;
	m_vertiUpLimit = 0;
	m_vertiUnLimit = 0;
	m_horizLeLimit = 0;
	m_horizRiLimit = 0;
	m_vertiSmplNum = 0;
	m_horizSmplNum = 0;
	m_vertiResolu = 0;
	m_horizResolu = 0;
	m_camera = 0;
	m_ownCar = 0;
	m_initType = _LIDAR_NOT_INIT_YET_;
	m_isAttach = false;
}

float * LiDAR::GetPointClouds() {
	if (m_pPointClouds == NULL || m_initType == _LIDAR_NOT_INIT_YET_ || !m_isAttach)
		return NULL;

	switch (m_initType) {
	case _LIDAR_INIT_AS_2D_: GenerateHorizPointClouds(90, m_pPointClouds);
	case _LIDAR_INIT_AS_3D_:
	{
		float phi = m_vertiUnLimit;

		for (int k = 0; k < m_vertiSmplNum; k++) {
			if (phi > m_vertiUpLimit - m_vertiResolu)
				phi = m_vertiUnLimit - k * m_vertiResolu;
			else
				break;

			GenerateHorizPointClouds(phi, &m_pPointClouds[k * m_horizSmplNum]);
		}
	}
	default:
		break;
	}

	return m_pPointClouds;
}

int LiDAR::getTotalSmplNum() {
	switch (m_initType) {
	case _LIDAR_INIT_AS_2D_:
		return m_horizSmplNum;
	case _LIDAR_INIT_AS_3D_:
		return m_horizSmplNum * m_vertiSmplNum;
	default:
		return 0;
	}
}

int LiDAR::getVertiSmplNum() {
	return m_vertiSmplNum;
}

int LiDAR::getHorizSmplNum() {
	return m_horizSmplNum;
}

int LiDAR::getCurType() {
	return m_initType;
}

void LiDAR::GenerateSinglePoint(float phi, float theta, float* p) {
	BOOL isHit;
	Entity hitEntity;
	Vector3 target, endCoord, surfaceNorm;

	int raycast_handle;
	float phi_rad = phi * D2R;
	float theta_rad = theta * D2R;

	endCoord.x = -m_maxRange * sin(phi_rad) * sin(theta_rad);	//rightward(east) is positive
	endCoord.y = m_maxRange * sin(phi_rad) * cos(theta_rad);	//forward(north) is positive
	endCoord.z = m_maxRange * cos(phi_rad);						//upward(up) is positive

	target.x = m_rotDCM[0] * endCoord.x + m_rotDCM[1] * endCoord.y + m_rotDCM[2] * endCoord.z + m_curPos.x;
	target.y = m_rotDCM[3] * endCoord.x + m_rotDCM[4] * endCoord.y + m_rotDCM[5] * endCoord.z + m_curPos.y;
	target.z = m_rotDCM[6] * endCoord.x + m_rotDCM[7] * endCoord.y + m_rotDCM[8] * endCoord.z + m_curPos.z;

	//options: -1 = everything
	raycast_handle = WORLDPROBE::_CAST_RAY_POINT_TO_POINT(m_curPos.x, m_curPos.y, m_curPos.z, target.x, target.y, target.z, -1, m_ownCar, 7);

	WORLDPROBE::_GET_RAYCAST_RESULT(raycast_handle, &isHit, &endCoord, &surfaceNorm, &hitEntity);

	//if (isHit)
	//	*p = sqrt((endCoord.x - m_curPos.x) * (endCoord.x - m_curPos.x) +
	//	(endCoord.y - m_curPos.y) * (endCoord.y - m_curPos.y) +
	//		(endCoord.z - m_curPos.z) * (endCoord.z - m_curPos.z));
	//else
	//	*p = m_maxRange;

#ifdef DEBUG_LOG
	printf("\nDEBUG_LOG: function: %s", __FUNCTION__);
	printf("\ntheta=%f, endcoord:x=%f, y=%f, z=%f", __FUNCTION__, theta, endCoord.x, endCoord.y, endCoord.z);
#endif //DEBUG_LOG

#ifdef DEBUG_GRAPHICS_LIDAR
	//GRAPHICS::DRAW_BOX(target.x - 0.05, target.y - 0.05, target.z - 0.05, target.x + 0.05, target.y + 0.05, target.z + 0.05, 255, 0, 0, 255);
	GRAPHICS::DRAW_LINE(endCoord.x - 0.03, endCoord.y - 0.03, endCoord.z - 0.03, endCoord.x + 0.03, endCoord.y + 0.03, endCoord.z + 0.03,  255,255,255, 255);
#endif //DEBUG_GRAPHICS_LIDAR

#ifdef SAVE_POINTCLOUD
	fout << endCoord.x << ", " << endCoord.y << ", " << endCoord.z << std::endl;
#endif
}

void LiDAR::GenerateHorizPointClouds(float phi, float *p) {
	int i, j;
	float theta = 0.0, quaterion[4];

	m_curPos = CAM::GET_CAM_COORD(m_camera);
	
	// ed: code added
	m_curPos.z += 2.0f;

	calcDCM();

	//Right side:
	theta = m_horizRiLimit;
	for (j = 0; j < m_horizSmplNum; j++) {
		if (theta < 360.0 - m_horizResolu)
			theta = m_horizRiLimit + j * m_horizResolu;
		else
			break;
		GenerateSinglePoint(phi, theta, p + j);
	}

	//Left side:
	theta = theta - 360.0;
	for (i = 0; i < m_horizSmplNum - j; i++) {
		if (theta < m_horizLeLimit - m_horizResolu)
			theta = 0.0 + i * m_horizResolu;
		else
			break;
		GenerateSinglePoint(phi, theta, p + i + j);
	}
}

void LiDAR::calcDCM() {
	ENTITY::GET_ENTITY_QUATERNION(m_ownCar, &m_quaterion[0], &m_quaterion[1], &m_quaterion[2], &m_quaterion[3]);
	//m_quaterion: R - coord spins to b - coord
	float q00 = m_quaterion[3] * m_quaterion[3], q11 = m_quaterion[0] * m_quaterion[0], q22 = m_quaterion[1] * m_quaterion[1], q33 = m_quaterion[2] * m_quaterion[2];
	float q01 = m_quaterion[3] * m_quaterion[0], q02 = m_quaterion[3] * m_quaterion[1], q03 = m_quaterion[3] * m_quaterion[2], q12 = m_quaterion[0] * m_quaterion[1];
	float q13 = m_quaterion[0] * m_quaterion[2], q23 = m_quaterion[1] * m_quaterion[2];

	//convert b-vector to R-vector, CbR
	m_rotDCM[0] = q00 + q11 - q22 - q33;
	m_rotDCM[1] = 2 * (q12 - q03);
	m_rotDCM[2] = 2 * (q13 + q02);
	m_rotDCM[3] = 2 * (q12 + q03);
	m_rotDCM[4] = q00 - q11 + q22 - q33;
	m_rotDCM[5] = 2 * (q23 - q01);
	m_rotDCM[6] = 2 * (q13 - q02);
	m_rotDCM[7] = 2 * (q23 + q01);
	m_rotDCM[8] = q00 - q11 - q22 + q33;
}


void LiDAR::InitFOUT() {
	fout.open("pointcloud.dat", std::fstream::in | std::fstream::out | std::fstream::app);
}

