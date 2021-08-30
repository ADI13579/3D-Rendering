#pragma once
#include<string>
#include"coordinate.h"
#include"camera.h"
// function declaration
#define N 4
bool inverse(float A[N][N], float inverse[N][N]);
void transpose(float A[][N], float B[][N]);

#define N 4

//Camera mycamera(coordinate3f(10,5,8)); // later on ; this is to be made external object 




//void show_matrix(float mat[4][4]) {
//	// show matrix
//
//			
//			std::cout << "Result matrix is \n";
//			for (int i = 0; i < N; i++) {
//				for (int j = 0; j < N; j++)
//					std::cout << mat[i][j] << " ";
//				std::cout << "\n";
//			}
//			
//}



class Shader {
	
	float modelMat[4][4];// = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
	float viewMat[4][4];// = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
	float projectionMat[4][4];// = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
	float modelView[4][4];// = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
	float normalModelView[4][4];
	float scale[4][4] =  {{1.5*SCREEN_WIDTH,0,0,0},{0,1.5*SCREEN_HEIGHT,0,0},{0,0,1,0},{0,0,0,1}} ; // inorder to map to viewPort 
public:
	Shader() {


	}
	// This function multiplies
// mat1[][] and mat2[][], and
// stores the result in res[][]
	void matMulVec(float mat1[4][4],
		float mat2[4][1],
		float res[4][1])
	{
		int i, j, k;
		for (i = 0; i < N; i++) {
			for (j = 0; j < 1; j++) {
				res[i][j] = 0;

				for (k = 0; k < N; k++)
					res[i][j] += mat1[i][k] * mat2[k][j];

			}
		}
	}


	// This function multiplies
// mat1[][] and mat2[][], and
// stores the result in res[][]
	void matMulMat(float mat1[4][4],
		float mat2[4][4],
		float res[4][4])
	{
		int i, j, k;
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				res[i][j] = 0;

				for (k = 0; k < N; k++)
					res[i][j] += mat1[i][k] * mat2[k][j];

			}
		}
	}


	void setMat(std::string name, float Mat[4][4]) {
		if (name == "model") {
			memcpy(modelMat, Mat, 4 * 4 * sizeof(float));



			
		}
		else if (name == "view") {
			memcpy(viewMat, Mat, 4 * 4 * sizeof(float));

			matMulMat(viewMat, modelMat, modelView);
			//show_matrix(viewMat);
			//show_matrix(modelMat);
			//show_matrix(modelView);
			float invrs[4][4];
			inverse(modelView, invrs);
			//std::cout << "inverse" << std::endl;
			//show_matrix(invrs);
			// transpose
			transpose(invrs, normalModelView);
		}
		else if (name == "projection") {
			memcpy(projectionMat, Mat, 4 * 4 * sizeof(float));
		}


	}



// write a function to return the final matrix 
	// input: co-ordinate  , output:  matrix multiplied co-ordinate
	coordinate3f getShadedCoordinate(coordinate3f vertex) {
		//float** vec;
		// define and initialize point vec;
		float vec[4][1];
		



		vec[0][0] = vertex.x;
		vec[1][0] = vertex.y;
		vec[2][0] = vertex.z;
		vec[3][0] = 1;


		// multiply with the model matrix
		/*float resModel[4][1];*/
		
		//matMulVec(modelMat, vec, resModel);
		//// multiply with view matrix


		//	//1. get the view matrix from Camera class (  i.e Look At matrix)
		//	//mycamera.GetViewMatrix(viewMat);

		//		//show_matrix(viewMat);
		//		float resView[4][1];
		//		//show_matrix(viewMat);
		//	matMulVec(viewMat, resModel, resView);

			
			


			float modelViewVector[4][1];
			matMulVec(modelView, vec, modelViewVector);

		// multiply with projection matrix 

			float resProj[4][1];
			matMulVec(projectionMat, modelViewVector, resProj);

		coordinate3f temp = coordinate3f(resProj[0][0]/ float(resProj[3][0]), resProj[1][0]/ float(resProj[3][0]), resProj[2][0]/ float(resProj[3][0]));

			// get the view port co-ordinate;
			float view[4][1];
			float tmp[4][1];
			tmp[0][0] = temp.x;
			tmp[1][0] = temp.y;
			tmp[2][0] = temp.z;
			tmp[3][0] = 1;

			matMulVec(scale, tmp, view);
			
		
		/*std::cout << temp.x <<","<< temp.y << "," << temp.z;*/
		return coordinate3f(view[0][0], view[1][0], view[2][0]);
		//return coordinate3f(resProj[0][0] , resProj[1][0] , resProj[2][0] );

		
		//return vertex;


		//final co-ordinate = projection * view * model * [actual] point

	}


	coordinate3f getShadedNormal(coordinate3f normal) {

		// multiply normal with the inverse transpose  of modelview matrix;

				// changes coordinate3f into vector type
				float normalVector[4][1];
				normalVector[0][0] = normal.x;
				normalVector[1][0] = normal.y;
				normalVector[2][0] = normal.z;
				normalVector[3][0] = 1;


				//float newModelView[4][4];
				// get the inverse and transpose of modelView --->newModelView
				//float invrs[4][4];
				//inverse(modelView, invrs);
				//show_matrix(modelView);
				//show_matrix(invrs);
				//float trnspse[4][4];
				//transpose(invrs, trnspse);
				//can be somehow optimised here;
				//memcpy(newModelView, trnspse, 4 * 4 * sizeof(float));
				// remove the translation  part 
				//newModelView[0][3] = 0;
				//newModelView[1][3] = 0;
				//newModelView[2][3] = 0;



				float newNormalVector[4][1];
				//  modelView * normalVector = newNormalvector

				matMulVec(normalModelView, normalVector, newNormalVector);
				// newNormalVector  ----- > coordinate 3f



				return !coordinate3f(newNormalVector[0][0], newNormalVector[0][1], newNormalVector[0][2]);


	}

	
	plane_t getShadedPlane(plane_t plane) {

		coordinate3f v0;
		coordinate3f v1;
		coordinate3f v2;


		v0 = getShadedCoordinate(plane.v[0]);
		v1 = getShadedCoordinate(plane.v[1]);
		v2 = getShadedCoordinate(plane.v[2]);

		//std::cout << "oldVO" << plane.v[0].x << "new v0" << v0.x << std::endl;
		plane.v[0] = v0;
		plane.v[1] = v1;
		plane.v[2] = v2;

		// recalculate the new avg normal vector
		plane.vn[0] = getShadedNormal(plane.vn[0]);
		plane.vn[1] = getShadedNormal(plane.vn[1]);
		plane.vn[2] = getShadedNormal(plane.vn[2]);

		plane.makeCalculations();

		return plane;
	}


	void show_matrix(float mat[4][4]) {
		// show matrix


		std::cout << "Result matrix is \n";
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++)
				std::cout << mat[i][j] << " ";
			std::cout << "\n";
		}

	}


};

