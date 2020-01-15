import numpy as np
import pandas as pd
import os
import fnmatch
import shutil
import seaborn as sns
from scipy.signal._savitzky_golay import * # savgol_filter
import scipy.stats as stats
import math

# Update the subsequent vector when adding new configuration to omnet.ini
SIMULATIONS = ['DeterministicRegimeOverloaded']

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
    os.system('rm -f *.csv')
    for sim in SIMULATIONS:
        os.system('scavetool x ' + sim + '*.sca -o ' + sim + '.csv')
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
    
    
    
    