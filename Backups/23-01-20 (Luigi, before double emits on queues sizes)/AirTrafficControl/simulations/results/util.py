import numpy as np
import pandas as pd
import os
import fnmatch
import shutil
import seaborn as sns
from scipy.signal._savitzky_golay import * # savgol_filter
import scipy.stats as stats
import math
import random
import matplotlib.pyplot as plt
import statsmodels.api as sm 
import pylab as py 
import pingouin as pg
from statsmodels.graphics import tsaplots

# Update the subsequent vector when adding new configuration to omnet.ini
SIM = 'ExponentialRegimeBalanced'
def randomColor():
    r = random.randint(0,255)
    g = random.randint(0,255)
    b = random.randint(0,255)
    rgb = [r, g, b]
    return rgb

def color(r, g, b):
    '''
    minimum = 1
    maximum = 5
    rn = ((r - minimum)/(maximum - minimum))
    gn = ((g - minimum)/(maximum - minimum))
    bn = ((b - minimum)/(maximum - minimum))
    '''
    random.seed(r)
    rn = random.random()
    random.seed(g)
    gn = random.random()
    random.seed(b)
    bn = random.random()
    rgbn = [[rn, gn, bn]]
    return rgbn

def parse_if_number(s):
    try: return float(s)
    except: return True if s=="true" else False if s=="false" else s if s else None

def parse_ndarray(s):
    return np.fromstring(s, sep=' ') if s else None

def extractVec(s):
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
    return vectors

def wide(vectors, name):
    index = 'iat'
    columns = ['lt', 'tot', 'pt']
    a = vectors[vectors['name'] == name].pivot_table(index = index, columns = columns, values = 'Mean')
    return a

def getVecFileName():
    parent_dir = os.getcwd()
    files = os.listdir(os.getcwd())
    for filename in fnmatch.filter(files, "*.vec"):
        source = filename
        filename = filename.split(".vec")
        directory = filename[0]
        path = os.path.join(parent_dir, directory)
        cmd = 'scavetool x ' + path + '/*.vec -o ' + path + '/' + filename[0] + '.csv'
        try:
            os.mkdir(path)
            print("Directory '%s' created successfully" % directory) 
        except OSError as error:
            print("Directory '%s' can not be created" % directory)
            print(error)
        try:
            shutil.move(source, path)
            print("File '%s' moved successfully" % source) 
        except OSError as error:
            print("File '%s' can not be moved" % source)
            print(error)
        os.system(cmd)
        '''
        TODO: SCRIPT SECTION THAT ANALYZES ALL THE CSV GOES HERE
        '''
    print('Ended')

def scavetool():
    #os.system('rm -f *.csv')
    for sim in SIMULATIONS:
        os.system('scavetool x ' + sim + '*.vec *.sca -o ' + sim + '.csv')
    #os.system('rm -f *.sca *.out *.vci *.vec')

def CI_bounds(vector, con_coef = None):
    n = len(vector)
    if con_coef is None or con_coef > 1 or con_coef < 0:
        con_coef = .95
    alpha = 1. - con_coef
    x_bar = vector.mean()
    sigma = vector.std()
    z_critical = stats.norm.ppf(q = con_coef + alpha/2)
    standard_error = sigma / math.sqrt(n)
    return (x_bar - z_critical * standard_error, x_bar + z_critical * standard_error)

def addIterationVar(df):
    # df is a dataframe coming from pandas.read_X()
    itervars_df = df.loc[df.type=='itervar', ['run', 'attrname', 'attrvalue']]    
    itervarspivot_df = itervars_df.pivot(index='run', columns='attrname', values='attrvalue')
    df2 = df.merge(itervarspivot_df, left_on='run', right_index=True, how='outer')
    itervarscol_df = df.loc[(df.type=='runattr') & (df.attrname.astype(str)=='iterationvars'), ['run', 'attrvalue']]
    itervarscol_df = itervarscol_df.rename(columns={'attrvalue': 'iterationvars'})
    df3 = df2.merge(itervarscol_df, left_on='run', right_on='run', how='outer')
    return df3

def barp(vectors):
    # AGGIUNGERE INTERVALLO DI CONFIDENZA
    for name in vectors.name.unique():
        table = vectors[(vectors['name'] == name)][['iat', 'tot', 'lt', 'pt', 'Mean']]
        '''
        print(name)
        print(table)
        '''
        s = []
        for i in range(0, table.index.size):
            plt.scatter(table.iloc[i][0], table.iloc[i][4], c = color(table.iloc[i][1], table.iloc[i][2], table.iloc[i][3]))
            s.append('tot= ' + str(table.iloc[i][1]) + ', lt=' +  str(table.iloc[i][2]) + ', pt=' + str(table.iloc[i][3]))
        plt.legend(pd.Series(s))
        plt.ylabel(name)
        plt.xlabel('iat')
        plt.show()

def fillb(time_series_df):
    smooth_path = time_series_df.rolling(20).mean()
    path_deviation = 2 * time_series_df.rolling(20).std()
    plt.plot(smooth_path, linewidth=2)
    plt.fill_between(path_deviation.index, (smooth_path-2*path_deviation)[0], (smooth_path+2*path_deviation)[0], color='b', alpha=.1)

def MS(df): # mean and standard deviation for a set of vectors
    tmp = np.array([])
    for j in range(0, len(df)):
        tmp = np.concatenate((tmp, df.iloc[j]))
    return (np.mean(tmp), np.std(tmp))

def cilineplot(x, y, m, s, con_coef = None): # m = mean, s = standard deviation
    if con_coef is None or con_coef > 1 or con_coef < 0:
        con_coef = .95
    alpha = 1. - con_coef
    z_critical = stats.norm.ppf(q = con_coef + alpha/2)
    standard_error = s / math.sqrt(len(x))
    lower = y - z_critical * standard_error
    upper = y + z_critical * standard_error
    plt.plot(x, y, linewidth = 0.5)
    plt.fill_between(x = x, y1 = lower, y2 = upper, color='b', alpha=.1)

def qqplot(df, dist = 'expon'):
    if dist == 'erlang':
        pg.qqplot(df, dist, sparams=(2,), confidence=.95)
    else:
        pg.qqplot(df, dist, confidence=.95)

def correlogram(df, title = None, lags = 10):
    '''
    Consult: (Luigi: preferisco questo)
        https://www.statsmodels.org/stable/generated/statsmodels.graphics.tsaplots.plot_acf.html
    and (for lag plot)
    https://github.com/2wavetech/How-to-Check-if-Time-Series-Data-is-Stationary-with-Python
    
    '''
    fig = tsaplots.plot_acf(df, lags=lags)
    #pd.plotting.autocorrelation_plot(df, lags = lags)

def discreteQQplot(x_sample, p=0.5):
    """ Plot the Q-Q plot of the geometric distribution """
    ecdf_sample = np.arange(1, len(x_sample) + 1)/(len(x_sample)+1)
    x_theor = stats.geom.ppf(ecdf_sample, p=p)
    plt.scatter(x_theor, x_theor, label='theor-theor quantiles')
    plt.scatter(x_theor, x_sample, label='theor-sample quantiles', marker = '.')
    plt.xlabel('Theoretical quantiles')
    plt.ylabel('Sample quantiles')
    plt.legend()

def plotDF(df):
    if os.path.isdir('./' + SIM) is False:
        os.mkdir('./' + SIM)
    if os.path.isdir('./' + SIM + '/plots') is False:
        os.mkdir('./' + SIM + '/plots')
    os.system('rm -rf ./' + SIM + '/plots/*')
    for itervars in df.iterationvars.unique():
        vectors = df[df.iterationvars == itervars][['iterationvars', 'name', 'vectime', 'vecvalue']]
        for name in vectors.name.unique():
            tmp = vectors[vectors.name == name]
            l = len(tmp)
            for i in range(l):
                plt.plot(tmp.iloc[i]['vectime'], tmp.iloc[i]['vecvalue'], marker = '.', markersize = 0.01)             
                plt.title(name.split(':')[0] + " " + itervars + " (iter = " + str(i) + ")")
                plt.savefig(name.split(':')[0] + " " + itervars + " (iter = " + str(i) + ").png")
                plt.clf()
    os.system('cp *.png ./' + SIM + '/plots')
    os.system('rm -f *.png')

def scatterDF(df):
    print("ciao")

def prova(tmp):
    #funzioni di prova plotDF
    con_coef = .99
    alpha = 1. - con_coef
    z = stats.norm.ppf(q = con_coef + alpha/2)*np.std(tmp.iloc[0]['vecvalue'])/math.sqrt(len(tmp.iloc[0]['vecvalue']))
    print(z)
    plt.plot(tmp.iloc[0]['vectime'], tmp.iloc[0]['vecvalue'])
    plt.fill_between(tmp.iloc[0]['vectime'], tmp.iloc[0]['vecvalue'] - z, tmp.iloc[0]['vecvalue'] + z, color='b', alpha=.1)

    plt.title("prova " + str(tmp.iloc[0].iterationvars))
    
def prova1(vectors):
    #funzione di prova per scatterDF
    vectors = vectors.groupby('iterationvars').agg(
        cmean = pd.NamedAgg(column = 'Mean', aggfunc = lambda x: sum(x)/len(x)))
    return vectors

def mean(arr): 
    n = len(arr)
    sum = 0
    for i in range(n): 
        sum = sum + arr[i]
    return sum/n

def sd(arr):
    sum = 0
    n = len(arr)
    m = mean(arr)
    for i in range(n): 
        sum = sum + (arr[i] - m)**2
    return sum / n

def combinedVariance(means, sds, counts): 
    # ATTENTION: sds must be variancem not standard deviation
    # https://www.youtube.com/watch?v=wUTBPLO4Xmc
    
    combinedMean = sum(x * y for x, y in zip(means, counts)) /sum(counts)

    return sum(z * (y + (x - combinedMean)**2) for x, y, z in zip(means, sds, counts)) /sum(counts)
    

