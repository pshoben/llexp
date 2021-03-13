#python3 plot_avb_barchart.py ../results/pingpong_compare_210313.md  rdtsc  cpu blocksize Pingpong-firehose
#python3 plot_avb_barchart.py ../results/pingpong_compare_210313.md  rdtscp cpu blocksize Pingpong-firehose

import numpy as np
import matplotlib.pyplot as plt
from argparse import ArgumentParser
import sys
import argparse

col_line=""
file_lines= []
data = []
groups = {}
bar_data = []
x_ticks = []

def getargs():
    global args
    parser = argparse.ArgumentParser("ab_barchart")
    parser.add_argument("filename")
    parser.add_argument("option")
    parser.add_argument("groupby")
    parser.add_argument("xticklabel")
    parser.add_argument("benchmark")
    args = parser.parse_args()
    print("filename " + args.filename)
    print("option " + args.option)
    print("groupby " + args.groupby)
    print("xticklabel " + args.xticklabel)
    print("benchmark " + args.benchmark)

def load_file():
    global args
    global col_line
    global file_lines
    got_column_names = False
    with open(args.filename) as infile:
        line = infile.readline()
        while line:
            line = infile.readline()
            if line.startswith("Run #2"):
                break;
            if line.startswith("|") and not line[2]=='-':
                if not got_column_names:
                    col_line = line
                    got_column_names = True
                else:
                    file_lines.append(line)
    return

def parse_file():
    global data
    col_names = col_line.split("|")[1:]

    for line in file_lines:
        row = {}
        line_arr = line.split("|")[1:]
        for i,cell in enumerate(line_arr):
            row[col_names[i].replace('-','').strip()]=cell.strip()
        data.append(row)
    return

def goc(the_map,key,default_val):
    ret = the_map.get(key)
    if ret is None:
        ret = default_val
        the_map[key]=default_val
    return ret

def build_groups():
    global groups
    for row in data:
        # filter out unwanted rows
        if row['option']==args.option:
            key = row[args.groupby]
            grp = goc(groups,key,[])
            grp.append(row)
    return

def build_bars():
    # TODO make config:

    global bar_data
    for key in groups:
        grp_data = {}
        bar_data.append(grp_data)
        for row in groups[key]:
            #bar = []
            min_val = float(row['min (ns)'])
            adj_min_val = min_val
            mean_val = float(row['avg (ns)'])
            mad_val = float(row['mad (ns)'])
            max_val = float(row['max (ns)'])

            # if the min is negative, there is a chart bug, we have to hack 2 extra bar chunks to workaround it
            neg_val = min_val
            if min_val < 0 :
                zero_val = 0.0
                min_val = abs(min_val)
            else:
                zero_val = min_val

            goc(grp_data,'neg',[]).append(neg_val) # neg values (may not be used)
            goc(grp_data,'zero',[]).append(zero_val)  # zero values (may not be used)
            goc(grp_data,'adj_min',[]).append(adj_min_val)  # min values (may not be used)
            goc(grp_data,'min',[]).append(min_val)  # min values (may not be used)
            goc(grp_data,'mean-mad',[]).append(mean_val-mad_val)  # mean-mad
            goc(grp_data,'mean',[]).append(mean_val)  # mean
            goc(grp_data,'mean+mad',[]).append(mean_val+mad_val)  # mean+mad
            goc(grp_data,'max',[]).append(max_val)  # max
    return

def build_xticks():
    global x_ticks
    for key in groups:
        for row in groups[key]:
            x_ticks.append(row[args.xticklabel])
        break # stop after first group
    return




def plot():
    global bar_data
    global x_ticks

    N = len(bar_data[0]['max']) # 10
    menMax = bar_data[0]['max'] # '[150, 160, 146, 172, 155,145, 149, 172, 165, 200]
    menPlusStd = bar_data[0]['mean+mad'] # # menMean[:]
    menMean = bar_data[0]['mean'] #[100, 100, 106, 102, 105,105, 109, 102, 105, 100]
    menMinusStd = bar_data[0]['mean-mad'] # menMean[:]
    menMin = bar_data[0]['adj_min'] # [20, 30, 32, 10, 20,30, 25, 20, 31, 22]
    menRealMin = bar_data[0]['min']
    menZero = bar_data[0]['zero']
    menNeg = bar_data[0]['neg']


    # for i in range(len(menPlusStd)):
    #     menPlusStd[i]+=10
    #     menMinusStd[i] -= 10

    fig, ax = plt.subplots()
    ax.set_ylabel('Write to Read Latency (ns)')
    ax.set_xlabel('Block size (bytes)')

    ind = np.arange(N)    # the x locations for the groups
    width = 0.35         # the width of the bars

    colors_blue = plt.cm.BuPu(np.linspace(0, 0.5, 6))
    #colors_blue[0]=[0,0,0,0]

    colors_orange = plt.cm.Oranges(np.linspace(0, 0.5, 6))
    #colors_orange[0]=[0,0,0,0]

    #pneg = ax.bar(ind, menZero, width, bottom=menNeg, color=colors_blue[4])
    #p2 = ax.bar(ind , menMin, width, bottom=menZero, color=colors_blue[2])
    #p1 = ax.bar(ind, menMinusStd, width, bottom=menMin, color=colors_blue[2])
    p1b = ax.bar(ind, menMean, width, bottom=menMinusStd, color=colors_blue[4])
    p1c = ax.bar(ind, menPlusStd, width, bottom=menMean, color=colors_blue[5])
    #p1d = ax.bar(ind, menMax, width, bottom=menPlusStd, color=colors_blue[2])

    womenMax = bar_data[1]['max'] # '[150, 160, 146, 172, 155,145, 149, 172, 165, 200]
    womenPlusStd = bar_data[1]['mean+mad'] # # menMean[:]
    womenMean = bar_data[1]['mean'] #[100, 100, 106, 102, 105,105, 109, 102, 105, 100]
    womenMinusStd = bar_data[1]['mean-mad'] # menMean[:]
    womenMin = bar_data[1]['adj_min']
    womenRealMin = bar_data[1]['min'] # [20, 30, 32, 10, 20,30, 25, 20, 31, 22]
    womenZero = bar_data[1]['zero']
    womenNeg = bar_data[1]['neg']

    # womenMax = [145, 149, 172, 165, 200,150, 160, 146, 172, 155]
    # womenMean = [110, 110, 126, 122, 125,125, 129, 122, 125, 120]
    # womenRealMin = [30, -25, -20, 31, 22,20, 30, 32, 10, 20]
    #
    # womenMin = [30, 0, 0, 31, 22,20, 30, 32, 10, 20]
    # womenZero = [30, 25, 20, 31, 22,20, 30, 32, 10, 20]
    # womenNeg = [30, -25, -20, 31, 22,20, 30, 32, 10, 20]

    # womenPlusStd = womenMean[:]
    # womenMinusStd = womenMean[:]
    # for i in range(len(womenPlusStd)):
    #     womenPlusStd[i]+=10
    #     womenMinusStd[i] -= 10

    #p2 = ax.bar(ind + width, womenMax, width, bottom=womenMean)#, yerr=womenStd)
    #pb = ax.bar(ind + width, womenMean, width, bottom=womenMin)#, yerr=womenStd)


    #pneg = ax.bar(ind + width, womenZero, width, bottom=womenNeg, color=colors_orange[4])
    #p2 = ax.bar(ind + width, womenMinusStd, width, bottom=womenMin, color=colors_orange[2])
    p2b = ax.bar(ind + width, womenMean, width, bottom=womenMinusStd, color=colors_orange[4])
    p2c = ax.bar(ind + width, womenPlusStd, width, bottom=womenMean, color=colors_orange[5])
    #p2d = ax.bar(ind + width, womenMax, width, bottom=womenPlusStd, color=colors_orange[2])


    ax.set_title(args.option + ' : benchmark #' + args.benchmark + "# CPU1/CPU2")
    ax.set_xticks(ind + width / 2)
    ax.set_xticklabels(x_ticks) # (('16', '20', '24', '28', '32','36', '40', '44', '48', '52'))

    ax.legend((p1c[0], p2c[0]), ('CPU1', 'CPU2'))


    # ----------------------------------

    # Plot bars and create text labels for the table
    cell_text = []
    vals = []
    for i in range(N):
        vals.append("{:.0f}".format(menMax[i])+'/'+"{:.0f}".format(womenMax[i]))
    cell_text.append(vals)

    vals = []
    for i in range(N):
        vals.append("{:.0f}".format(menPlusStd[i])+'/'+"{:.0f}".format(womenPlusStd[i]))
    cell_text.append(vals)

    vals = []
    for i in range(N):
        vals.append("{:.0f}".format(menMean[i])+'/'+"{:.0f}".format(womenMean[i]))
    cell_text.append(vals)

    vals = []
    for i in range(N):
        vals.append("{:.0f}".format(menMinusStd[i])+'/'+"{:.0f}".format(womenMinusStd[i]))
    cell_text.append(vals)

    vals = []
    for i in range(N):
        vals.append("{:.0f}".format(menMin[i])+'/'+"{:.0f}".format(womenRealMin[i]))
    cell_text.append(vals)

    col_labels = []
    for x in x_ticks:
        col_labels.append(str(x)+ " C1/C2")

    # Add a table at the bottom of the axes
    the_table = plt.table(cellText=cell_text,
                          rowLabels=['max latency (ns)','mean+mad latency (ns)','mean latency (ns)','mean-mad latency (ns)', 'min latency (ns)'],
                          #rowColours=colors,
                          colLabels=col_labels, #['16 C1/C2', '20 C1/C2','24 C1/C2','28 C1/C2','32 C1/C2','36 C1/C2','40 C1/C2','44 C1/C2','48 C1/C2','52 C1/C2'],
                          loc='bottom',
                          bbox=[0.25, -0.45, 0.9, .28]
                          )

    ax.autoscale_view()

    # Adjust layout to make room for the table:
    plt.subplots_adjust(left=0.1,bottom=0.3)


    output_file_name = args.filename.replace(".md","_avb_"+args.option+"_"+args.groupby+"_"+args.benchmark+".png")
    plt.savefig(output_file_name, dpi=600,
            format="png")

    #plt.yscale('log')
    #plt.savefig('test.png',dpi=600)
    plt.show()

if __name__ == "__main__":
    getargs()
    load_file()
    parse_file()
    build_groups()
    build_bars()
    build_xticks()
    plot()


