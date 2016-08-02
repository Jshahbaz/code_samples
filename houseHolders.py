'''
Jared Shahbazian

Program to perform linear regression using Householders method for QR decomposition.

To run: Go to bottom and pick a csv to run regression on.

Output: the RMSE of the regression.

DISCLAIMER: TAKES VERY LONG - RUN ON A SMALLER SAMPLE TO TEST

'''

import random as r
import numpy as np
import csv
from numpy import matrix
import math

# house holder transformation
def houseHolders(X):
	#get dimensions n x d
	n = len(X)
	d = len(X.A[0])

	R = X
	Qaccumulated = matrix
	
	for i in range(d):
		print "performing the ",i,"th run in householders"
		
		zi = matrix(R.T.A[i][i:]).T

		Ei = matrix([0 for idx in range(len(zi))]).T
		Ei.A[0] = 1
	
		if(zi[0] > 0):
			vi = -(math.sqrt(zi.T*zi)*Ei) - zi
		else:
			vi = (math.sqrt(zi.T*zi)*Ei) - zi

		# find Pi
		numerator = 2*(vi * vi.T)
		denom = vi.T * vi
		quo = numerator/denom

		# need to create an identity matrix
		sizeI = len(quo)
		I = matrix([[0 for v in range(sizeI)] for v in range(sizeI)])
		for r in range(sizeI):
			for k in range(sizeI):
				if(r == k):
					I.A[r][k] = 1

		Pi = I - quo

		Qi = matrix([[np.float32(0) for w in range(n)] for u in range(n)])
		for r in range(n):
			for k in range(n):
				if(r == k):
					Qi.A[r][k] = 1

		#need to "merge" Qi and Pi
		for rows in range(len(Pi)):
			for cols in range(len(Pi.A[0])):
				Qi.A[rows+i][cols+i] = Pi.A[rows][cols]

		R = Qi * R
		if(i == 0):
			Qaccumulated = Qi
		else:
			Qaccumulated = Qi * Qaccumulated

	return R,Qaccumulated.T


#function to perform backsolving
# takes in R and QtransposeY matrices
# assumes R is upper right triangular
# outputs the beta vector
def backSolver(R, y):
	numRows = len(R)
	numCols = len(R.A[0])
	
	print "starting backsolver"

	beta = [np.float32(1) for i in range(numCols)]
	
	#get the first (last) betaN
	beta[numCols-1] = (y.A[0][numCols-1] / R.A[numCols-1][numCols-1])
	
	#use the known betas to discover new one
	#loop through each row, from bottom to top
	knownBs = 1
	row = numCols - 2 #start at n-2 because we want to use as index
	
	while row >= 0:
		currentRow = R.A[row]
		sum = 0
		
		for i in range(knownBs):
			currentCol = numCols-1-i
			sum += beta[currentCol] * currentRow[currentCol]
		
		rhs = y.A[0][row] - sum
		newBeta = rhs / currentRow[numCols - knownBs  - 1]
		beta[row] = newBeta
		knownBs =  knownBs + 1
		row = row - 1

	return beta


#function to split csv, run decomposition, backsolver, then find error
def driver(file):
	with open(file, 'rb') as csvfile:
		reader = csv.reader(csvfile , delimiter=',')
		data = np.array([[float(r) for r in row] for row in reader])

	np.random.seed(0)
	np.random.shuffle(data)
	train_num = int(data.shape[0] * 0.9)
	X_train = data[:train_num, :-1]
	Y_train = data[:train_num, -1]
	X_test = data[train_num: , :-1]
	Y_test = data[train_num: , -1]

	# linear least square Y = X beta
	R,Q = houseHolders(matrix(X_train))

	beta = backSolver(R, np.dot(Q.T,Y_train))

	print np.sqrt(np.mean((np.dot(X_test, beta) - Y_test) ** 2))


# RUNNER
# All of our test data sets
A =  "regression-A.csv"
B =  "regression-B.csv"
C =  "regression-C.csv"
fivePercent = "regression-0.05.csv"
fivePercentSmall = "small-5-percent.csv"

# Pick a file and run
driver(B)




