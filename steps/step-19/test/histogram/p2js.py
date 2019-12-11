import sys,re,math,random
from scipy.stats import chisquare

usage = 'usage: python p2js.py <svg-filename-prefix> <param-file-name> [ <param-file-name> [ <param-file-name> [...]]]'
assert len(sys.argv) > 1, usage
svgprefix = sys.argv[1]
filenames = sys.argv[2:]
print usage
print 'svg-filename-prefix =',svgprefix
for fn in filenames:
    print 'param-file-name =',fn
skip = 2    # number of rows to skip
col  = 4    # index of column containing resolution class

# Create a vector data in which data[i] is a list of
# the resolution classes found in column col of filenames[i]
data = []
for fn in filenames:
    d = []
    lines = open(fn, 'r').readlines()
    for line in lines[skip:]:
        parts = line.split()
        x = int(parts[col])
        d.append(x)
    data.append(d)

nplots = len(data)
print 'nplots =',nplots
nbins = max(data[0])
for i in range(1,len(data)):
    if max(data[i]) > nbins:
        nbins = max(data[i])
print 'nbins =',nbins

# Create a file "resolution-class-data.js" that contains a javascript variable "bins"
# containing an array of arrays of counts of resolution classes in each file examined.
# This file can be input into an html file using d3js to create plots using the "bins"
# variable as data.
outf = open('resolution-class-data.js','w')
outf.write('var svgfilename = "%s.svg";\n' % svgprefix)
outf.write('var labels = [%s];\n' % ','.join(['"%s"' % fn[:-2] for fn in filenames]))
outf.write('var bins = [\n')
for i,dvect in enumerate(data):
    bins = [0]*nbins
    for d in dvect:
        bins[d-1] += 1
    print 'sample size from file "%s" = %d' % (filenames[i],sum(bins))
    comma = (i < nplots-1) and ',' or ''
    outf.write(' [%s]%s\n' % (','.join(['%d' % b for b in bins]),comma))
    
    # Perform chi-squared test on observed data
    N = sum(bins)
    print 'Observed data:'
    for b in bins:
        print '%12d' % b,
    print
    print 'P =',chisquare(bins)
    
    # Perform chi-squared test on simulated data
    simbins = [0]*nbins
    for i in range(N):
        simbins[random.randint(0,nbins-1)] += 1
    print
    print 'Simulated data:'
    for b in simbins:
        print '%12d' % b,
    print
    print 'P =',chisquare(simbins)
        
outf.write('];\n')
outf.close()



