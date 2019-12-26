import sys,re,math,random
from scipy.stats import chisquare

C = 1.0

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

# nplots equals the number of filenames
nplots = len(data)
print 'nplots =',nplots

# nbins is the maximum number of bins over all plots
nbins = max(data[0])
for i in range(1,len(data)):
    if max(data[i]) > nbins:
        nbins = max(data[i])
print 'nbins =',nbins

# samplesize[i] is the number of samples found in filenames[i]
samplesize = []
for i,mvect in enumerate(data):
    n = len(mvect)
    samplesize.append(n)
    print '%d samples were found in file "%s"' % (n, filenames[i])

# Create a file "resolution-class-data.js" that contains a javascript variable "bins"
# containing an array of arrays of counts of resolution classes in each file examined.
# This file can be input into an html file using d3js to create plots using the "bins"
# variable as data.
outf = open('resolution-class-data.js','w')
outf.write('var svgfilename = "%s.svg";\n' % svgprefix)
outf.write('var labels = [%s];\n' % ','.join(['"%s"' % fn[:-2] for fn in filenames]))

# bins holds observed frequencies
print '\n************* Observed ****************\n'
outf.write('var bins = [\n')
for i,dvect in enumerate(data):
    bins = [0]*nbins
    for d in dvect:
        bins[d-1] += 1
    print 'sample size from file "%s" = %d' % (filenames[i],sum(bins))
    comma = (i < nplots-1) and ',' or ''
    outf.write(' [%s]%s\n' % (','.join(['%d' % b for b in bins]),comma))
    
    # construct expected counts
    total = float(sum(bins))
    xbins = [0]*nbins
    y = 1.0
    ysum = 0.0
    for b in range(nbins):
        i = nbins - b - 1
        ysum += y
        xbins[i] = y
        y *= C
    mult = total/ysum
    for i in range(nbins):
        xbins[i] *= mult
    
    # Perform chi-squared test on observed data
    N = sum(bins)
    print 'Observed:'
    for b in bins:
        print '%12d' % b,
    print
    print 'Expected:'
    for b in xbins:
        print '%12d' % b,
    print
    print 'P =',chisquare(bins, xbins)
    print 'Note: assuming C = %.3f' % C
        
outf.write('];\n')

# sbins holds frequencies from simulated data
print '\n************* Simulated ****************\n'
outf.write('var sbins = [\n')
for i in range(nplots):
    bins = [0]*nbins
    n = samplesize[i]
    
    # construct probability distribution
    p = [0]*nbins
    y = 1.0
    ysum = 0.0
    for b in range(nbins):
        k = nbins - b - 1
        ysum += y
        p[k] = y
        y *= C
    for b in range(nbins):
        p[b] /= ysum
        
    cump = [p[0]]
    #print 'cump[0] = %.5f' % cump[0]
    for b in range(1,nbins):
        cump.append(p[b] + cump[-1])
        #print 'cump[%d] = %.5f' % (b,cump[b])
    assert math.fabs(cump[-1] - 1.0) < 1.e-8, 'probabilities do not add to 1'
    
    # simulate data
    for j in range(n):  
        u = random.random()
        found = False
        for b in range(nbins):
            if u < cump[b]:
                bins[b] += 1
                found = True
                break
        assert found, 'fell through'

    # construct expected counts
    total = float(sum(bins))
    xbins = [total*prob for prob in p]
    
    comma = (i < nplots-1) and ',' or ''
    outf.write(' [%s]%s\n' % (','.join(['%d' % b for b in bins]),comma))
    
    # Perform chi-squared test on observed data
    N = sum(bins)
    print 'Observed:'
    for b in bins:
        print '%12d' % b,
    print
    print 'Expected:'
    for b in xbins:
        print '%12d' % b,
    print
    print 'P =',chisquare(bins, xbins)
    print 'Note: assuming C = %.3f' % C
        
outf.write('];\n')

outf.close()

print '\n'



