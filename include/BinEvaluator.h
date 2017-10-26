
/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
 * 
*/

#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <AgileMap.h>
#include <fitsio.h>
#include <stdio.h>
#include <iostream>

 
using namespace std;

class BinEvaluator {
	public:
 
		BinEvaluator(const char * fitsFilePath, double ** image, double l, double b, double radius);
		BinEvaluator(const char * fitsFilePath, double l, double b, double radius);

		int sumBin();
		bool isRadiusInside();

		const char * fitsFilePath;
		double l, b, radius;
		int x, y;
		double binSum;
		double tmin;
		double tmax;
		AgileMap * agileMapUtils;

 

 		bool convertFitsDataToMatrix(); 
		int rows;
		int cols;
		double ** image;
 

	 

		
		
};

#endif