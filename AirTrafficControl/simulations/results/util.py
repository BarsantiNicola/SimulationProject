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
import gc
from statsmodels.distributions.empirical_distribution import ECDF
from datetime import datetime
from sklearn.linear_model import LinearRegression
from scipy.stats import rv_continuous

# Update the subsequent vector when adding new configuration to omnet.ini
SIM = 'Debug'
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
    vectors['iterationvars'] = vectors.apply (lambda row: row['iterationvars'].split(', $0')[0], axis=1)
    vectors['Mean'] = vectors.apply (lambda row: row['vecvalue'].mean(), axis=1)
    vectors['StDev'] = vectors.apply (lambda row: row['vecvalue'].std(), axis=1)
    vectors['count'] = vectors.apply (lambda row: row['vecvalue'].size, axis=1)
    vectors['neff'] = vectors.apply (lambda row: neff(np.array(row['vecvalue'])), axis=1)
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
    os.system('scavetool x /media/luigi/88A4ABD5A4ABC3D2/results/*.vec -o /media/luigi/88A4ABD5A4ABC3D2/results/results.csv');

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

def qqplot(df, dist = 'expon', con_coef = 0.95):
    if con_coef is None or con_coef > 1 or con_coef < 0:
        con_coef = .95
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
    tsaplots.plot_acf(df, lags=lags)
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

def plotDF(df, con_coef = None):
    if con_coef is None or con_coef > 1 or con_coef < 0:
        con_coef = .95
    alpha = 1. - con_coef
    z_critical = stats.norm.ppf(q = con_coef + alpha/2)
    if os.path.isdir('./' + SIM) is False:
        os.mkdir('./' + SIM)
    if os.path.isdir('./' + SIM + '/plots') is False:
        os.mkdir('./' + SIM + '/plots')
    os.system('rm -rf ./' + SIM + '/plots/*')
    for itervars in df.iterationvars.unique():
        vectors = df[df.iterationvars == itervars][['iterationvars', 'name', 'vectime', 'vecvalue', 'StDev', 'neff']]
        for name in vectors.name.unique():
            tmp = vectors[vectors.name == name]
            l = len(tmp)
            for i in range(l):
                try:
                    plt.plot(tmp.iloc[i]['vectime'], tmp.iloc[i]['vecvalue'], marker = '.', markersize = 0.01)
                    errBand = z_critical*tmp.iloc[i]['StDev']/math.sqrt(tmp.iloc[i]['neff'])
                    lower = tmp.iloc[i]['vecvalue'] - errBand
                    upper = tmp.iloc[i]['vecvalue'] + errBand
                    
                    plt.fill_between(x = tmp.iloc[i]['vectime'], y1 = lower, y2 = upper, color='b', alpha=.1)
                    plt.title(name.split(':')[0] + " " + itervars + " (iter = " + str(i) + ")")
                    plt.savefig(name.split(':')[0] + " " + itervars + " (iter = " + str(i) + ").png")
                    #plt.show()
                    plt.clf()
                except OverflowError:
                    print(name + " " + itervars)
                    continue
    os.system('cp *.png ./' + SIM + '/plots')
    os.system('rm -f *.png')
    
def scatterDF(df, con_coef = None):
    if con_coef is None or con_coef > 1 or con_coef < 0:
        con_coef = .95
        
    alpha = 1. - con_coef
    

    vectors = df.groupby(['iat', 'lt', 'tot', 'pt', 'name']).apply(lambda x: pd.Series({
          'cmean': (x['Mean']*x['neff']).sum()/x['neff'].sum(),
          'cEffCount': x['neff'].sum(),
          'cvar':(x['neff'] * ( x['StDev']**2 + ( x['Mean'] - ( x['Mean'] * x['neff'] ).sum()/x['neff'].sum())**2)).sum()/x['neff'].sum()
      })
    )
    vectors = vectors.reset_index()
    
    vectors['upper'] = vectors.apply (lambda row: row.cmean + stats.norm.ppf(q = con_coef + alpha/2)*math.sqrt(row.cvar/row.cEffCount), axis=1)
    vectors['lower'] = vectors.apply (lambda row: row.cmean - stats.norm.ppf(q = con_coef + alpha/2)*math.sqrt(row.cvar/row.cEffCount), axis=1)
    vectors['error'] = vectors.apply (lambda row: row.upper - row.lower, axis = 1)
    
    if os.path.isdir('./' + SIM) is False:
        os.mkdir('./' + SIM)
    if os.path.isdir('./' + SIM + '/scatters') is False:
        os.mkdir('./' + SIM + '/scatters')
    os.system('rm -rf ./' + SIM + '/scatters/*')
    
    for name in vectors.name.unique():
        i = 1
        for pt in vectors.pt.unique():
            s = []
            for row in vectors[(vectors.name == name) & (vectors.pt == pt)].itertuples():
                plt.scatter(row.iat, row.cmean)
                '''
                to enable legend on plot, aside the dots
                if i%2 == 0: align ='right'
                else: align = 'left'
                i = i + 1
                plt.text(row.iat, row.cmean, s = 'tot= ' + str(row.tot) + '\nlt=' +  str(row.lt), fontsize = 10, ha = align, va = 'center')
                '''
                plt.errorbar(row.iat, row.cmean, yerr = (row.upper - row.lower))
                s.append('tot= ' + str(row.tot) + ', lt=' +  str(row.lt))
            plt.legend(pd.Series(s), loc='center left', bbox_to_anchor=(1, 0.5))
            plt.title(name.split(':')[0] + "(pt=" + str(row.pt) + ")")
            plt.savefig(name.split(':')[0] + " (pt=" + str(row.pt) + ").png", quality = .95)
            plt.xlabel('iat')
            plt.show()
            plt.clf()
    
    os.system('cp *.png ./' + SIM + '/scatters')
    os.system('rm -f *.png')
    
    vectors.to_csv('./' + SIM + '/scatters/recap.csv')
    
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
    # ATTENTION: sds must be variance and not standard deviation
    # https://www.youtube.com/watch?v=wUTBPLO4Xmc
    
    combinedMean = sum(x * y for x, y in zip(means, counts)) /sum(counts)

    return sum(z * (y + (x - combinedMean)**2) for x, y, z in zip(means, sds, counts)) /sum(counts)
    
def lorenz(df):
    if os.path.isdir('./' + SIM) is False:
        os.mkdir('./' + SIM)
    if os.path.isdir('./' + SIM + '/lorenz') is False:
        os.mkdir('./' + SIM + '/lorenz')
    os.system('rm -rf ./' + SIM + '/lorenz/*')
    
    for itervars in df.iterationvars.unique():
        vectors = df[df.iterationvars == itervars][['iterationvars', 'name', 'vecvalue']]
        for name in vectors.name.unique():
            if (name == 'DepartQueueSize:vector') or (name == 'HoldingQueueSize:vector') or (name =='ParkedPlanes:vector'):
                continue
            tmp = vectors[vectors.name == name]
            l = len(tmp)
            s = []
            for i in range(l):
                arr = np.sort(tmp.iloc[i]['vecvalue'])
                m = arr.mean()
                lcg = (abs(arr - m)).sum()/(2*arr.size*m)
                scaled_prefix_sum = arr.cumsum() / arr.sum()
                lc = np.insert(scaled_prefix_sum, 0, 0)
                plt.plot(np.linspace(0.0, 1.0, lc.size), lc)
                s.append('iter = ' + str(i) + ', lcg = ' + str('%.2f'%lcg)) # to redce precision
            plt.legend(pd.Series(s))
            plt.plot([0, 1], [0, 1], color = 'k')
            plt.title(name.split(':')[0] + " " + itervars)
            #plt.show()
            plt.savefig(name.split(':')[0] + " " + itervars + ".png")
            plt.clf()
    os.system('cp *.png ./' + SIM + '/lorenz')
    os.system('rm -f *.png')

def histDF(df):
    if os.path.isdir('./' + SIM) is False:
        os.mkdir('./' + SIM)
    if os.path.isdir('./' + SIM + '/hist') is False:
        os.mkdir('./' + SIM + '/hist')
    os.system('rm -rf ./' + SIM + '/hist/*')
    
    for itervars in df.iterationvars.unique():
        vectors = df[df.iterationvars == itervars][['iterationvars', 'name', 'vecvalue']]
        for name in vectors.name.unique():
            if name != 'ParkedPlanes:vector': continue
            tmp = vectors[vectors.name == name]
            l = len(tmp)
            for i in range(l):
                arr = tmp.iloc[i]['vecvalue']
                mu = 1/arr.mean()

                if name == 'HoldingQueueWaitingTime:vector' or name == 'DepartQueueWaitingTime:vector':
                    _, bins, _ = plt.hist(arr, bins = 30, density = 1)
                    x = np.arange(min(bins), max(bins), step = 0.5)
                    y = mu*np.exp(-1*mu*x)
                elif name == 'AirportResponseTime:vector': #Erlang
                    _, bins, _ = plt.hist(arr, bins = 30, density = 1)
                    x = np.arange(min(bins), max(bins), step = 0.5)
                    k = 2
                    mu = mu*k
                    op1 = np.power(mu, k)
                    op2 = np.power(x, k-1)
                    op3 = np.exp(-1*x*mu)
                    op4 = 1/math.factorial(k-1)
                    y = op1*op2*op3*op4
                elif name == 'HoldingQueueSize:vector' or name == 'DepartQueueSize:vector':
                    _, bins, _ = plt.hist(arr, bins = np.arange(round(min(arr)), round(max(arr)), step = 1), density = 1)
                    x = np.arange(min(bins), max(bins), step = 1)
                    y = mu*((1-mu)**(x-1))
                else:
                    continue
                    # don't now what the distribution of parked planes is 
                    # to print this cancel the first line after the second for statement
                    _, bins, _ = plt.hist(arr, bins = np.arange(round(min(arr)), round(max(arr)), step = 1), density = 1)
                    x = np.arange(min(bins), max(bins), step = 1)
                    print(mu)
                    y = (mu**2)*((1-mu)**(x-2))*(x-1)
                    return x,y
                plt.plot(x, y, color='r')
                plt.text(np.mean(x), .75*max(y), s = "$\lambda=" + str('%.4f'%(1/mu)) + "$")
                plt.title(name.split(':')[0] + " " + itervars + "(iter = " + str(i) + ")")
                plt.tight_layout()
                plt.show()
                #return arr
                #plt.savefig(name.split(':')[0] + " " + itervars + " (iter=" + str(i) + ").png")
                plt.clf()
    os.system('cp *.png ./' + SIM + '/hist')
    os.system('rm -f *.png')

def subsample(vector):
    p = 0.0002
    np.random.seed(datetime.now().microsecond)
    arr = np.array([])
    for x in vector:
        if np.random.rand() < p: arr = np.concatenate((arr, [x]))
    return arr

def mergeVectors(df, name):
    res = np.array([])
    vectors = df[df.name == name]['vecvalue']
    for i in range(len(vectors)):
        res = np.concatenate((res, np.array(vectors.iloc[i])))
    return res

def hyperExpQuantile(p):
    '''
    Returns the quantiles of an hyperexponential distribution
    of degree 2 and with parameters:
        labda_1 = 1, lambda:_2 = 2
        p_1 = 0.5, p_2 = 0.5
    '''
    if p > 1 or p < 0:
        return -1
    return math.log(2/(math.sqrt(1 - 8*(p - 1))-1))
    #return math.log(1/(1-p))/2*p


def hyperQQ(vectors):
    probs = np.arange(0, 10000)/9999
    x = []
    y = []
    for p in probs:
        try: tmp = hyperExpQuantile(p)
        except ZeroDivisionError: continue
        x.append(tmp)
        y.append(np.quantile(vectors, p))
    x_r = np.array(x).reshape((-1,1))
    x = np.array(x)
    y = np.array(y)
    linear_regressor = LinearRegression() 
    linear_regressor.fit(x_r, y) 
    Y_pred = linear_regressor.predict(x_r)
    plt.plot(x, Y_pred, color='red')
    plt.scatter(x, np.array(y), color = 'b', marker = '.')
    plt.show()
    return linear_regressor.score(x_r, y)

def neff(arr):
    '''
    References:
        http://www.metrology.pg.gda.pl/full/2010/M&MS_2010_003.pdf

    '''
    n = len(arr)
    acf = sm.tsa.acf(arr, nlags = n, fft = True)
    sums = 0
    for k in range(1, len(arr)):
        sums = sums + (n-k)*acf[k]/n
    
    return n/(1 + 2*sums)

class hexpon_gen(rv_continuous):
    "Hyper Exponential distribution"
    def _pdf(self, x):
        return .8*np.exp(-x) + 2*np.exp(-2*x)/5
    def _cdf(self, x):
        return .8*(1-np.exp(-x)) + .2*(1-np.exp(-2*x))
hexpon = hexpon_gen(a=0.0, name='hexpon')
    
def boxplotDF(df):
    if os.path.isdir('./' + SIM) is False:
        os.mkdir('./' + SIM)
    if os.path.isdir('./' + SIM + '/boxplot') is False:
        os.mkdir('./' + SIM + '/boxplot')
    os.system('rm -rf ./' + SIM + '/boxplot/*')
    
    for itervars in df.iterationvars.unique():
        vectors = df[df.iterationvars == itervars][['iterationvars', 'name', 'vecvalue']]
        for name in vectors.name.unique():
            tmp = vectors[vectors.name == name]
            l = len(tmp)
            for i in range(l):
                arr = tmp.iloc[i]['vecvalue']
                plt.boxplot(arr, notch=True, autorange=True)
                plt.legend(pd.Series('iter = ' + str(i)))
                plt.title(name.split(':')[0] + " " + itervars)
                #plt.show()
                plt.savefig(name.split(':')[0] + " " + itervars + " (iter=" + str(i) + ").png")
                plt.clf()
    os.system('cp *.png ./' + SIM + '/boxplot')
    os.system('rm -f *.png')
    
def QQPlotDF(df):
    gc.enable()
    if os.path.isdir('./' + SIM) is False:
        os.mkdir('./' + SIM)
    if os.path.isdir('./' + SIM + '/qqplot') is False:
        os.mkdir('./' + SIM + '/qqplot')
    os.system('rm -rf ./' + SIM + '/qqplot/*')
    
    for itervars in df.iterationvars.unique():
        vectors = df[df.iterationvars == itervars][['iterationvars', 'name', 'vecvalue']]
        for name in vectors.name.unique():
            if (name == 'HoldingQueueSize:vector') or (name == 'DepartQueueSize:vector') or (name == 'ParkedPlanes:vector'):
                print(name)
                continue
            tmp = mergeVectors(df, name)
            gc.collect()
            qqplot(tmp, 'expon')
            gc.collect()
            plt.title(name.split(':')[0] + " " + itervars)
            #plt.show()
            plt.savefig(name.split(':')[0] + " " + itervars + ".png")
            plt.clf()
    os.system('cp *.png ./' + SIM + '/qqplot')
    os.system('rm -f *.png')

def F_w(iat, lt, tot, w):
    mux = 1.0/lt
    muy = 1.0/tot
    muz = 1.0/iat
    if w < 0:
        return 1 - np.exp(w/muz)*(muz**2)/((muz+mux)*(muz+muy))
    else:
        return (1-np.exp(-1*w/mux))*(mux**2)/((mux-muy)*(mux+muz)) + (1-np.exp(-1*w/muy))*(muy**2)/((muy-mux)*(muy+muz)) + (muz**2)/((muz+mux)*(muz+muy))

def test(iat, lt, tot):
    return stats.expon.rvs(lt) + stats.expon.rvs(tot) - stats.expon.rvs(iat)