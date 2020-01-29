import pandas as pd
import numpy  as np
import scipy.stats
import math
import random
from datetime import datetime
from matplotlib import pyplot as plt
from scipy import stats
import threading
import subprocess
import os
import pingouin as pg
from sklearn.linear_model import LinearRegression
###################### MULTI-PROCESSING


parallelDataArchive = []

def parallelWorker(sim,rvType,iid):
	print ( sim + ":"+rvType+" Extraction started\n")
	data = read_csv( sim , rvType)
	if iid == 1:
		x = getIID(data,0.9999)
		parallelDataArchive.append(x)
	else:
		parallelDataArchive.append(data)
	print ( sim + ":"+rvType+" Extraction terminated\n")
	

###################### DATA EXTRACTION

def parse_if_number(s):
    try: return float(s)
    except: return True if s=="true" else False if s=="false" else s if s else None

def parse_ndarray(s):
    return np.fromstring(s, sep=' ') if s else None

def getMeasures(s):
	vectors = read_csv(s)
	result = []
	for m in measures:
		result.append(m.replace(':vector',''))
	return result

def read_csv( s , measure= None):
    vectors = pd.read_csv(s, converters = {
    'attrvalue': parse_if_number,
    'binedges': parse_ndarray,
    'binvalues': parse_ndarray,
    'vectime': parse_ndarray,
    'vecvalue': parse_ndarray})
    itervars_df = vectors.loc[vectors.type=='itervar', ['run', 'attrname', 'attrvalue']]
    itervarspivot_df = itervars_df.pivot(index='run', columns='attrname', values='attrvalue')
    vectors2 = vectors.merge(itervarspivot_df, left_on='run', right_index=True, how='outer')
    itervarscol_df = vectors.loc[(vectors.type=='runattr') & (vectors.attrname.astype(str)=='iterationvars'), ['run', 'attrvalue']]
    itervarscol_df = itervarscol_df.rename(columns={'attrvalue': 'iterationvars'})
    vectors3 = vectors2.merge(itervarscol_df, left_on='run', right_on='run', how='outer')
    vectors = vectors3[vectors3.type=='vector']
    # Now some columns are about to be dropped because they won't be used. If needed, update subsequent line of code accordingly
    vectors = vectors.drop(columns = ['run', 'type', 'module', 'attrname', 'attrvalue', 'value'])
    # Now, some statistics are computed and appended to each tuple. If some of them will not
    # be used cancel or comment the proper line. If other are need, add a new line accordingly
    vectors['Mean'] = vectors.apply (lambda row: row['vecvalue'].mean(), axis=1)
    vectors['StDev'] = vectors.apply (lambda row: row['vecvalue'].std(), axis=1)
    vectors['count'] = vectors.apply (lambda row: row['vecvalue'].size, axis=1)
    vectors['min'] = vectors.apply (lambda row: row['vecvalue'].min(), axis=1)
    vectors['max'] = vectors.apply (lambda row: row['vecvalue'].max(), axis=1)
    #vectors['median'] = vectors.apply (lambda row: stats.median(row['vecvalue']), axis=1)
    if measure == None:
    	return vectors
    else:
    	measure = measure + ':vector'
    	return pd.DataFrame({'Time':vectors[vectors.name==(measure)]['vectime'].iloc[0],'Value':vectors[vectors.name==(measure)]['vecvalue'].iloc[0]})


def doSimulations(omnetConfName):
	v1 = ('./CompileAndRun')                 # variabile con l'applicazione da lanciare
	vFin = ("%s %s" % (v1,omnetConfName))    # trasformo le var. in stringa senza virgole
	subprocess.call(vFin, shell=True)    	 # svolgo il processo "usando il terminale"
	value = os.popen("ls /home/nicola/Scrivania/HDD/TempSim/"+omnetConfName+"*0.sca").read()
	value.replace(",","_")	
	value = value[value.index(omnetConfName):len(value)-8]
	value = value + datetime.now().strftime("_%d_%H_%M")
	os.system("mkdir /home/nicola/Scrivania/HDD/"+value)
	files = os.popen("ls /home/nicola/Scrivania/HDD/TempSim/*.vec").read()
	files = files.split("\n")
	for file in files:
		os.system("scavetool x "+file+" -o " +file[:len(file)-4]+".csv")
	os.system("mv /home/nicola/Scrivania/HDD/TempSim/*.csv /home/nicola/Scrivania/HDD/"+value)
	os.system("rm /home/nicola/Scrivania/HDD/TempSim/*")
	
def showRecordedSimulations():
	return os.popen("ls /home/nicola/Scrivania/HDD").read().replace("\nTempSim\n","").split("\n")

def getRvs( sim , rvType , iid=1):
	files = os.popen("ls /home/nicola/Scrivania/HDD/"+sim+"/*.csv").read()
	files = files.split("\n")
	print(files)
	rvs = pd.DataFrame({'Time':[],'Value':[]})
	for a in range(0,len(files)-1):
		data = read_csv( files[a] , rvType)
		if iid ==1:
			x = getIID(data,0.9999)
			rvs = pd.concat([rvs , x])
		else:
			rvs = pd.concat([rvs,data])
		print(rvs.size)
		print( np.mean(rvs['Value'].values))
		print( + np.median(rvs['Value'].values))
		print(np.var(rvs['Value'].values))
		print("---------------------")
	return rvs

            
def getParallelRvs( sim, rvType , iid=1):

	files = os.popen("ls /home/nicola/Scrivania/HDD/"+sim+"/*.csv").read()
	files = files.split("\n")
	print(files)
	rvs = pd.DataFrame({'Time':[],'Value':[]})
	threads = []
	for a in range(0,len(files)-1):
		thread = threading.Thread(target=parallelWorker,args=(files[a],rvType,iid))
		thread.setName(sim+":"+rvType)
		threads.append(thread)
		thread.start()
	for a in range(0,len(threads)):
		threads[a].join()
	data = pd.concat(parallelDataArchive)
	parallelDataArchive.clear()
	return data
	
########################### DATA ANALYSIS

def cdf(data):
	print('ciao')

def histogram(data):
	print('ciao')

def IQR(data):
	return np.percentile(data,75,interpolation='midpoint')-np.percentile(data,25,interpolation='midpoint')

def removeOutliers(df):
	data = df['Value'].values
	time = df['Time'].values
	newValue = []
	newTime = []
	maxValue = 1.5*IQR(data)

	for i in range(0,len(data)):
		if abs(data[i]) <= maxValue: 
			newValue.append(data[i])
			newTime.append(time[i])

	return pd.DataFrame({'Time':newTime,'Value':newValue})


def autocorrelation( df , max_lag ):
	ser = pd.Series(df['Value'].values)
	values = []
	for i in range(1,max_lag+1):
		values.append(ser.autocorr(i))
	return values

# the function verify if the RVs are indipendent. It uses the formula z(a/2)/sqrt(n) as a threshold to verify indipendence
def isIID(df,accuracy):  
	n = df.size/2	
	CI = (scipy.stats.norm.ppf(accuracy))/math.sqrt(n)  # define the threshold
	correlation = autocorrelation(df,1000)  # we take the autocorrelation data and verify it stays under the threshold
	for i in correlation: 
		if abs(i)>CI: 
			return False
	return True


# reduces of the 10% the sample space, the function is very efficient to decrease RV's correlation
def subsample(df , p=.9):
	retTimes = []
	retValues = []
	times = df['Time'].values
	sample = df['Value'].values
	random.seed(datetime.now())
	new_array = np.array([])
	for i in range(0,len(sample)):
		if np.random.rand() < p:
			retTimes.append(times[i])
			retValues.append(sample[i])
	return pd.DataFrame({'Time':retTimes,'Value':retValues})


# reduce the sample space until variable will be IID with a certain accuracy(accuracy between 0 and 1)
def getIID(df , accuracy):  
	accuracy = ((1-accuracy)/2)+accuracy  #  written form wanted from the scipy.ppf function
	while( not isIID(df,accuracy) ):	  #  we verify if the sample RVs are indipendente
		df = subsample(df)                  #  if not we do a subsample 
	
	return df

# Used for qqplot into the fitting function, generates an array of quantiles of a given distribution
def getReferenceDistr( distrType , distrParam ):

	data = []
	control = 0
	# generation of the function from which we will calculate reference quantiles
	switcher={
		#discrete
		'geometric'   : np.random.geometric(distrParam,10000000),         #  PARAM: probability of success   
		'binomial'    : np.random.binomial(1,distrParam , 10000000),      #  PARAM: probability of success
		'poisson'     : np.random.poisson(distrParam,10000000),           #  PARAM: lambda
		#continue
		'uniform'     : np.random.uniform(0,distrParam,10000000),         #  PARAM: right limit of the distribution(first fixed to 0)
		'exponential' : np.random.exponential(distrParam,10000000),       #  PARAM: lambda
   		'normal'      : np.random.normal(distrParam,1,10000000),          #  PARAM: mean of the distribution
		'pareto'      : np.random.pareto(distrParam,10000000),            #  PARAM: A coefficient(NOT A^2), point of starting of the distribution
		'erlang'      : np.random.gamma(2,1,10000000)
	}
	data = switcher.get(distrType, 'Error')
	return data


# the function gives an array with the percentiles of the given data
def getPercentiles( distr , n_Quantiles ):

	quantiles = []
 	# we can use 0 or 100 quantiles, or we are like saying there couldn't be a value greater/lower than the 100/0 quantile	
	quantiles_interval = 98/(n_Quantiles*100)
	current_quantile = 0.01
	
	for i in range(0 , n_Quantiles):
		quantiles.append(np.quantile(distr, current_quantile))
		current_quantile += quantiles_interval 
	
	maxVal = max(quantiles)

	return quantiles

def qqplot(df, dist , table_name ):
	
	if dist == 'erlang':
		pg.qqplot(df, dist, sparams=(2,), confidence=.95).set_title(table_name)
	else:
		pg.qqplot(df, dist, confidence=.95).set_title(table_name)
       
def hiberexponentialQuantile(n_Quantiles):
	quantiles = []
	probs = np.arange(1,n_Quantiles+2)/(n_Quantiles+1)
	
	for p in probs:
		if not p == 1:
			quantiles.append(math.log(2/(math.sqrt(1-8*(p-1))-1)))
	return quantiles

def CI_bounds(vector, con_coef = None):
    n = 100000
    if con_coef is None or con_coef > 1 or con_coef < 0:
        con_coef = .95
    alpha = 1. - con_coef
    x_bar = vector.mean()
    sigma = vector.std()
    z_critical = stats.norm.ppf(q = con_coef + alpha/2)
    standard_error = sigma / math.sqrt(n)
    return (x_bar - z_critical * standard_error, x_bar + z_critical * standard_error)	

#  the function fit the given data to a distributionType, distrParam is used to configure the reference distribution, more info on getReferenceDistr()
def fitting( data , distrType , distrParam  ):
	
	# 50 points recomended for big dataset, otherwise a number of points equal to the sample size(quantile_interval equal to 1/n)
	# https://meetheskilled.com/q-q-plot-ed-uso-in-normality-test/
 
	distrQ = getPercentiles(data,50)
	if distrType == 'hiperexponential':
		refQ = hiberexponentialQuantile(50)
	else:
		refQ = getPercentiles(getReferenceDistr(distrType,distrParam),50)

	lr = LinearRegression()
	rfQres = np.array(refQ).reshape((-1,1))

	lr.fit(rfQres,distrQ)

	pred = lr.predict(rfQres)
	
	# to set the line to see better the plot tendency
	maxY = refQ.pop()
	maxX = distrQ.pop()
	if maxY > maxX: 
		m = maxY
	else:
		m = maxX
	
	refQ.append(maxY)
	distrQ.append(maxX)
	minY = refQ[0]
	
	plt.scatter(refQ,distrQ)
	plt.plot(refQ,pred,color='red')
	plt.xlabel('Theoretical quantiles')
	plt.ylabel('Ordered quantiles')
	plt.legend(pd.Series("R\u00b2 ="+str('%.3f'%lr.score(rfQres,distrQ))))
	plt.title('Q-Q plot')



