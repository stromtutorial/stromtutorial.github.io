import re,os,subprocess as sub,sys

paup_executable = 'paup4a166_osx'
data_file_path = '../rbcl10.nex'
tree_file_path = '../rbcl10.tre'

basefreq = "0.294667 0.189172 0.206055 0.310106"            
rmatrix  = "0.06082 0.27887 0.06461 0.06244 0.48492 0.04834"

partitioning = True
if partitioning:
    output_filename = 'output-partitioned.txt'
else:
    output_filename = 'output-unpartitioned.txt'

remove_temporary_files = False
temporary_files = ['rbcl1st.nex','rbcl2nd.nex','rbcl3rd.nex','tree1st.tre','tree2nd.tre','tree3rd.tre', 'output-all.txt', 'output1st.txt', 'output2nd.txt', 'output3rd.txt']

save_paup_output_files = True

def readNexusFile(fn):
    '''
    Reads nexus file whose name is specified by fn and returns ntax, nchar, taxa, and a
    sequences dictionary with taxon names as keys. The values ntax and nchar are integers,
    while taxa is a list of taxon names in the order they were found in the taxa block or
    data block. Any underscores in taxon names are converted to spaces before being saved
    in the taxa list or as a key in the sequences dictionary. Also all nexus comments
    (text in square brackets) will be ignored.
    '''
    stuff = open(fn, 'r').read()
    mask = None

    # determine if taxa block exists
    taxa_block = None
    m = re.search('(?:BEGIN|Begin|begin)\s+(?:TAXA|Taxa|taxa)\s*;(.+?)(?:END|End|end)\s*;', stuff, re.M | re.S)
    if m is not None:
        taxa_block = m.group(1).strip()

    # determine if characters block exists
    characters_block = None
    m = re.search('(?:BEGIN|Begin|begin)\s+(?:CHARACTERS|Characters|characters)\s*;(.+?)(?:END|End|end)\s*;', stuff, re.M | re.S)
    if m is not None:
        characters_block = m.group(1).strip()

    # determine if data block exists
    data_block = None
    m = re.search('(?:BEGIN|Begin|begin)\s+(?:DATA|Data|data)\s*;(.+?)(?:END|End|end)\s*;', stuff, re.M | re.S)
    if m is not None:
        data_block = m.group(1).strip()

    if data_block is not None:
        # get ntax and nchar
        m = re.search('(?:DIMENSIONS|dimensions|Dimensions)\s+(?:NTAX|ntax|Ntax|NTax)\s*=\s*(\d+)\s+(?:NCHAR|nchar|Nchar|NChar)\s*=\s*(\d+)\s*;', data_block, re.M | re.S)
        assert m, 'Could not decipher dimensions statement in data block'
        ntax = int(m.group(1))
        nchar = int(m.group(2))

        # get matrix
        m = re.search('(?:MATRIX|matrix|Matrix)\s+(.+?)\s*;', data_block, re.M | re.S)
        assert m, 'Could not decipher matrix statement in data block'
        lines = m.group(1).strip().split('\n')
        taxa = []
        sequences = {}
        for line in lines:
            m = re.match('\[([-*]+)\]', line.strip())
            if m is not None:
                mask = m.group(1)
            else:
                stripped_line = re.sub('\[.+?\]', '', line).strip()
                if len(stripped_line) > 0:
                    parts = line.split()
                    assert len(parts) == 2, 'Found more than 2 parts to this line:\n%s' % line
                    taxon_name = re.sub('_', ' ', parts[0]).strip()
                    taxa.append(taxon_name)
                    sequences[taxon_name] = parts[1]
    else:
        assert characters_block is not None and taxa_block is not None, 'Assuming nexus file contains either a data block or a taxa block and characters block'

        # get ntax from taxa block
        m = re.search('(?:DIMENSIONS|dimensions|Dimensions)\s+(?:NTAX|ntax|Ntax|NTax)\s*=\s*(\d+)\s*;', taxa_block, re.M | re.S)
        assert m, 'Could not decipher dimensions statement in taxa block'
        ntax = int(m.group(1))

        # get nchar from characters block
        m = re.search('(?:DIMENSIONS|dimensions|Dimensions)\s+(?:NCHAR|nchar|Nchar|NChar)\s*=\s*(\d+)\s*;', characters_block, re.M | re.S)
        assert m, 'Could not decipher dimensions statement in characters block'
        nchar = int(m.group(1))

        # get matrix from characters block
        m = re.search('(?:MATRIX|matrix|Matrix)\s+(.+?)\s*;', characters_block, re.M | re.S)
        assert m, 'Could not decipher matrix statement in characters block'
        lines = m.group(1).strip().split('\n')
        taxa = []
        sequences = {}
        for line in lines:
            m = re.match('\[([-*]+)\]', line.strip())
            if m is not None:
                mask = m.group(1)
            else:
                stripped_line = re.sub('\[.+?\]', '', line).strip()
                if len(stripped_line) > 0:
                    parts = stripped_line.split()
                    assert len(parts) == 2, 'Found more than 2 parts to this line:\n%s' % line
                    taxon_name = re.sub('_', ' ', parts[0]).strip()
                    taxa.append(taxon_name)
                    sequences[taxon_name] = parts[1]

    return (ntax, nchar, mask, taxa, sequences)

def writeNexusFile(fn, ntax, nchar, mask, taxa, sequences, paupblk):
    if os.path.exists(fn):
        os.rename(fn, '%s.bak' % fn)
    longest = max([len(t) for t in taxa])
    taxonfmt = '  %%%ds' % longest
    f = open(fn, 'w')
    f.write('#nexus\n\n')
    f.write('begin data;\n')
    f.write('  dimensions ntax=%d nchar=%d;\n' % (ntax, nchar))
    f.write('  format datatype=dna gap=-;\n')
    f.write('  matrix\n')
    if mask is not None:
        f.write(taxonfmt % ' ')
        f.write('[%s]\n' % mask)
    for t in taxa:
        taxon_name = re.sub('\s+', '_', t)
        f.write(taxonfmt % taxon_name)
        f.write(' %s\n' % sequences[t])
    f.write('  ;\n')
    f.write('end;\n\n')
    f.write('%s' % paupblk)
    f.close()
    
def readNexusTreeFile(fn):
    '''
    Reads nexus tree file whose name is specified by fn and returns ntax, ntrees,
    translate (a list of taxon number,name tuples), tree_names (a list of tree names in
    the order found in the file), and newicks (a list of newick tree descriptions in the
    order found in the file).
    '''
    stuff = open(fn, 'r').read()

    # determine if trees block exists
    trees_block = None
    m = re.search('(?:BEGIN|Begin|begin)\s+(?:TREES|Trees|trees)\s*;(.+?)(?:END|End|end)\s*;', stuff, re.M | re.S)
    if m is not None:
        trees_block = m.group(1).strip()

    if trees_block is not None:
        # get translate statement
        m = re.search('(?:TRANSLATE|Translate|translate)\s+(.+?)\s*;', trees_block, re.M | re.S)
        assert m, 'Could not decipher translate statement in trees block'
        number_name_pairs = m.group(1).split(',')
        ntax = len(number_name_pairs)
        translate = []
        for p in number_name_pairs:
            number_name = p.split()
            translate.append(tuple(number_name))

        # get trees
        tree_names_and_newicks = re.findall('^\s*tree\s+(.+?)\s*=\s*(.+?)\s*;', trees_block, re.M | re.S)
        tree_names = []
        newicks = []
        for tree_name,newick in tree_names_and_newicks:
            tree_names.append(tree_name)
            newicks.append(newick)
        ntrees = len(newicks)
        assert ntrees == len(tree_names)

    return (ntax, ntrees, translate, tree_names, newicks)

def scaleBrlens(newick, scaling_factor):
    # Break up newick string into parts
    parts = re.split('(:[0-9.e-]+)', newick)
    newdef = ''
    for p in parts:
        if p[0] == ':':
            newdef += ':'
            brlen = float(p[1:])*scaling_factor
            newdef += '%g' % brlen
        else:
            newdef += p
    return newdef
    
def writeNexusTreeFile(fn, ntax, ntrees, translate, tree_names, newicks): 
    if os.path.exists(fn):
        os.rename(fn, '%s.bak' % fn)
    f = open(fn, 'w')
    f.write('#nexus\n\n')
    f.write('begin trees;\n')
    f.write('	translate\n')
    for t in range(ntax-1):
        a,b = translate[t]
        f.write('		%s %s,\n' % (a,b))
    a,b = translate[ntax-1]
    f.write('		%s %s\n' % (a,b))
    f.write('		;\n')
    for t in range(ntrees):
        f.write("tree %s = [&U] %s;\n" % (tree_names[t], newicks[t]))
    f.write('end;\n')
    f.close()
    
if __name__ == '__main__':
    ntax, nchar, mask, taxa, sequences = readNexusFile(data_file_path)
    
    if not partitioning:
        # exchangeabilities: 0.08394222 0.34116704 0.03603322 0.15737940 0.30297095 0.07850717
        # normalized for paup: 1.06922998243 4.34568001878 0.458979988707 2.00465002114 3.85915006234 1.0
        paupcmd = "begin paup;\n  set crit=like storebrlen;\n  lset nst=6 basefreq=(%s) rmatrix=(%s) rates=gamma shape=0.517281;\n  gettrees file=%s;\n  lscores 1 / userbrlen;\n  quit;\nend;\n" % (basefreq, rmatrix, tree_file_path)
        writeNexusFile('rbcl-all.nex', ntax, nchar, mask, taxa, sequences, paupcmd)
        outf = open(output_filename, 'w')
        outf.write(paupcmd)
        paupout,pauperr = sub.Popen([paup_executable,'rbcl-all.nex'], stdout=sub.PIPE, stderr=sub.PIPE).communicate()
        if save_paup_output_files:
            tmpf = open('output-all.txt', 'w')
            tmpf.write(paupout)
            tmpf.write('\n')
            tmpf.close()
        m = re.search('-ln L\s+([0-9.]+)', paupout, re.M | re.S)
        assert m is not None
        loglike = -float(m.group(1))
        outf.write('log-likelihood: %.5f' % loglike)
        outf.close()
    else:
        # Split rbcL.nex into 3 data sets corresponding to 1st, 2nd, and 3rd codon positions
        sequences1st = {}
        sequences2nd = {}
        sequences3rd = {}
        for t in taxa:
            seq = sequences[t]
            s1 = ''
            s2 = ''
            s3 = ''
            for i in range(0,nchar,3):
                s1 += seq[i+0]
                s2 += seq[i+1]
                s3 += seq[i+2]
            sequences1st[t] = s1
            sequences2nd[t] = s2
            sequences3rd[t] = s3
        
        paup1st = "begin paup;\n  set crit=like storebrlen;\n  lset nst=6 basefreq=(%s) rmatrix=(%s) rates=gamma shape=1.0 pinvar=0.1;\n  gettrees file=tree1st.tre;\n  lscores 1 / userbrlen;\n  describetrees 1 / plot=none brlens=sumonly;\n  quit;\nend;\n" % (basefreq, rmatrix)
        paup2nd = "begin paup;\n  set crit=like storebrlen;\n  lset nst=6 basefreq=(%s) rmatrix=(%s) rates=gamma shape=1.0 pinvar=0.1;\n  gettrees file=tree2nd.tre;\n  lscores 1 / userbrlen;\n  describetrees 1 / plot=none brlens=sumonly;\n  quit;\nend;\n" % (basefreq, rmatrix)
        paup3rd = "begin paup;\n  set crit=like storebrlen;\n  lset nst=6 basefreq=(%s) rmatrix=(%s) rates=gamma shape=1.0 pinvar=0.1;\n  gettrees file=tree3rd.tre;\n  lscores 1 / userbrlen;\n  describetrees 1 / plot=none brlens=sumonly;\n  quit;\nend;\n" % (basefreq, rmatrix)
        
        writeNexusFile('rbcl1st.nex', ntax, nchar//3, mask, taxa, sequences1st, paup1st)
        writeNexusFile('rbcl2nd.nex', ntax, nchar//3, mask, taxa, sequences2nd, paup2nd)
        writeNexusFile('rbcl3rd.nex', ntax, nchar//3, mask, taxa, sequences3rd, paup3rd)   
    
        # Read tree from treefile and save into 3 tree files with branch lengths multiplied by subset_rate values
        relrate1st =  1.0
        relrate2nd =  2.0
        relrate3rd =  3.0
        
        # Normalize the relative rates to have mean 1
        sum_relrates = relrate1st + relrate2nd + relrate3rd
        relrate1st *= 3./sum_relrates
        relrate2nd *= 3./sum_relrates
        relrate3rd *= 3./sum_relrates
        print 'relrate1st = ',relrate1st
        print 'relrate2nd = ',relrate2nd
        print 'relrate3rd = ',relrate3rd
        print 'mean relative rate = ',(relrate1st + relrate2nd + relrate3rd)/3.
    
        ntax, ntrees, translate, tree_names, newicks = readNexusTreeFile(tree_file_path)
        assert ntrees == 1
        newicks1st = scaleBrlens(newicks[0], relrate1st)
        newicks2nd = scaleBrlens(newicks[0], relrate2nd)
        newicks3rd = scaleBrlens(newicks[0], relrate3rd)
        writeNexusTreeFile('tree1st.tre', ntax, ntrees, translate, tree_names, [newicks1st])
        writeNexusTreeFile('tree2nd.tre', ntax, ntrees, translate, tree_names, [newicks2nd])
        writeNexusTreeFile('tree3rd.tre', ntax, ntrees, translate, tree_names, [newicks3rd])

        outf = open(output_filename, 'w')
    
        outf.write('First subset:\n')
        outf.write('-------------\n')
        outf.write('Subset rate: %.1f\n' % relrate1st)
        outf.write(paup1st)
        print 'Working on 1st...'
        paupout,pauperr = sub.Popen([paup_executable,'rbcl1st.nex'], stdout=sub.PIPE, stderr=sub.PIPE).communicate()
        if save_paup_output_files:
            tmpf = open('output1st.txt', 'w')
            tmpf.write(paupout)
            tmpf.write('\n')
            tmpf.close()
        m = re.search('-ln L\s+([0-9.]+)', paupout, re.M | re.S)
        assert m is not None
        loglike1st = -float(m.group(1))
        outf.write('log-likelihood: %.5f' % loglike1st)
    
        outf.write('\n\n')
    
        outf.write('Second subset:\n')
        outf.write('--------------\n')
        outf.write('Subset rate: %.1f\n' % relrate2nd)
        outf.write(paup2nd)
        print 'Working on 2nd...'
        paupout,pauperr = sub.Popen([paup_executable,'rbcl2nd.nex'], stdout=sub.PIPE, stderr=sub.PIPE).communicate()
        if save_paup_output_files:
            tmpf = open('output2nd.txt', 'w')
            tmpf.write(paupout)
            tmpf.write('\n')
            tmpf.close()
        m = re.search('-ln L\s+([0-9.]+)', paupout, re.M | re.S)
        assert m is not None
        loglike2nd = -float(m.group(1))
        outf.write('log-likelihood: %.5f' % loglike2nd)
    
        outf.write('\n\n')
    
        outf.write('Third subset:\n')
        outf.write('-------------\n')
        outf.write('Subset rate: %.1f\n' % relrate3rd)
        outf.write(paup3rd)
        print 'Working on 3rd...'
        paupout,pauperr = sub.Popen([paup_executable,'rbcl3rd.nex'], stdout=sub.PIPE, stderr=sub.PIPE).communicate()
        if save_paup_output_files:
            tmpf = open('output3rd.txt', 'w')
            tmpf.write(paupout)
            tmpf.write('\n')
            tmpf.close()
        m = re.search('-ln L\s+([0-9.]+)', paupout, re.M | re.S)
        assert m is not None
        loglike3rd = -float(m.group(1))
        outf.write('log-likelihood: %.5f' % loglike3rd)
    
        outf.write('\n\n')

        outf.write('Total log-likelihood: %.5f' % (loglike1st + loglike2nd + loglike3rd,))
    
        outf.close()
    
    if remove_temporary_files:
        for f in temporary_files:
            if os.path.exists(f):
                os.remove(f)
             