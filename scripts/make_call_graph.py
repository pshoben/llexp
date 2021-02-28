#!/usr/bin/python2

import sys
import argparse

top = None
grand_total_cycles = 1

# find the entry at the key, if not exists, create an empty default map it and return that
def goc(map,key,default_val):
    x = map.get(key)
    if x is None:
        x = default_val 
        map[key]=x
    return x
    
def getargs():
    global args
    parser = argparse.ArgumentParser("make_call_graph")
    parser.add_argument("filename")
    args = parser.parse_args()

def make_path(path_arr, parent,cycles):
    if len(path_arr)>0:
       first = path_arr[0]
       remaining = path_arr[1:]
       node = { "name": first, "children": [], "total_cycles": cycles }
       parent.append(node)
       make_path(remaining, node.get("children"), cycles)
       if len(path_arr)==1:
           node["cycles"]=cycles 
 
def render( a, indent ):
    global grand_total_cyclesa
    cycles = a["total_cycles"]
    print indent + "{ " + a["name"] + "   self " + str(a.get("cycles")) +  "   incl " + str(cycles) + " (" + '{0:.3g}'.format( float(cycles) * 100.0 / float(grand_total_cycles) ) + "%)"
    for child in a["children"]:
        render( child, indent + "  ")
    print indent + "}"

 
def merge_nodes(a,b):
    #print "merging "
    #render(a,"")
    #print " with:"
    #render( b,"")
    if a["name"]==b["name"]:
        a["total_cycles"]+=b["total_cycles"]
        for b_child in b["children"]:
            found_child_match = False
            for a_child in a["children"]:
                child_match = merge_nodes(a_child,b_child)
                if child_match:
                    found_child_match = True
            if not found_child_match:
                # no match found, add it to the end
                a["children"].append(b_child) 
        return True
    return False   


def add_node(line):
    #print "adding " + line
    global top
    arr = line.split()
    path_arr = arr[0].split(";")
    cycles = int(arr[1])
    #print str(path_arr)
    #print str(cycles)
    node = [] 
    make_path(path_arr,node,cycles)
    #print str(node)
    if top is None:
      top = node[:]
    else:
      for top_child in top:
          merge_nodes(top_child,node[0])

if __name__ == "__main__":
    getargs()
    global args
    global top
    global grand_total_cycles

    print " filename = " + args.filename
     
    with open(args.filename) as infile:
        line = infile.readline()
        while line:
            linenum = add_node(line)
            line = infile.readline()
    grand_total_cycles = 0
    for child in top or []:
        grand_total_cycles+=child["total_cycles"]
    print "grand total = " + str(grand_total_cycles) 
    print "after merge : "
    for child in top or []:
        render(child,"")
            
