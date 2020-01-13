import numpy as np
import pandas as pd
import os
import fnmatch
import shutil

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
    vectors = vectors[vectors.type=='vector']
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
    os.system('rm -f *.sca *.out *.vci *.vec')

