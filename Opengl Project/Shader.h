#pragma once

#include<string>
#include"coordinate.h"
#include"plane.h"
#include"camera.h"

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
	
	float modelMat[4][4];
	float viewMat[4][4];
	float projectionMat[4][4];
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

	void deallocate(float ** ptr) {
		//Free each sub-array
		for (int i = 0; i < 4; ++i) {
			delete[] ptr[i];
		}
		//Free the array of pointers
		delete[] ptr;
	}
	~Shader() {
		
	}

	void setMat(std::string name, float Mat[4][4]) {
		if (name == "model") {
			memcpy(modelMat, Mat, 4 * 4 * sizeof(float));



			
		}
		else if (name == "view") {
			memcpy(viewMat, Mat, 4 * 4 * sizeof(float));
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
		float resModel[4][1];
		
		matMulVec(modelMat, vec, resModel);
		// multiply with view matrix


			//1. get the view matrix from Camera class (  i.e Look At matrix)
			//mycamera.GetViewMatrix(viewMat);

				//show_matrix(viewMat);
				float resView[4][1];
		
			matMulVec(viewMat, resModel, resView);




		// multiply with projection matrix 

			float resProj[4][1];
			matMulVec(projectionMat, resModel, resProj);



		
		return coordinate3f(resProj[0][0], resProj[1][0], resProj[2][0]);
		//return vertex;


		//final co-ordinate = projection * view * model * [actual] point

	}


	
	plane_t getShadedPlane(plane_t plane) {

		coordinate3f v0;
		coordinate3f v1;
		coordinate3f v2;


		v0 = getShadedCoordinate(plane.v[0]);
		v1 = getShadedCoordinate(plane.v[1]);
		v2 = getShadedCoordinate(plane.v[2]);
		plane.v[0] = v0;
		plane.v[1] = v1;
		plane.v[2] = v2;


		return plane;

	}


};
