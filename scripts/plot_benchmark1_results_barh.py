# pip3 install pandas matplotlib
#python3 plot_benchmark1_results_barh.py benchmark1_compare_210302.md

from argparse import ArgumentParser

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

#import matplotlib
#matplotlib.use("Agg") # run on headless server
#import matplotlib.pyplot as plt
#import numpy as np

import sys
import argparse

col_names = []
data = []
flipped = {}
index = []

def flip_data(sub_index):
    global args 
    global flipped
    for col_name in sub_index:
        col_data = []
        for x in data:
            if x.get('targ m/s') == args.rate : 
                col_data.append(float(x[col_name]))
                #index.append(x.get('name ----'))
        flipped[col_name]=col_data[:]
    #index=[]
    #for x in data:
    #    if x.get('targ m/s') =='1000': 
    #        index.append(x.get('name ----'))


def getargs():
    global args
    parser = argparse.ArgumentParser("make_call_graph")
    parser.add_argument("filename")
    parser.add_argument("rate")
    args = parser.parse_args()

def add_col_names(line):
    global col_names
    col_names_arr = line.split("|")
    for col_name in col_names_arr:
        col_names.append(col_name.strip())

def add_data_line(line):
    global data
    cells = {} 
    line_arr = line.split("|")
    count=0
    for cell in line_arr:
        cells[col_names[count]]=cell.strip()
        count+=1
    data.append(cells)


def plot():
    global args
    global index

    print( " filename = " + args.filename)
    got_column_names = False 
    with open(args.filename) as infile:
        line = infile.readline()
        while line:
            line = infile.readline()
            if line.startswith("|") and not line[2]=='-':
                if not got_column_names:
                    #print("cols:" + line)
                    add_col_names(line)
                    got_column_names = True
 
                else:
                    #print("data:" + line)
                    add_data_line(line)
    index = ['mutex','mut+drain','mutex cb','mut+dr cb','spinlock','spindrain','spin cb','spindr cb','lockfree','lockfr+dr']
    index_formatted = ['mutex std::queue','mutex std::queue (drain)',
                       'mutex boost::circular_buffer','mutex boost::circular_buffer (drain)',
                       'spinlock std::queue','spinlock std::queue drain',
                       'spinlock boost::circular_buffer','spinlock boost::circular_buffer (drain)',
                       'boost::lockfree::queue',
                       'boost::lockfree:queue (drain)']


    sub_index = ['lat med ns','lat 90p ns','lat 90p ns','lat 95p ns','lat 99p ns','lat 99.5p ns','lat 99.7p ns','lat 99.9p ns']


    #print(col_names)
    print(data)

    flip_data(sub_index)

    #print(flipped)
    #print(index)

    df = pd.DataFrame( flipped, index=index)

    #df = pandas.DataFrame(dict(graph=['Item one', 'Item two', 'Item three'],
    #                           n=[3, 5, 2], m=[6, 1, 3])) 
    
    ind = np.arange(len(df))
    width = 0.4

    ax = df.plot.barh() # subplots(xcount, ycount, sharex=True)
   
    #fig, ax = plt.subplots()
    #ax.barh(ind, df.n, width, color='red', label='N')
    #ax.barh(ind + width, df.m, width, color='green', label='M')
    
    #ax.set(yticks=ind + width, yticklabels=df.graph, ylim=[2*width - 1, len(df)])
    #ax.legend()
    
    #plt.show() 
    #fig2 = df.gcf()
    output_file_name = args.filename.replace(".md","_"+args.rate+".png")
    plt.savefig(output_file_name,
                 dpi=100,
                 format="png")



#    fig,ax = df.plot_barh()
#    fig.savefig("test.png")
#    plt.close(fig)
#
#    fig, axs = plt.plot_barh(df) # subplots(xcount, ycount, sharex=True)
##    for ik, key in enumerate(category_map.keys()):
##        plot_row(category_map[key],ik,0,key,axs,title)
#    fig2 = plt.gcf()
#    #plt.show()
#    file_format="png"
#    output_file_name = "test.png"
#    # args.input_file.replace(".csv","")
#    #if args.ignore_name is not None:
#    #    output_file_name = output_file_name + "_ignore_" + args.ignore_name
#    #output_file_name = output_file_name + ".png"
#    fig2.savefig(output_file_name,
#                 dpi=100,
#                 format=file_format,
#                 quality=100)

if __name__ == "__main__":
    getargs()
    plot()

