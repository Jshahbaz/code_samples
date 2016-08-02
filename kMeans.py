'''
Jared Shahbazian

Program that runs K-Means for K=1,2,4,8,16 on a csv data set and uses the
clusters to train a regression model. Then, the regression models are tested
using the test data.  

Outputs: The Within Cluster Sum of Squares for each cluster made, the RMSE for 
each value of K tested. 

'''

import random as r
import numpy as np
import csv
from numpy import matrix
import math
from scipy.spatial import distance
import matplotlib.pyplot as plt
import math



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


#function to zscale Xy_train, but not the last column!
def zScale(Xy_train):
  numCols = Xy_train[0].size - 1
  numRows = len(Xy_train)
    
  xTranspose = Xy_train.T
  listOfMeans = []
  listOfSDs = []
  
  for col in range(numCols):
    listOfMeans.append(xTranspose[col].mean())
    listOfSDs.append(xTranspose[col].std())
    
  for row in range(numRows):
    for col in range(numCols):
      Xy_train[row][col] = (Xy_train[row][col] - listOfMeans[col]) / listOfSDs[col]
      
  return listOfMeans,listOfSDs, Xy_train


#function to put rows into clusters initially
def assign(clusters,centroids,vector):
  # find the centroid that is the closest to vector
  dists = []
  for i in range(len(centroids)):
    dists.append(distance.euclidean(centroids[i][:-1],vector[:-1]))
    
  lowest = 0
  lowestValue = dists[0]
  for i in range(len(dists)):
    if dists[i] < lowestValue:
      lowestValue = dists[i]
      lowest = i
  
  # add vector to that centroids' cluster
  clusters[lowest].append(vector)
    
  return clusters


#function to get centroid of clusters
def getCentroids(clusters):
  centroids = []
  clusters = np.array(clusters)
  for clusterNum in range(len(clusters)):
    cluster = np.array(clusters[clusterNum])
    colMeans = []
    
    for col in range(len(cluster[0])-1):
      colMeans.append(np.mean(cluster.T[col]))
  
    centroids.append(np.array(colMeans))
  
  return centroids
  
  
#function to rearrange clusters based on centroids
def rearrangeClusters(clusters):
  moved = 1
  centroids = []
  
  while moved==1:
    moved = 0
    #find centroids
    centroids = getCentroids(clusters)

    #go thru each obs and move it if closer to another centroid
    for clusterNum in range(len(clusters)):
      cluster = clusters[clusterNum]
      clusterSizeBeginning = len(cluster)
      toDelete = []
      
      for row in range(clusterSizeBeginning):
        dists = []
        obs = cluster[row]
        
        for centroidNum in range(len(centroids)):
          dists.append(distance.euclidean(obs[:-1],centroids[centroidNum]))
          
        bestCentroidIndex = 0
        minDistance = dists[0]
        
        for i in range(len(dists)):
          if dists[i] < minDistance:
            minDistance = dists[i]
            bestCentroidIndex = i
        
        if bestCentroidIndex != clusterNum:
          #add to clusters[bestCentroidIndex]
          clusters[bestCentroidIndex].append(obs)
          #remove from clusters[clusterNum]
          toDelete.append(row)
          moved = 1
      
      #delete after we get thru whole cluster  
      numToDelete = len(toDelete)
      idx = numToDelete
      while idx > 0:
        indexToDelete = toDelete[idx-1]
        del clusters[clusterNum][indexToDelete]
        idx = idx - 1
        
  return clusters,centroids
  

# function to find the WCSS give a set of clusters and their centroids
def findWCSS(centroids,clusters):
  wcss = 0
  for clusterNum in range(len(clusters)):
    for obs in range(len(clusters[clusterNum])):
      diff = distance.euclidean(clusters[clusterNum][obs],centroids[clusterNum])
      squaredDiff = diff*diff
      wcss += squaredDiff
      
  return wcss
  
 
  
# function to Calculate the Mean and SD for each feature within each cluster
def printClusterStats(cluster,k,clusterNum):
  means = []
  sds = []
  cluster = cluster.T
  
  for col in range(len(cluster)):
    column = np.array(cluster[col])
    means.append(column.mean())
    sds.append(column.std())
    
  print "Means for Cluster #",clusterNum, " in K=",k
  print means
  print "SD's for Cluster #",clusterNum, " in K=",k
  print sds

  

# fucntion to un-do the zscaling so that regression is accurate
def unZScale(cluster,listOfMeans,listOfSDs):  
  for row in range(len(cluster)):    
    for col in range(len(cluster[row])-1):
      cluster[row][col] = (cluster[row][col] * listOfSDs[col]) + listOfMeans[col]
      
  return cluster
  
  
  
# RUN K MEANS  
#  - CHOOSE INITIAL CLUSTERS RANDOMLY
#  - TERMINATE WEHN NO MORE OBS SWITCH CLUSTERS 
def kMeans(k, Xy_train):
  
  # get initial centroids w random numbers
  centroids = []
  randInts = []
  clusters = []
  
  for i in range(k):
    cluster = []
    randInt = np.random.randint(0,len(Xy_train)-1)
    randInts.append(randInt)
    centroids.append(Xy_train[randInt])
    cluster.append(centroids[i])
    clusters.append(cluster)
            
  skip = 0
  for row in range(len(Xy_train)):
    skip = 0
    #make sure we arent double counting the already centroids
    for i in range(len(randInts)):
      if (randInts[i] == row):
        skip = 1
        
    if(skip):
      continue
    #ok now we arent on a randInt, start adding to clusters
    clusters = assign(clusters,centroids,Xy_train[row])
    
  clusters,centroids = rearrangeClusters(clusters)
  
  return centroids, clusters



#function to split csv, 
def driver(file):
  data = []
  #fill matrix of observations
  with open(file, 'rb') as f:
    obs = csv.reader(f)
    for row in obs:
      if(file == "abalone.data.txt"):
        row = proxy(row)
      for i in range(len(row)):
        row[i] = float(row[i])
        
      data.append(row)
  
  data = np.array(data)

  #Shuffle obs and split
  np.random.seed(35)
  np.random.shuffle(data)
  train_num = int(data.shape[0] * 0.9)
  Xy_train = data[:train_num, : ]
  X_test = data[train_num: , :-1]
  Y_test = data[train_num: , -1]
  
  # Z-SCALE XTRAIN
  listOfMeans,listOfSDs, Xy_train = zScale(Xy_train)
  
  # RUN K MEANS FOR K = 1,2,4,8,16
  k = 1
  allClusters = []
  allCentroids = []
  
  for i in range(5):
    centroids, clusters = kMeans(k,Xy_train)
    allClusters.append(clusters)
    allCentroids.append(centroids)
    k *= 2
  
  # OUTPUT CENTROIDS FOR EACH K
  print "CENTROID FOR K=1: "
  print allCentroids[0]
  print "CENTROIDS FOR K=2: "
  print allCentroids[1]
  print "CENTROIDS FOR K=4: "
  print allCentroids[2]
  print "CENTROID FOR K=8: "
  print allCentroids[3]
  print "CENTROID FOR K=16: "
  print allCentroids[4]
    
  allWCSS = []
  # FIND WCSS FOR EACH K 
  for i in range(len(allClusters)):
    allWCSS.append(findWCSS(allCentroids[i],allClusters[i]))

  print "WCSS FOR K=1: ",allWCSS[0]
  print "WCSS FOR K=2: ",allWCSS[1]
  print "WCSS FOR K=4: ",allWCSS[2]
  print "WCSS FOR K=8: ",allWCSS[3]
  print "WCSS FOR K=16: ",allWCSS[4]
  
  # PLOT WCSS VS K
  plt.plot([1,2,4,8,16],allWCSS, 'ro')
  plt.axis([0, 20, 3000, 40000])
  plt.show()
  
  # Output the Mean and Standard Deviation for each feature within each cluster
  for k in range(len(allClusters)):
    for numCluster in range(len(allClusters[k])):
      cluster = np.array(allClusters[k][numCluster])
      printClusterStats(cluster,k,numCluster)
  
  # UNSCALE THE DATA SO THAT REGRESSION YIELDS GOOD BETAS
  allClusters[0][0] = unZScale(allClusters[0][0],listOfMeans,listOfSDs)
 
  # UPDATES CENTROIDS WITH NEW DATA
  for k in range(len(allClusters)):    
    allCentroids[k] = getCentroids(allClusters[k])
  
  # Train a QR Model for observations for each of clusters
  allRMSE = []
  for k in range(len(allClusters)):
    betas = []
    
    #find beta for each cluster
    for numCluster in range(len(allClusters[k])):
      cluster = np.array(allClusters[k][numCluster])
      
      beta,_,_,_ = np.linalg.lstsq(cluster[0:,:-1],cluster[0:,-1])
      betas.append(beta)
      
    # For each test observation choose the cluster whose centroid is nearest this point
    error = 0
    for row in range(len(X_test)):
      obsX = X_test[row]
      
      # find the centroid that is the closest to vector
      dists = []
      for i in range(len(allCentroids[k])):
        dists.append(distance.euclidean(allCentroids[k][i],obsX))
  
      lowestDistIdx = 0
      lowestValue = dists[0]
      for i in range(len(dists)):
        if dists[i] < lowestValue:
          lowestValue = dists[i]
          lowestDistIdx = i
      
      beta = betas[lowestDistIdx]
      
      error += math.pow((np.dot(obsX, beta) - Y_test[row]),2)
      
    rmse = np.sqrt(error/len(X_test))
    allRMSE.append(rmse)
    
  
  for k in range(len(allRMSE)):
    print "RMSE FOR K=",math.pow(2,k),": ",allRMSE[k]

  # PLOT RMSE VS K
  plt.plot([1,2,4,8,16],allRMSE, 'ro')
  plt.axis([0, 20, 0, 3])
  plt.show()


# RUNNER
# All of our test data sets
abalone = "abalone.data.txt"

# Pick a file and run
driver(abalone)




