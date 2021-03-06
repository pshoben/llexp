
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
grid = []
max_val = 0 

def flip_data(sub_index):
    global args 
    global flipped
    global grid
    global max_val

    for col_name in sub_index:
        col_data = []
        for x in data:
            if x.get('targ m/s') == args.rate : 
                col_data.append(float(x[col_name]))
                #index.append(x.get('name ----'))
        flipped[col_name]=col_data[:]

    for col_name in sub_index:
        col_data = []
        for x in data:
            if x.get('targ m/s') == args.rate : 
                val = float(x[col_name])
                if val > max_val:
                    max_val = val

                col_data.append(val)
                #index.append(x.get('name ----'))
        if len(col_data) > 0 :
            grid.append(col_data[:])

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
    global grid
    global data
    global max_val

    got_column_names = False 
    with open(args.filename) as infile:
        line = infile.readline()
        while line:
            line = infile.readline()
            if line.startswith("|") and not line[2]=='-':
                if not got_column_names:
                    add_col_names(line)
                    got_column_names = True
 
                else:
                    add_data_line(line)
    index = ['mutex','mut+drain','mutex cb','mut+dr cb','spinlock','spindrain','spin cb','spindr cb','lockfree','lockfr+dr']
    index_formatted = ['mutex std::queue','mutex std::queue (drain)',
                       'mutex boost::circular_buffer','mutex boost::circular_buffer (drain)',
                       'spinlock std::queue','spinlock std::queue drain',
                       'spinlock boost::circular_buffer','spinlock boost::circular_buffer (drain)',
                       'boost::lockfree::queue',
                       'boost::lockfree:queue (drain)']

    #sub_index = ['lat med ns','lat 90p ns','lat 95p ns','lat 99p ns','lat 99.5p ns','lat 99.7p ns','lat 99.9p ns']
    sub_index = ['lat 99.9p ns','lat 99.7p ns','lat 99.5p ns','lat 99p ns','lat 95p ns','lat 90p ns','lat med ns']

    formatted_sub_index = ['99.9th pct','99.7th pct','99.5th pct','99th pct','95th pct','90th pct','median']

    flip_data(sub_index)

    data = grid
    columns = index
    rows =  sub_index
 
    values = np.arange(0, max_val, max_val/5)
    value_increment = 5 
    
    # Get some pastel shades for the colors
    colors = plt.cm.BuPu(np.linspace(0, 0.5, len(rows)))
    n_rows = len(data)
    
    bvindex = np.arange(len(columns)) + 0.2
    bar_width = 0.5
    
    # Initialize the vertical-offset for the stacked bar chart.
    y_offset = np.zeros(len(columns))

    diffs = []
    for row in range(n_rows-1,-1,-1):
        y_diff = data[row][:]
        if row < len(data)-1:
            y_diff = []
            for i in range(len(data[row])):
                y_diff.append(data[row][i] - data[row+1][i]) 
                i+=1
        diffs.append(y_diff)
  
    # Plot bars and create text labels for the table
    cell_text = []
    for row in range(n_rows):
        plt.bar(bvindex, diffs[row], bar_width, bottom=y_offset, color=colors[row])
        y_offset = y_offset + diffs[row]
        cell_text.append(['%1.0f' % x for x in data[row]])
    # Reverse colors and text labels to display the last value at the top.
    colors = colors[::-1]
    #cell_text.reverse()
    
    # Add a table at the bottom of the axes
    the_table = plt.table(cellText=cell_text,
                          rowLabels=rows,
                          rowColours=colors,
                          colLabels=columns,
                          loc='bottom')
    
    # Adjust layout to make room for the table:
    plt.subplots_adjust(left=0.2, bottom=0.4)
    
    plt.ylabel("Latency (ns)".format(value_increment))

    vertical_ticks = []
    tick = 0
    while tick < max_val:
        vertical_ticks.append( tick ) 
        tick += 2000

    plt.yticks( vertical_ticks, ['%d' % val for val in vertical_ticks])
    plt.xticks([])
    plt.title('Queue Latency at ' + args.rate + " msgs/sec")

    output_file_name = args.filename.replace(".md","_bv_"+args.rate+".png")
    plt.savefig(output_file_name,
                 dpi=400,
                 format="png")

if __name__ == "__main__":
    getargs()
    plot()

