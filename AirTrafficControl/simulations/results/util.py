import numpy as np
import pandas as pd
import os
#from scipy.signal._savitzky_golay import * # savgol_filter
import scipy.stats as stats
import math
import random
import matplotlib.pyplot as plt
import statsmodels.api as sm
import pingouin as pg
from statsmodels.graphics import tsaplots
import gc
from datetime import datetime
from sklearn.linear_model import LinearRegression
from scipy.stats import rv_continuous
from scipy.stats import rv_discrete
from scipy.special import factorial
from sklearn.metrics import r2_score
from fitter import Fitter

# Update the subsequent vector when adding new configuration to omnet.ini
SIM = 'sim'
def randomColor():
    r = random.randint(0,255)
    g = random.randint(0,255)
    b = random.randint(0,255)
    rgb = [r, g, b]
    return rgb

def color(r, g, b):
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
    vectors['uf'] = vectors.apply (lambda row: (row['lt'] + row['tot'])/row['iat'], axis=1)
    vectors['expectedMeanPP'] = vectors.apply (lambda row: row['pt']/row['iat'], axis=1)    
    return vectors

def extractSca(s):
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
    vectors = vectors3[vectors3.type=='scalar']
    vectors = vectors.drop(columns = ['run', 'type', 'module', 'attrname', 'attrvalue'])    
    return vectors

def scavetool():
    os.system('scavetool x /media/luigi/88A4ABD5A4ABC3D2/results/D*.sca -o /media/luigi/88A4ABD5A4ABC3D2/results/DeterministiRegimeStable.csv')

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


def qqplot(df, dist = 'expon', con_coef = 0.95, title = None):
    if con_coef is None or con_coef > 1 or con_coef < 0:
        con_coef = .95
    df = df - df.mean()
    if dist == 'expon': 
        pg.qqplot(df, 'expon', confidence=.95)
    else: 
        pg.qqplot(df, hexpon, confidence=.95)
        dist = 'Hyperexponential'
    plt.xlabel('Theoretical quantiles (' + dist.capitalize() + ')')
    if title != None: plt.title('QQPlot - ' + title)

def correlogram(df):
    '''
    Consult: 
        https://www.statsmodels.org/stable/generated/statsmodels.graphics.tsaplots.plot_acf.html
    and (for lag plot)
    https://github.com/2wavetech/How-to-Check-if-Time-Series-Data-is-Stationary-with-Python
    
    '''
    tsaplots.plot_acf(df, lags = len(df)-20, fft = True, zero = False, unbiased = True, alpha = .05, use_vlines = False)
    plt.title('Airport Response Time (\u03C1 = 0.33)')
    plt.xlabel('Lag (Max Lag = ' + str(len(df)-2)+')')
    #pd.plotting.autocorrelation_plot(df)

class mygeom_gen(rv_discrete):

    def _pmf(self, k, p):
        return np.power(1-p, k) * p
mygeom = mygeom_gen(a=0.0, name='mygeom')

def discreteQQplot(x_sample, dist, title = None):
    """ Plot the Q-Q plot of the Poisson and Geometric Disribution distribution """
    
    if dist == 'poisson':
        
        theor = stats.poisson.rvs(x_sample.mean(), size = len(x_sample))
    elif dist == 'geom':
        theor = mygeom.rvs(.1, size = 500)
    else:
        raise ValueError('Distribution not allowed')
        return
    
    probs = np.arange(0, len(x_sample))/len(x_sample)
    x = np.quantile(theor,probs)
    y = np.quantile(x_sample,probs)
    
    plt.scatter(x, y)
    x = x.reshape(-1,1)
    y = y.reshape(-1,1)
    
    linear_regressor = LinearRegression() 
    linear_regressor.fit(x, y)
    Y_pred = linear_regressor.predict(x)
    plt.plot(x, Y_pred, color='red')
    score = r2_score(y, Y_pred)
    plt.text(s = '$R^2 = $' + str('%.3f'%score), x = 0.6*max(theor), y = .25*max(x_sample))
    if title != None: plt.title(title)
    plt.xlabel('Theoretical quantiles (' + dist.capitalize() + ')')
    plt.ylabel('Sample quantiles')

def plotDF(df, con_coef = None):
    gc.enable()
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
                    values = np.array(tmp.iloc[i]['vecvalue'])
                    times = np.array(tmp.iloc[i]['vectime'])
                    
                    plt.plot(times, values, marker = '.', markersize = 0.01)
                    errBand = z_critical*tmp.iloc[i]['StDev']/math.sqrt(tmp.iloc[i]['neff'])
                    lower = values - errBand
                    upper = values + errBand
                    
                    plt.fill_between(x = times, y1 = lower, y2 = upper, color='b', alpha=.1)
                    plt.title(name.split(':')[0] + " " + itervars + " (iter = " + str(i) + ")")
                    plt.savefig(name.split(':')[0] + " " + itervars + " (iter = " + str(i) + ").png")
                    #plt.show()
                    plt.clf()
                except OverflowError:
                    print(name + " " + itervars)
                    continue
                finally: gc.collect()
        break
    os.system('cp *.png ./' + SIM + '/plots')
    os.system('rm -f *.png')
    
def scatterDF(df, con_coef = None, deterministic = False):
    
    if deterministic == True:
        vectors = df.groupby(['iat', 'lt', 'tot', 'pt', 'name']).apply(lambda x: pd.Series({
            'mean': (x['value']).mean()})
            )
        vectors = vectors.reset_index()
        
        vectors['uf'] = vectors.apply (lambda row: (row['lt'] + row['tot'])/row['iat'], axis=1)
        vectors.to_csv('DeterministiRegimeStable.csv')
        vectors = vectors[['uf', 'pt','iat', 'name','mean']]
        
        vectors = vectors.groupby(['uf', 'pt', 'iat', 'name']).apply(lambda x: pd.Series({
            'mean': (x['mean']).mean()})
            )
        vectors = vectors.reset_index()
        for name in vectors.name.unique():
            plt.title(name.split(':')[0])
            plt.xlabel('Utilization Factor')
            plt.ylabel('Mean')
            s = []
            for pt in vectors.pt.unique():
                for iat in vectors['iat'].unique():
                    plt.plot(vectors[(vectors.name == name) & (vectors.pt == pt) & (vectors['iat'] == iat)]['uf'], vectors[(vectors.name == name) & (vectors.pt == pt) & (vectors['iat'] == iat)]['mean'])
                    s.append('pt=' + str(int(pt)) + ' iat=' + str(int(iat)))
                plt.legend(pd.Series(s))
            plt.show()
            plt.clf()
        return
    
    
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
    
    vectors.to_csv('./' + SIM + '/scatters/recap.csv')
    
    for name in vectors.name.unique():
        i = 1
        for pt in vectors.pt.unique():
            #s = []
            for row in vectors[(vectors.name == name) & (vectors.pt == pt)].itertuples():
                plt.scatter(row.iat, row.cmean)
                
                #to enable legend on plot, aside the dots
                if i%2 == 0: align ='right'
                else: align = 'left'
                i = i + 1
                plt.text(row.iat, row.cmean, s = 'tot= ' + str(row.tot) + '\nlt=' +  str(row.lt), fontsize = 10, ha = align, va = 'center')
                
                plt.errorbar(row.iat, row.cmean, yerr = (row.upper - row.lower))
                #s.append('tot= ' + str(row.tot) + ', lt=' +  str(row.lt))
            #plt.legend(pd.Series(s), loc='center left', bbox_to_anchor=(1, 0.5))
            plt.title(name.split(':')[0] + "(pt=" + str(row.pt) + ")")
            plt.savefig(name.split(':')[0] + " (pt=" + str(row.pt) + ").png", quality = .95)
            plt.xlabel('iat')
            #plt.show()
            plt.clf()
    
    os.system('cp *.png ./' + SIM + '/scatters')
    os.system('rm -f *.png')
    
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
    gc.enable()
    if os.path.isdir('./' + SIM) is False:
        os.mkdir('./' + SIM)
    if os.path.isdir('./' + SIM + '/hist') is False:
        os.mkdir('./' + SIM + '/hist')
    os.system('rm -rf ./' + SIM + '/hist/*')
    
    for itervars in df.iterationvars.unique():
        vectors = df[df.iterationvars == itervars][['iterationvars', 'name', 'vecvalue']]
        for name in vectors.name.unique():
            tmp = vectors[vectors.name == name]
            l = len(tmp)
            for i in range(l):
                try:
                    arr = tmp.iloc[i]['vecvalue']
                    mu = 1/arr.mean()
                    if name == 'HoldingQueueWaitingTime:vector' or name == 'DepartQueueWaitingTime:vector':
                        _, bins, _ = plt.hist(arr, bins = 30, density = 1)
                        x = np.arange(min(bins), max(bins), step = 0.5)
                        y = mu*np.exp(-1*mu*x)
                        plt.plot(x, y, color='r')
                        plt.text(np.mean(x), .75*max(y), s = "$\lambda=" + str('%.4f'%(1/mu)) + "$")
                        plt.title(name.split(':')[0] + " " + itervars + "(iter = " + str(i) + ")")
                        plt.tight_layout()
                    elif name == 'AirportResponseTime:vector': #Hypoexponential (parameters estimation via MLE)  
                        _, bins, _ = plt.hist(arr, bins = 30, density = 1)
                        x = np.arange(min(bins), max(bins), step = 0.5)
                        c = arr.std()/arr.mean()
                        mu = 2*mu
                        op = math.sqrt(1 + 2*(c**2 - 1))
                        l1 = mu/(1+op)
                        l2 = mu/(1-op)
                        y = l1*l2*(np.exp(-1*l2*x)-np.exp(-1*l1*x))/(l1-l2)
                        plt.plot(x, y, color='r')
                        plt.text(np.mean(x), .75*max(y), s = "$\lambda_1=" + str('%.4f'%l1) + "$ \n $\lambda_2=" + str('%.4f'%l2) +"$")
                        plt.title(name.split(':')[0] + " " + itervars + "(iter = " + str(i) + ")")
                        plt.tight_layout()
                        
                    elif name == 'HoldingQueueSize:vector' or name == 'DepartQueueSize:vector':
                        _, bins, _ = plt.hist(arr, bins = np.arange(round(min(arr)), round(max(arr)), step = 1), density = 1)
                        x = np.arange(min(bins), max(bins), step = 1)
                        y = mu*((1-mu)**(x-1))
                        plt.plot(x+.5, y, color='r')
                        plt.text(np.mean(x), .75*max(y), s = "$\lambda=" + str('%.4f'%(1/mu)) + "$")
                        plt.title(name.split(':')[0] + " " + itervars + "(iter = " + str(i) + ")")
                        plt.tight_layout()
                        
                    else:
                        ''' Holding Queue Size, Depart Queue Size, Parked Planes '''
                        _, bins, _ = plt.hist(arr, bins = np.arange(round(min(arr)), round(max(arr)), step = 1), density = 1)
                        
                        mu= 1/mu
                        x = np.arange(min(bins), max(bins), step = 1)
                        y = (np.exp(-1*mu)*mu**x)/factorial(x)
                        plt.plot(x+.5, y, color='r')          
                        plt.text(np.mean(x), .75*max(y), s = "$\lambda=" + str('%.4f'%(mu)) + "$")
                        plt.title(name.split(':')[0] + " " + itervars + "(iter = " + str(i) + ")")
                        plt.tight_layout()
                    #plt.show()
                     
                    plt.savefig(name.split(':')[0] + " " + itervars + " (iter=" + str(i) + ").png")
                    plt.clf()   
                    gc.collect()
                except BaseException as be:
                    print(be)
                    print(name)
                    print(itervars)
                    plt.clf() 
                    continue
                finally: 
                    gc.collect()
                    
    os.system('cp *.png ./' + SIM + '/hist')
    os.system('rm -f *.png')

def subsample(vector):
    p = .9
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

def neff(arr):
    '''
    References:
        http://www.metrology.pg.gda.pl/full/2010/M&MS_2010_003.pdf

    '''
    n = len(arr)
    acf = sm.tsa.acf(arr, nlags = n, fft = True, unbiased = False)
    sums = 0
    for k in range(0,n):
        sums = sums + (n-k)*(abs(acf[k]))/n
    
    return n/(1+2*sums)

class hexpon_gen(rv_continuous):
    '''
    Hyper-exponential distribution definition
    lamba1 = 2, lambda2 = 1
    '''
    def _pdf(self, x):
        return 2*(np.exp(-1*x) - np.exp(-2*x))
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

# F_w(iat, lt, tot, z)
def F_w(z, x, y, w): #Pr{LT + TOT - IAT <= w} (when w = 0 -> Pr{LT + TOT <= IAT}) 
    if w>=0:
        return 1 + y*z*np.exp(-1*x*w)/((x-y)*(x+z)) - x*z*np.exp(-1*y*w)/((x-y)*(y+z))
    return x*y*np.exp(z*w)/((y+z)*(x+z))
  
def makeInput(df):
    df = df[['name', 'value', 'iat', 'lt', 'tot', 'pt', 'iterationvars']]
    df = df.sort_values(by=['pt', 'tot', 'lt', 'iat'], ascending=[0,0,0,0])
    df.to_csv('./' + SIM + '/2kr/recap.csv')
    for name in df.name.unique():
        print(name)
        # print in order: iat lt tot pt 
        if(name.split(':')[1] != 'mean'): continue
        txt = []
        for iters in df.iterationvars.unique():
            txt.append(df[(df.name == name) & (df.iterationvars == iters)]['value'].values)
        filename = name.split(':')[0]
        np.savetxt('./' + SIM + '/2kr/inputs/' + filename, np.array(txt))
        os.system('./factorial2kr.py --confidence 0.95 --residuals ./' + SIM + '/2kr/residuals/residuals' + filename + ' --qqnorm ./' + SIM + '/2kr/qqnorm/qqnorm' + filename + ' ./' + SIM + '/2kr/inputs/' + filename + ' > ./' + SIM + '/2kr/results/results' + filename)
        qqnorm('./' + SIM + '/2kr/qqnorm/qqnorm' + filename, filename)

def qqnorm(file, title, dist = None):
    residualQuantiles = []
    if dist is None: dist = 'norm'
    with open(file) as f:
       for line in f:
           residualQuantiles.append(np.fromstring(line.rstrip('\n'), sep = ' ')[0])
    
    residualQuantiles = np.array(residualQuantiles)
    tmp = open('./' + SIM + '/2kr/qqnorm/errors' + title + '.txt', 'w') 
    tmp.write('Mean: ' + str(residualQuantiles.mean()) + '\nStDev: ' + str(residualQuantiles.std()) + '\nSkewness: ' + str(stats.skew(residualQuantiles)) + '\nMin: ' + str(min(residualQuantiles)) + '\nMax: ' + str(max(residualQuantiles)) + '\n25Q: ' + str(np.quantile(residualQuantiles, .25)) + '\n50Q: ' + str(np.quantile(residualQuantiles, .5)) + '\n75Q: ' + str(np.quantile(residualQuantiles, .75)))
    tmp.close()
    
    residualQuantiles = (residualQuantiles - residualQuantiles.mean())/residualQuantiles.std()
    #Sono davvero t student?
    
    plt.hist(residualQuantiles, bins = 30)
    plt.title('Errors for ' + title)
    plt.savefig('./' + SIM + '/2kr/qqnorm/qqnormHist' + title + '.png')
    plt.clf()
    
    residuals('./' + SIM + '/2kr/residuals/residuals' + title, title)
    
    pg.qqplot(residualQuantiles)
    plt.title('QQ-Plot of residuals for ' + title)
    plt.xlabel('Normal Quantile')
    plt.ylabel('Residual Quantile')
    plt.savefig('./' + SIM + '/2kr/qqnorm/qqnorm' + title + '.png')
    plt.clf()    
       
def residuals(file, title):
     residuals = []
     predictedResponse = []
     with open(file) as f:
        for line in f:
            tmp = np.fromstring(line.rstrip('\n'), sep = ' ')
            residuals.append(tmp[1])
            predictedResponse.append(tmp[0])
        plt.scatter(predictedResponse, residuals, marker = '.', s = .1)
        plt.xlabel('Predicted Response')
        plt.ylabel('Residuals')
        plt.title('Predicted Responses vs. Residuals for ' + title)
        plt.savefig('./' + SIM + '/2kr/residuals/residuals' + title + '.png')
        plt.clf()

def analysis2kr(s):
    if os.path.isdir('./' + SIM) is False:
        os.mkdir('./' + SIM)
    if os.path.isdir('./' + SIM + '/2kr') is False:
        os.mkdir('./' + SIM + '/2kr')
    os.system('rm -rf ./' + SIM + '/2kr/*')
    os.mkdir('./' + SIM + '/2kr/inputs')
    os.mkdir('./' + SIM + '/2kr/qqnorm')
    os.mkdir('./' + SIM + '/2kr/residuals')
    os.mkdir('./' + SIM + '/2kr/results')
    df = extractSca(s)
    makeInput(df)

def my_dist(z, x, y, size):
    ar = []
    for i in range(size):
        ar.append(stats.expon.rvs(x) + stats.expon.rvs(y) - stats.expon.rvs(z))
    return np.array(ar)

def mergeDF(directory):
    files = os.listdir(directory)
    first = True
    for file in files:
        if file.endswith("."+str('sca')):
            if first is True:
                os.system('scavetool x ' + directory + '/' + file + ' -o tmp.csv')
                print('cia0')
                df = extractSca('tmp.csv')
                first = False
            else:
                os.system('scavetool x ' + directory + '/' + file + ' -o tmp.csv')
                to_append = extractSca('tmp.csv')
                df = df.append(to_append)
    df.to_csv('recap.csv')