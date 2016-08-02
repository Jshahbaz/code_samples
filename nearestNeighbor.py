'''
Jared Shahbazian

Program to perform k nearest neighbor on csv data sets. Nearest neighbor 
is a tool for prediction based on similiar observations. The program splits data
into test and training sets in order to get unbiased results. 

To run: Go to bottom and pick a test file to run, or insert your own csv

Output: the k value (either 1,3,5,7,9) that maximized the test correctness, as
well as a confusion matrix that shows the accuracy of the prediction.

DISCLAIMER: Will take long to execute on large data sets.
'''

import random as r
import numpy as np
import matplotlib.pyplot as plt
import csv
from scipy.spatial import distance
import operator

# what size sample do we want
sampleSize = 0.10

#function to correct abalone data and create proxy variables
def proxy(row):
	letter = row[0]
	row[0] = 0
	row.insert(0,0)
	row.insert(0,0)
	
	if(letter == 'M'):
		row[0] = 1
	if(letter == 'F'):
		row[1] = 1
	if(letter == 'I'):
		row[2] = 1

	return row

#function to split data into training and test sets
def split(file):
	matrix = []
	#fill matrix of observations (which is a list of lists)
	with open(file, 'rb') as f:
		obs = csv.reader(f)
		for row in obs:
			if(file == "abalone.data.txt"):
				row = proxy(row)
			i = 0
			for i in range(len(row)):
				row[i] = float(row[i])
			matrix.append(row)
		
		size =  obs.line_num

	#now start procedure to seperate test and training data
	r.seed(38)
	testData = []
	trainingData = []
	needed = sampleSize * size
	remaining = size
	prob = needed / remaining
	#loop through the rows, checking against threshhold
	k=0
	for k in range(size):
		rand = r.random()
		if(rand < prob):
			needed = needed - 1
			testData.append(matrix[k])
		else:
			trainingData.append(matrix[k])
		#Update probability
		remaining = remaining - 1
		if remaining != 0 :
			prob = needed / remaining

	return (testData,trainingData,matrix)


#function to put rows into columns
def columnize(test, training):
	#get training set into columns
	trainingCols = []
	col = []
	i = 0
	for i in range(len(training[0])):
		x = 0
		for x in range(len(training)):
			col.append(training[x][i])
		
		trainingCols.append(col)
		col = []
	
	#get test set into columns
	testCols = []
	col = []
	i = 0
	for i in range(len(test[0])):
		x = 0
		for x in range(len(test)):
			col.append(float(test[x][i]))
		
		testCols.append(col)
		col = []

	return (trainingCols,testCols)

#function to perform z scaling
def zscale(data,means,stds):
	row = 0
	for row in range(len(data)):
		column = 0
		for column in range(len(data[0])-1):
			x = data[row][column]
			xbar = means[column]
			sigma = stds[column]

			data[row][column] = (x-xbar)/sigma

	return data

#function to use neighbors to estimate y col, then check if prediction was
#   accurate or not and update confusion matrix accordingly
def run(k,neighborsOfEachRow,test,file):
	correct = 0
	wrong = 0

	#fill confusion matrix
	if(file == "abalone.data.txt"):
		confusionMatrix = [[0 for i in range(29)] for i in range(29)]
	else:
		confusionMatrix = [[0 for i in range(2)] for i in range(2)]

	for row in range(len(neighborsOfEachRow)):
		estimates = []
		for x in range(k):
			estimates.append(neighborsOfEachRow[row][x][0][-1])
		
		guess = max(set(estimates), key=estimates.count)
					 
		#check if guess was right
		if(guess == test[row][-1]):
			correct = correct + 1
		else:
			wrong = wrong + 1
	
		item1 = int(test[row][-1])
		item2 = int(guess)

		#add to confusion matrix
		confusionMatrix[item1][item2] = confusionMatrix[item1][item2] + 1

	errRate = wrong / float(len(test))
	
	return (errRate,k,confusionMatrix)
					 
					 
#function to do actual nearest neighbor testing and output accuracy
def estimate(test,training,file):
	neighborsOfEachRow = []
	#for each row in test, find the k nearest neighbors
	for row in range(len(test)):
		dists = []
		secondToLast = len(test[row]) - 2
		for x in range(len(training)):
			dist = distance.euclidean(test[row][0:secondToLast],training[x][0:secondToLast])
			dists.append((training[x],dist))
		dists.sort(key=operator.itemgetter(1))
		neighbors = []
		for x in range(9):
			neighbors.append(dists[x])
		
		neighborsOfEachRow.append(neighbors)

	samples = []
	samples.append(run(1,neighborsOfEachRow,test,file))
	samples.append(run(3,neighborsOfEachRow,test,file))
	samples.append(run(5,neighborsOfEachRow,test,file))
	samples.append(run(7,neighborsOfEachRow,test,file))
	samples.append(run(9,neighborsOfEachRow,test,file))

	best = samples[0]
	for i in range(len(samples)-1):
		if(samples[i+1][0] < samples[i][0]):
			best = samples[i+1]

	return best
		


#function to run test n times
def knn(file):
	#get sets using split()
	(test,training,matrix) = split(file)
	
	#put into columns
	(trainingCols,testCols) = columnize(test,training)
	
	numColumns = len(trainingCols)
	
	#get mean and std
	means = []
	stds = []
	for i in range(numColumns):
		means.append(np.mean(trainingCols[i]))
		stds.append(np.std(trainingCols[i]))

	test = zscale(test,means,stds)
	training = zscale(training,means,stds)
	
	best = estimate(test,training,file)
	return best


#funtion to print
def printOut(output):
	errRate = output[0]
	k = output[1]
	matrix = output[2]

	print "Error rate: ",errRate
	print('\n')
	print "k: ",k
	print('\n')
	for i in range(len(matrix)):
		print matrix[i]

#actually run test
run1 = knn("abalone.data.txt")
printOut(run1)

run2 = knn("Seperable.csv")
printOut(run2)

run3 = knn("3percent-miscategorization.csv")
printOut(run3)

run4 = knn("10percent-miscatergorization.csv")
printOut(run4)